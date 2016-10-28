#pragma once

#include <atomic>
#include <cstdint>
#include <string>

#include <cpp_http_server/network/io_service.hpp>
#include <cpp_http_server/network/tcp_socket.hpp>
#include <cpp_http_server/typedefs.hpp>

#define __CPP_HTTP_SERVER_CONNECTION_QUEUE_SIZE 1024

namespace cpp_http_server {

namespace network {

class tcp_server {
public:
  //! ctor & dtor
  tcp_server(void);
  ~tcp_server(void);

  //! copy ctor & assignment operator
  tcp_server(const tcp_server&) = delete;
  tcp_server& operator=(const tcp_server&) = delete;

public:
  //! start & stop the tcp server
  void start(const std::string& addr, std::uint32_t port);
  void stop(void);

  //! returns whether the server is currently running or not
  bool is_running(void) const;

private:
  //! io service read callback
  void on_read_available(fd_t fd);

private:
  //! store io_service
  //! prevent deletion of io_service before the tcp_server itself
  std::shared_ptr<io_service> m_io_service;

  //! server socket
  cpp_http_server::network::tcp_socket m_socket;

  //! whether the server is currently running or not
  std::atomic_bool m_is_running;
};

} //! network

} //! cpp_http_server
