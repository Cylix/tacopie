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

#include <tacopie/network/tcp_server.hpp>
#include <tacopie/utils/error.hpp>
#include <tacopie/utils/logger.hpp>

#include <cstring>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

namespace tacopie {

void
tcp_socket::connect(const std::string& host, std::uint32_t port, std::uint32_t timeout_msecs) {
  //! Reset host and port
  m_host = host;
  m_port = port;

  create_socket_if_necessary();
  check_or_set_type(type::CLIENT);

  struct sockaddr_un server_addr_un;
  struct sockaddr_in server_addr_in;

  //! Handle case of unix sockets if port is 0
  bool is_unix_socket = m_port == 0;
  if (is_unix_socket) {
    std::memset(&server_addr_un, 0, sizeof(server_addr_un));
    server_addr_un.sun_family = AF_UNIX;
    strncpy(server_addr_un.sun_path, host.c_str(), sizeof(server_addr_un.sun_path) - 1);
  }
  else {
    struct addrinfo* result = nullptr;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family   = AF_INET;

    if (getaddrinfo(host.c_str(), nullptr, &hints, &result) != 0) { __TACOPIE_THROW(error, "getaddrinfo() failure"); }

    std::memset(&server_addr_in, 0, sizeof(server_addr_in));
    server_addr_in.sin_addr   = ((struct sockaddr_in*) (result->ai_addr))->sin_addr;
    server_addr_in.sin_port   = htons(port);
    server_addr_in.sin_family = AF_INET;

    freeaddrinfo(result);
  }

  socklen_t addr_len                 = is_unix_socket ? sizeof(server_addr_un) : sizeof(server_addr_in);
  const struct sockaddr* server_addr = is_unix_socket ? (const struct sockaddr*) &server_addr_un : (const struct sockaddr*) &server_addr_in;

  if (timeout_msecs > 0) {
    //! for timeout connection handling:
    //!  1. set socket to non blocking
    //!  2. connect
    //!  3. poll select
    //!  4. check connection status
    if (fcntl(m_fd, F_SETFL, fcntl(m_fd, F_GETFL, 0) | O_NONBLOCK) == -1) {
      close();
      __TACOPIE_THROW(error, "connect() set non-blocking failure");
    }
  }

  int ret = ::connect(m_fd, server_addr, addr_len);
  if (ret < 0 && errno != EINPROGRESS) {
    close();
    __TACOPIE_THROW(error, "connect() failure");
  }

  if (timeout_msecs > 0) {
    timeval tv;
    tv.tv_sec  = (timeout_msecs / 1000);
    tv.tv_usec = ((timeout_msecs - (tv.tv_sec * 1000)) * 1000);

    fd_set set;
    FD_ZERO(&set);
    FD_SET(m_fd, &set);

    //! 1 means we are connected.
    //! 0/-1 means a timeout.
    if (select(0, NULL, &set, NULL, &tv) == 1) {
      //! Set back to blocking mode as the user of this class is expecting
      if (fcntl(m_fd, F_SETFL, fcntl(m_fd, F_GETFL, 0) & (~O_NONBLOCK)) == -1) {
        close();
        __TACOPIE_THROW(error, "connect() set blocking failure");
      }
    }
    else {
      close();
      __TACOPIE_THROW(error, "connect() timed out");
    }
  }
}

//!
//! server socket operations
//!

void
tcp_socket::bind(const std::string& host, std::uint32_t port) {
  //! Reset host and port
  m_host = host;
  m_port = port;

  create_socket_if_necessary();
  check_or_set_type(type::SERVER);

  struct sockaddr_un server_addr_un;
  struct sockaddr_in server_addr_in;

  //! Handle case of unix sockets if port is 0
  bool is_unix_socket = m_port == 0;
  if (is_unix_socket) {
    std::memset(&server_addr_un, 0, sizeof(server_addr_un));
    server_addr_un.sun_family = AF_UNIX;
    strncpy(server_addr_un.sun_path, host.c_str(), sizeof(server_addr_un.sun_path) - 1);
  }
  else {
    struct addrinfo* result = nullptr;

    if (getaddrinfo(host.c_str(), nullptr, nullptr, &result) != 0) {
      __TACOPIE_THROW(error, "getaddrinfo() failure");
    }

    std::memset(&server_addr_in, 0, sizeof(server_addr_in));
    server_addr_in.sin_addr   = ((struct sockaddr_in*) (result->ai_addr))->sin_addr;
    server_addr_in.sin_port   = htons(port);
    server_addr_in.sin_family = AF_INET;

    freeaddrinfo(result);
  }

  socklen_t addr_len                 = is_unix_socket ? sizeof(server_addr_un) : sizeof(server_addr_in);
  const struct sockaddr* server_addr = is_unix_socket ? (const struct sockaddr*) &server_addr_un : (const struct sockaddr*) &server_addr_in;

  if (::bind(m_fd, server_addr, addr_len) == -1) { __TACOPIE_THROW(error, "bind() failure"); }
}

//!
//! general socket operations
//!

void
tcp_socket::close(void) {
  if (m_fd != __TACOPIE_INVALID_FD) {
    __TACOPIE_LOG(debug, "close socket");
    ::close(m_fd);
  }

  m_fd   = __TACOPIE_INVALID_FD;
  m_type = type::UNKNOWN;
}
//!
//! create a new socket if no socket has been initialized yet
//!

void
tcp_socket::create_socket_if_necessary(void) {
  if (m_fd != __TACOPIE_INVALID_FD) { return; }

  //! new TCP socket
  //! handle case of unix sockets by checking whether the port is 0 or not
  m_fd   = socket(m_port == 0 ? AF_UNIX : AF_INET, SOCK_STREAM, 0);
  m_type = type::UNKNOWN;

  if (m_fd == __TACOPIE_INVALID_FD) { __TACOPIE_THROW(error, "tcp_socket::create_socket_if_necessary: socket() failure"); }
}

} // namespace tacopie
