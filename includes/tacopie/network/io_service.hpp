// MIT License
//
// Copyright (c) 2016-2017 Simon Ninon <simon.ninon@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#include <Winsock2.h>
#else
#include <poll.h>
#endif /* _WIN32 */

#include <tacopie/network/self_pipe.hpp>
#include <tacopie/network/tcp_socket.hpp>
#include <tacopie/utils/thread_pool.hpp>

#ifndef __TACOPIE_IO_SERVICE_NB_WORKERS
#define __TACOPIE_IO_SERVICE_NB_WORKERS 1
#endif /* __TACOPIE_IO_SERVICE_NB_WORKERS */

namespace tacopie {

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

  //! wait until the socket has been effectively removed
  //! basically wait until all pending callbacks are executed
  void wait_for_removal(const tcp_socket& socket);

private:
  //! struct tracked_socket
  //! contains information about what a current socket is tracking
  struct tracked_socket {
    //! ctor
    tracked_socket(void)
    : rd_callback(nullptr)
    , wr_callback(nullptr) {}

    //! rd event
    event_callback_t rd_callback;
    std::atomic<bool> is_executing_rd_callback = ATOMIC_VAR_INIT(false);

    //! wr event
    event_callback_t wr_callback;
    std::atomic<bool> is_executing_wr_callback = ATOMIC_VAR_INIT(false);

    //! marked for untrack
    std::atomic<bool> marked_for_untrack = ATOMIC_VAR_INIT(false);
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
  std::atomic<bool> m_should_stop;

  //! poll thread
  std::thread m_poll_worker;

  //! callback workers
  utils::thread_pool m_callback_workers;

  //! thread safety
  std::mutex m_tracked_sockets_mtx;

  //! data structure given to poll
  std::vector<struct pollfd> m_poll_fds_info;

  //! condition variable to wait on removal
  std::condition_variable m_wait_for_removal_condvar;

  //! fd associated to the pipe used to wake up the poll call
  tacopie::self_pipe m_notifier;
};

//! default io_service getter & setter
const std::shared_ptr<io_service>& get_default_io_service(void);
void set_default_io_service(const std::shared_ptr<io_service>&);

} //! tacopie
