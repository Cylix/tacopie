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

#include <cstdint>
#include <string>
#include <vector>

#include <tacopie/typedefs.hpp>

namespace tacopie {

class tcp_socket {
public:
  //! possible types of a TCP socket, either a client or a server
  //! type is used to prevent the used of client specific operations on a server socket (and vice-versa)
  //!
  //! UNKNOWN is used when socket type could not be determined for now
  enum class type {
    CLIENT,
    SERVER,
    UNKNOWN
  };

public:
  //! ctor & dtor
  tcp_socket(void);
  ~tcp_socket(void) = default;

  //! custom ctor
  //! build socket from existing file descriptor
  tcp_socket(fd_t fd, const std::string& host, std::uint32_t port, type t);

  //! move ctor
  tcp_socket(tcp_socket&&);

  //! copy ctor & assignment operator
  tcp_socket(const tcp_socket&) = delete;
  tcp_socket& operator=(const tcp_socket&) = delete;

public:
  //! comparison operator
  bool operator==(const tcp_socket& rhs) const;
  bool operator!=(const tcp_socket& rhs) const;

public:
  //! client socket operations
  std::vector<char> recv(std::size_t size_to_read);
  std::size_t send(const std::vector<char>& data, std::size_t size_to_write);
  void connect(const std::string& host, std::uint32_t port);

  //! server socket operations
  void bind(const std::string& host, std::uint32_t port);
  void listen(std::size_t max_connection_queue);
  tcp_socket accept(void);

  //! general socket operations
  void close(void);

public:
  //! get socket name information
  const std::string& get_host(void) const;
  std::uint32_t get_port(void) const;

  //! get socket type
  type get_type(void) const;
  //! set type, should be used if some operations determining socket type
  //! have been done on the behalf of the tcp_socket instance
  void set_type(type t);

  //! direct access to the underlying fd
  fd_t get_fd(void) const;

private:
  //! create a new socket if no socket has been initialized yet
  void create_socket_if_necessary(void);

  //! check whether the current socket has an approriate type for that kind of operation
  //! if current type is UNKNOWN, update internal type with given type
  void check_or_set_type(type t);

private:
  //! fd associated to the socket
  fd_t m_fd;

  //! socket name information
  std::string m_host;
  std::uint32_t m_port;

  //! type of the socket
  type m_type;
};

} //! tacopie
