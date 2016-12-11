#include <tacopie/network/io_service.hpp>
#include <tacopie/error.hpp>
#include <tacopie/logger.hpp>

#include <fcntl.h>
#include <unistd.h>

namespace tacopie {

//!
//! default io_service getter & setter
//!

static std::shared_ptr<io_service> io_service_default_instance = nullptr;

const std::shared_ptr<io_service>&
get_default_io_service(void) {
  if (io_service_default_instance == nullptr)
    { io_service_default_instance = std::make_shared<io_service>(); }

  return io_service_default_instance;
}

void
set_default_io_service(const std::shared_ptr<io_service>& service) {
  if (service) {
    __TACOPIE_LOG(debug, "setting new default_io_service");
    io_service_default_instance = service;
  }
  else {
    __TACOPIE_LOG(warn, "setting new default_io_service failed because the service is null");
  }
}

//!
//! ctor & dtor
//!

io_service::io_service(void)
: m_should_stop(false)
, m_callback_workers(__TACOPIE_IO_SERVICE_NB_WORKERS)
, m_notif_pipe_fds{-1, -1}
{
  __TACOPIE_LOG(debug, "create io_service");

  if (pipe(m_notif_pipe_fds) == -1)
    { __TACOPIE_THROW(error, "pipe() failure"); }

  //! Start worker after everything has been initialized
  m_poll_worker = std::thread(std::bind(&io_service::poll, this));
}

io_service::~io_service(void) {
  __TACOPIE_LOG(debug, "destroy io_service");

  m_should_stop = true;

  wake_up();
  m_poll_worker.join();
  m_callback_workers.stop();

  close(m_notif_pipe_fds[0]);
  close(m_notif_pipe_fds[1]);
}

//!
//! poll worker function
//!

void
io_service::poll(void) {
  __TACOPIE_LOG(debug, "starting poll() worker");

  while (not m_should_stop) {
    init_poll_fds_info();

    __TACOPIE_LOG(debug, "polling fds");
    if (::poll(const_cast<struct pollfd*>(m_poll_fds_info.data()), m_poll_fds_info.size(), -1) > 0)
      { process_events(); }
    else
      { __TACOPIE_LOG(debug, "poll woke up, but nothing to process"); }
  }

  __TACOPIE_LOG(debug, "stop poll() worker");
}

//!
//! process poll detected events
//!

void
io_service::process_events(void) {
  std::lock_guard<std::mutex> lock(m_tracked_sockets_mtx);

  __TACOPIE_LOG(debug, "processing events");

  for (const auto& poll_result : m_poll_fds_info) {
    if (poll_result.fd == m_notif_pipe_fds[0] and poll_result.revents & POLLIN) {
      process_wake_up_event();
      continue ;
    }

    auto it = m_tracked_sockets.find(poll_result.fd);

    if (it == m_tracked_sockets.end())
      { continue ; }

    auto& socket = it->second;

    if (poll_result.revents & POLLIN and socket.rd_callback and not socket.is_executing_rd_callback)
      { process_rd_event(poll_result, socket); }

    if (poll_result.revents & POLLOUT and socket.wr_callback and not socket.is_executing_wr_callback)
      { process_wr_event(poll_result, socket); }
  }
}

void
io_service::process_wake_up_event(void) {
  __TACOPIE_LOG(debug, "processing wake_up event");

  char buf[1024];
  (void) read(m_notif_pipe_fds[0], buf, 1024);
}

void
io_service::process_rd_event(const struct pollfd& poll_result, tracked_socket& socket) {
  __TACOPIE_LOG(debug, "processing read event");

  auto rd_callback = socket.rd_callback;
  auto fd = poll_result.fd;

  socket.is_executing_rd_callback = true;

  m_callback_workers << [=] {
    __TACOPIE_LOG(debug, "execute read callback");
    rd_callback(fd);

    std::lock_guard<std::mutex> lock(m_tracked_sockets_mtx);
    auto it = m_tracked_sockets.find(fd);

    if (it == m_tracked_sockets.end())
      { return ; }

    auto& socket = it->second;
    socket.is_executing_rd_callback = false;

    if (socket.marked_for_untrack && not socket.is_executing_wr_callback) {
      __TACOPIE_LOG(debug, "untrack socket");
      m_tracked_sockets.erase(it);
    }
  };
}

void
io_service::process_wr_event(const struct pollfd& poll_result, tracked_socket& socket) {
  __TACOPIE_LOG(debug, "processing write event");

  auto wr_callback = socket.wr_callback;
  auto fd = poll_result.fd;

  socket.is_executing_wr_callback = true;

  m_callback_workers << [=] {
    __TACOPIE_LOG(debug, "execute write callback");
    wr_callback(fd);

    std::lock_guard<std::mutex> lock(m_tracked_sockets_mtx);
    auto it = m_tracked_sockets.find(fd);

    if (it == m_tracked_sockets.end())
      { return ; }

    auto& socket = it->second;
    socket.is_executing_wr_callback = false;

    if (socket.marked_for_untrack && not socket.is_executing_rd_callback) {
      __TACOPIE_LOG(debug, "untrack socket");
      m_tracked_sockets.erase(it);
    }
  };
}

//!
//! init m_poll_fds_info
//!

void
io_service::init_poll_fds_info(void) {
  std::lock_guard<std::mutex> lock(m_tracked_sockets_mtx);

  m_poll_fds_info.clear();

  for (const auto& socket : m_tracked_sockets) {
    const auto& fd = socket.first;
    const auto& socket_info = socket.second;

    if (not socket_info.rd_callback and not socket_info.wr_callback)
      { continue ; }

    if (socket_info.marked_for_untrack)
      { continue ; }

    struct pollfd poll_fd_info;
    poll_fd_info.fd = fd;
    poll_fd_info.events = 0;
    poll_fd_info.revents = 0;

    if (socket_info.rd_callback and not socket_info.is_executing_rd_callback)
      { poll_fd_info.events |= POLLIN; }

    if (socket_info.wr_callback and not socket_info.is_executing_wr_callback)
      { poll_fd_info.events |= POLLOUT; }

    if (poll_fd_info.events)
      { m_poll_fds_info.push_back(std::move(poll_fd_info)); }
  }

  m_poll_fds_info.push_back({ m_notif_pipe_fds[0], POLLIN, 0 });
}

//!
//! track & untrack socket
//!

void
io_service::track(const tcp_socket& socket, const event_callback_t& rd_callback, const event_callback_t& wr_callback) {
  std::lock_guard<std::mutex> lock(m_tracked_sockets_mtx);

  __TACOPIE_LOG(debug, "track new socket");

  auto& track_info = m_tracked_sockets[socket.get_fd()];
  track_info.rd_callback = rd_callback;
  track_info.wr_callback = wr_callback;
  track_info.marked_for_untrack = false;

  wake_up();
}

void
io_service::set_rd_callback(const tcp_socket& socket, const event_callback_t& event_callback) {
  std::unique_lock<std::mutex> lock(m_tracked_sockets_mtx);

  __TACOPIE_LOG(debug, "update read socket tracking callback");

  auto& track_info = m_tracked_sockets[socket.get_fd()];
  track_info.rd_callback = event_callback;
  track_info.marked_for_untrack = false;

  wake_up();
}

void
io_service::set_wr_callback(const tcp_socket& socket, const event_callback_t& event_callback) {
  std::unique_lock<std::mutex> lock(m_tracked_sockets_mtx);

  __TACOPIE_LOG(debug, "update write socket tracking callback");

  auto& track_info = m_tracked_sockets[socket.get_fd()];
  track_info.wr_callback = event_callback;
  track_info.marked_for_untrack = false;

  wake_up();
}

void
io_service::untrack(const tcp_socket& socket) {
  std::unique_lock<std::mutex> lock(m_tracked_sockets_mtx);

  auto it = m_tracked_sockets.find(socket.get_fd());

  if (it == m_tracked_sockets.end())
    { return ; }

  if (it->second.is_executing_rd_callback or it->second.is_executing_wr_callback) {
    __TACOPIE_LOG(debug, "mark socket for untracking");
    it->second.marked_for_untrack = true;
  }
  else {
    __TACOPIE_LOG(debug, "untrack socket");
    m_tracked_sockets.erase(it);
  }

  wake_up();
}

//!
//! force poll to wake-up
//!

void
io_service::wake_up(void) {
  __TACOPIE_LOG(debug, "wake up poll");
  (void) write(m_notif_pipe_fds[1], "a", 1);
}

} //! tacopie
