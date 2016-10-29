#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include <poll.h>

#include <cpp_http_server/network/tcp_socket.hpp>
#include <cpp_http_server/utils/thread_pool.hpp>

#define __CPP_HTTP_SERVER_IO_SERVICE_NB_WORKERS 1

namespace cpp_http_server {

namespace network {

class io_service {
public:
  //! ctor & dtor
  io_service(void);
  ~io_service(void);

  //! copy ctor & assignment operator
  io_service(const io_service&) = delete;
  io_service& operator=(const io_service&) = delete;

public:
  //! callback handler typedef
  //! called on new socket event if register to io_service
  typedef std::function<void(fd_t)> event_callback_t;

  //! track & untrack socket
  void track(const tcp_socket& socket, const event_callback_t& rd_callback = nullptr, const event_callback_t& wr_callback = nullptr);
  void set_rd_callback(const tcp_socket& socket, const event_callback_t& event_callback);
  void set_wr_callback(const tcp_socket& socket, const event_callback_t& event_callback);
  void untrack(const tcp_socket& socket);

  //! force poll to wake-up
  void wake_up(void);

private:
  //! struct tracked_socket
  //! contains information about what a current socket is tracking
  struct tracked_socket {
    //! ctor
    tracked_socket(void)
      : rd_callback(nullptr)
      , is_executing_rd_callback(false)
      , wr_callback(nullptr)
      , is_executing_wr_callback(false)
      {}

    //! rd event
    event_callback_t rd_callback;
    std::atomic_bool is_executing_rd_callback;

    //! wr event
    event_callback_t wr_callback;
    std::atomic_bool is_executing_wr_callback;

    //! is executing callback
    std::condition_variable executing_callback_condvar;
  };

private:
  //! poll worker function
  void poll(void);

  //! init m_poll_fds_info
  void init_poll_fds_info(void);

  //! process poll detected events
  void process_events(void);
  void process_rd_event(const struct pollfd& poll_result, tracked_socket& socket);
  void process_wr_event(const struct pollfd& poll_result, tracked_socket& socket);

private:
  //! tracked sockets
  std::unordered_map<fd_t, tracked_socket> m_tracked_sockets;

  //! whether the worker should stop or not
  std::atomic_bool m_should_stop;

  //! poll thread
  std::thread m_poll_worker;

  //! callback workers
  utils::thread_pool m_callback_workers;

  //! thread safety
  std::mutex m_tracked_sockets_mtx;

  //! data structure given to poll
  std::vector<struct pollfd> m_poll_fds_info;

  //! fd associated to the pipe used to wake up the poll call
  int m_notif_pipe_fds[2];
};

//! default io_service getter & setter
const std::shared_ptr<io_service>& get_default_io_service(void);
void set_default_io_service(const std::shared_ptr<io_service>&);

} //! network

} //! cpp_http_server
