#include <cpp_http_server/error.hpp>
#include <cpp_http_server/network/tcp_client.hpp>

namespace cpp_http_server {

namespace network {

//!
//! ctor & dtor
//!

tcp_client::tcp_client(void)
: m_io_service(network::get_default_io_service())
, m_is_connected(false)
{}

tcp_client::~tcp_client(void)
{ disconnect(); }

//!
//! custom ctor
//! build socket from existing socket
//!

tcp_client::tcp_client(const tcp_socket& socket)
: m_io_service(network::get_default_io_service())
, m_socket(socket)
, m_is_connected(true)
{}

//!
//! start & stop the tcp client
//!

void
tcp_client::connect(const std::string& host, std::uint32_t port) {
  if (is_connected())
    { __CPP_HTTP_SERVER_THROW("tcp_client::connect: tcp_client is already connected"); }

  m_socket.connect(host, port);
  m_io_service->track(m_socket);

  m_is_connected = true;
}

void
tcp_client::disconnect(void) {
  if (not is_connected())
    { return ; }

  m_io_service->untrack(m_socket);
  m_socket.close();

  m_is_connected = false;
}

//!
//! io service read callback
//!

void
tcp_client::on_read_available(fd_t) {
}

//!
//! io service write callback
//!

void
tcp_client::on_write_available(fd_t) {
}

//!
//! returns whether the client is currently running or not
//!

bool
tcp_client::is_connected(void) const {
  return m_is_connected;
}

} //! network

} //! cpp_http_server
