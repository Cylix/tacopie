#include <cpp_http_server/network/io_service.hpp>

namespace cpp_http_server {

namespace network {

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
  if (service)
    { io_service_default_instance = service; }
}

//!
//! ctor & dtor
//!

io_service::io_service(void)
: m_should_stop(false)
, m_poll_worker(std::bind(&io_service::poll, this))
, m_callback_workers(__CPP_HTTP_SERVER_IO_SERVICE_NB_WORKERS)
{}

io_service::~io_service(void) {
  m_should_stop = true;
  m_poll_worker.join();
}

//!
//! poll worker function
//!

void
io_service::poll(void) {
  while (not m_should_stop) {
    init_poll_fds_info();

    if (::poll(const_cast<struct pollfd*>(m_poll_fds_info.data()), m_poll_fds_info.size(), 0) > 0)
      { process_events(); }
  }
}

//!
//! process poll detected events
//!

void
io_service::process_events(void) {
  std::lock_guard<std::mutex> lock(m_tracked_sockets_mtx);

  for (const auto& poll_result : m_poll_fds_info) {
    auto it = m_tracked_sockets.find(poll_result.fd);

    if (it == m_tracked_sockets.end())
      { continue ; }


    if (poll_result.revents & POLLIN and it->second.rd_callback) {
      ++it->second.executing_callback;

      auto rd_callback = it->second.rd_callback;
      m_callback_workers << [&, rd_callback] {
        rd_callback(poll_result.fd);

        --it->second.executing_callback;
        it->second.executing_callback_condvar.notify_all();
      };
    }

    if (poll_result.revents & POLLOUT and it->second.wr_callback) {
      ++it->second.executing_callback;

      auto wr_callback = it->second.wr_callback;
      m_callback_workers << [&, wr_callback] {
        wr_callback(poll_result.fd);

        --it->second.executing_callback;
        it->second.executing_callback_condvar.notify_all();
      };
    }
  }
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

    struct pollfd poll_fd_info;
    poll_fd_info.fd = fd;
    poll_fd_info.events = 0;

    if (socket_info.rd_callback)
      { poll_fd_info.events |= POLLIN; }

    if (socket_info.wr_callback)
      { poll_fd_info.events |= POLLOUT; }

    m_poll_fds_info.push_back(std::move(poll_fd_info));
  }
}

//!
//! track & untrack socket
//!

void
io_service::track(const tcp_socket& socket, const event_callback_t& rd_callback, const event_callback_t& wr_callback) {
  std::lock_guard<std::mutex> lock(m_tracked_sockets_mtx);

  auto& track_info = m_tracked_sockets[socket.get_fd()];
  track_info.rd_callback = rd_callback;
  track_info.wr_callback = wr_callback;
}

void
io_service::set_rd_callback(const tcp_socket& socket, const event_callback_t& event_callback) {
  std::unique_lock<std::mutex> lock(m_tracked_sockets_mtx);

  auto& track_info = m_tracked_sockets[socket.get_fd()];
  wait_for_callback_completion(lock, track_info);
  track_info.rd_callback = event_callback;
}

void
io_service::set_wr_callback(const tcp_socket& socket, const event_callback_t& event_callback) {
  std::unique_lock<std::mutex> lock(m_tracked_sockets_mtx);

  auto& track_info = m_tracked_sockets[socket.get_fd()];
  wait_for_callback_completion(lock, track_info);
  track_info.wr_callback = event_callback;
}

void
io_service::untrack(const tcp_socket& socket) {
  std::unique_lock<std::mutex> lock(m_tracked_sockets_mtx);

  auto it = m_tracked_sockets.find(socket.get_fd());
  wait_for_callback_completion(lock, it->second);
  m_tracked_sockets.erase(it);
}

//!
//! wait for callback completion
//!

void
io_service::wait_for_callback_completion(std::unique_lock<std::mutex>& lock, tracked_socket& socket) {
  socket.executing_callback_condvar.wait(lock, [&] {
    return socket.executing_callback == 0;
  });
}

} //! network

} //! cpp_http_server
