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
#include <cstdint>
#include <list>
#include <memory>
#include <mutex>
#include <string>

#include <tacopie/network/io_service.hpp>
#include <tacopie/network/tcp_client.hpp>
#include <tacopie/network/tcp_socket.hpp>
#include <tacopie/typedefs.hpp>

#define __TACOPIE_CONNECTION_QUEUE_SIZE 1024

namespace tacopie {

class tcp_server {
public:
  //! ctor & dtor
  tcp_server(void);
  ~tcp_server(void);

  //! copy ctor & assignment operator
  tcp_server(const tcp_server&) = delete;
  tcp_server& operator=(const tcp_server&) = delete;

public:
  //! comparison operator
  bool operator==(const tcp_server& rhs) const;
  bool operator!=(const tcp_server& rhs) const;

public:
  //! convenience typedef
  typedef std::function<bool(const std::shared_ptr<tcp_client>&)> on_new_connection_callback_t;

  //! start & stop the tcp server
  void start(const std::string& addr, std::uint32_t port, const on_new_connection_callback_t& callback = nullptr);
  void stop(void);

  //! returns whether the server is currently running or not
  bool is_running(void) const;

public:
  //! get socket
  tcp_socket& get_socket(void);
  const tcp_socket& get_socket(void) const;

public:
  //! get client sockets
  const std::list<std::shared_ptr<tacopie::tcp_client>>& get_clients(void) const;

private:
  //! io service read callback
  void on_read_available(fd_t fd);

  //! client disconnected
  void on_client_disconnected(const std::shared_ptr<tcp_client>& client);

private:
  //! store io_service
  //! prevent deletion of io_service before the tcp_server itself
  std::shared_ptr<io_service> m_io_service;

  //! server socket
  tacopie::tcp_socket m_socket;

  //! whether the server is currently running or not
  std::atomic<bool> m_is_running = ATOMIC_VAR_INIT(false);

  //! clients
  std::list<std::shared_ptr<tacopie::tcp_client>> m_clients;

  //! clients thread safety
  std::mutex m_clients_mtx;

  //! on new connection callback
  on_new_connection_callback_t m_on_new_connection_callback;
};

} //! tacopie
