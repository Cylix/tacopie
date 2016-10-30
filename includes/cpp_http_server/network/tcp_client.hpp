#pragma once

#include <atomic>
#include <cstdint>
#include <string>

#include <cpp_http_server/network/io_service.hpp>
#include <cpp_http_server/network/tcp_socket.hpp>
#include <cpp_http_server/typedefs.hpp>

namespace cpp_http_server {

namespace network {

class tcp_client {
public:
  //! ctor & dtor
  tcp_client(void);
  ~tcp_client(void);

  //! custom ctor
  //! build socket from existing socket
  explicit tcp_client(const tcp_socket& socket);

  //! copy ctor & assignment operator
  tcp_client(const tcp_client&) = delete;
  tcp_client& operator=(const tcp_client&) = delete;

public:
  //! start & stop the tcp client
  void connect(const std::string& addr, std::uint32_t port);
  void disconnect(void);

  //! returns whether the client is currently connected or not
  bool is_connected(void) const;

private:
  //! io service read callback
  void on_read_available(fd_t fd);

  //! io service write callback
  void on_write_available(fd_t fd);

private:
  //! store io_service
  //! prevent deletion of io_service before the tcp_client itself
  std::shared_ptr<io_service> m_io_service;

  //! client socket
  cpp_http_server::network::tcp_socket m_socket;

  //! whether the client is currently connected or not
  std::atomic_bool m_is_connected;
};

} //! network

} //! cpp_http_server
