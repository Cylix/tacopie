#include <cpp_http_server/error.hpp>
#include <cpp_http_server/network/tcp_server.hpp>

namespace cpp_http_server {

namespace network {

//!
//! ctor & dtor
//!

tcp_server::tcp_server(void)
: m_io_service(network::get_default_io_service())
{}

tcp_server::~tcp_server(void)
{ stop(); }

//!
//! start & stop the tcp server
//!

void
tcp_server::start(const std::string& host, std::uint32_t port) {
  if (is_running())
    { __CPP_HTTP_SERVER_THROW("tcp_server::start: tcp_server is already running"); }

  m_socket.bind(host, port);
  m_socket.listen(__CPP_HTTP_SERVER_CONNECTION_QUEUE_SIZE);

  m_io_service->track(m_socket);
  m_io_service->set_rd_callback(m_socket, std::bind(&tcp_server::on_read_available, this, std::placeholders::_1));

  m_is_running = true;
}

void
tcp_server::stop(void) {
  if (not is_running())
    { return ; }

  m_io_service->untrack(m_socket);
  m_socket.close();

  m_is_running = false;
}

//!
//! io service read callback
//!

void
tcp_server::on_read_available(fd_t) {
  try {
    m_socket.accept();
  }
  catch (const cpp_http_server::error&) {
    stop();
  }
}

//!
//! returns whether the server is currently running or not
//!

bool
tcp_server::is_running(void) const {
  return m_is_running;
}

} //! network

} //! cpp_http_server
