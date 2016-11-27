#include <tacopie/error.hpp>
#include <tacopie/network/tcp_server.hpp>

#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace tacopie {

namespace network {

//!
//! ctor & dtor
//!

tcp_socket::tcp_socket(void)
: m_fd(-1)
, m_host("")
, m_port(0)
, m_type(type::UNKNOWN)
{}

//!
//! custom ctor
//! build socket from existing file descriptor
//!

tcp_socket::tcp_socket(fd_t fd, const std::string& host, uint32_t port, type t)
: m_fd(fd)
, m_host(host)
, m_port(port)
, m_type(t)
{}

//!
//! client socket operations
//!

std::vector<char>
tcp_socket::recv(std::size_t size_to_read) {
  create_socket_if_necessary();
  check_or_set_type(type::CLIENT);

  std::vector<char> data(size_to_read, 0);

  ssize_t rd_size = ::recv(m_fd, const_cast<char*>(data.data()), size_to_read, 0);

  if (rd_size == -1)
    { __TACOPIE_THROW("tcp_socket::recv: recv() failure"); }

  return data;
}

std::size_t
tcp_socket::send(const std::vector<char>& data, std::size_t size_to_write) {
  create_socket_if_necessary();
  check_or_set_type(type::CLIENT);

  ssize_t wr_size = ::send(m_fd, data.data(), size_to_write, 0);

  if (wr_size == -1)
    { __TACOPIE_THROW("tcp_socket::send: send() failure"); }

  return wr_size;
}

void
tcp_socket::connect(const std::string& host, std::uint32_t port) {
  create_socket_if_necessary();
  check_or_set_type(type::CLIENT);

  struct hostent* addr = gethostbyname(host.c_str());

  if (not addr)
    { __TACOPIE_THROW("tcp_socket::bind: gethostbyname() failure"); }

  struct sockaddr_in server_addr;
  std::memset(&server_addr, 0, sizeof(server_addr));
  std::memcpy(&server_addr.sin_addr.s_addr, addr->h_addr, addr->h_length);
  server_addr.sin_port   = htons(port);
  server_addr.sin_family = AF_INET;

  if (::connect(m_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    { __TACOPIE_THROW("tcp_socket::bind: connect() failure"); }
}

//!
//! server socket operations
//!

void
tcp_socket::bind(const std::string& host, std::uint32_t port) {
  create_socket_if_necessary();
  check_or_set_type(type::SERVER);

  struct hostent* addr = gethostbyname(host.c_str());

  if (not addr)
    { __TACOPIE_THROW("tcp_socket::bind: gethostbyname() failure"); }

  struct sockaddr_in server_addr;
  std::memset(&server_addr, 0, sizeof(server_addr));
  std::memcpy(&server_addr.sin_addr.s_addr, addr->h_addr, addr->h_length);
  server_addr.sin_port   = htons(port);
  server_addr.sin_family = AF_INET;

  if (::bind(m_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    { __TACOPIE_THROW("tcp_socket::bind: bind() failure"); }
}

void
tcp_socket::listen(std::uint32_t max_connection_queue) {
  create_socket_if_necessary();
  check_or_set_type(type::SERVER);

  if (::listen(m_fd, max_connection_queue) == -1)
    { __TACOPIE_THROW("tcp_socket::listen: listen() failure"); }
}

tcp_socket
tcp_socket::accept(void) {
  create_socket_if_necessary();
  check_or_set_type(type::SERVER);

  struct sockaddr_in client_info;
  socklen_t client_info_struct_size = sizeof(client_info);

  fd_t client_fd = ::accept(m_fd, (struct sockaddr *)&client_info, &client_info_struct_size);

  if (client_fd == -1)
    { __TACOPIE_THROW("tcp_socket::accept: accept() failure"); }

  //! TODO: init with real client addr
  return { client_fd, "", client_info.sin_port, type::CLIENT };
}

//!
//! general socket operations
//!

void
tcp_socket::close(void) {
  if (m_fd != -1)
    { ::close(m_fd); }

  m_fd = -1;
  m_type = type::UNKNOWN;
}

//!
//! create a new socket if no socket has been initialized yet
//!

void
tcp_socket::create_socket_if_necessary(void) {
  if (m_fd != -1)
    { return ; }

  //! new TCP socket
  m_fd = socket(AF_INET, SOCK_STREAM, 0);
  m_type = type::UNKNOWN;

  if (m_fd == -1)
    { __TACOPIE_THROW("tcp_socket::create_socket_if_necessary: socket() failure"); }
}

//!
//! check whether the current socket has an approriate type for that kind of operation
//! if current type is UNKNOWN, update internal type with given type
//!

void
tcp_socket::check_or_set_type(type t) {
  if (m_type != type::UNKNOWN and m_type != t)
    { __TACOPIE_THROW("tcp_socket::check_or_set_type: trying to perform invalid operation on socket"); }

  m_type = t;
}

//!
//! get socket name information
//!

const std::string&
tcp_socket::get_host(void) const {
  return m_host;
}

std::uint32_t
tcp_socket::get_port(void) const {
  return m_port;
}

//!
//! get socket type
//!

tcp_socket::type
tcp_socket::get_type(void) const {
  return m_type;
}

//!
//! set type, should be used if some operations determining socket type
//! have been done on the behalf of the tcp_socket instance
//!

void
tcp_socket::set_type(type t) {
  m_type = t;
}

//!
//! direct access to the underlying fd
//!

fd_t
tcp_socket::get_fd(void) const{
  return m_fd;
}

} //! network

} //! tacopie
