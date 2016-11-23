#include <tacopie/error.hpp>
#include <tacopie/network/tcp_client.hpp>

namespace tacopie {

namespace network {

//!
//! ctor & dtor
//!

tcp_client::tcp_client(void)
: m_io_service(network::get_default_io_service())
, m_is_connected(false)
, m_disconnection_handler(nullptr)
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
, m_disconnection_handler(nullptr)
{}

//!
//! start & stop the tcp client
//!

void
tcp_client::connect(const std::string& host, std::uint32_t port) {
  if (is_connected())
    { __TACOPIE_THROW("tcp_client::connect: tcp_client is already connected"); }

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
  read_result result;

  auto callback = process_read(result);

  if (callback)
    { callback(*this, result); }
}

//!
//! io service write callback
//!

void
tcp_client::on_write_available(fd_t) {
  write_result result;

  auto callback = process_write(result);

  if (callback)
    { callback(*this, result); }
}

//!
//! process read & write operations when available
//!

tcp_client::async_read_callback_t
tcp_client::process_read(read_result& result) {
  std::lock_guard<std::mutex> lock(m_read_requests_mtx);

  if (m_read_requests.empty())
    { return nullptr; }

  const auto& request = m_read_requests.front();
  auto callback = request.async_read_callback;

  try {
    result.buffer = m_socket.recv(request.size);
    result.success = true;
  }
  catch (const tacopie::error&) {
    result.success = false;
  }

  m_read_requests.pop();

  return callback;
}

tcp_client::async_write_callback_t
tcp_client::process_write(write_result& result) {
  std::lock_guard<std::mutex> lock(m_write_requests_mtx);

  if (m_write_requests.empty())
    { return nullptr; }

  const auto& request = m_write_requests.front();
  auto callback = request.async_write_callback;

  try {
    result.size = m_socket.send(request.buffer, request.buffer.size());
    result.success = true;
  }
  catch (const tacopie::error&) {
    result.success = false;
  }

  m_write_requests.pop();

  return callback;
}

//!
//! async read & write operations
//!

void
tcp_client::async_read(const read_request& request) {
  std::lock_guard<std::mutex> lock(m_read_requests_mtx);

  m_read_requests.push(request);
}

void
tcp_client::async_write(const write_request& request) {
  std::lock_guard<std::mutex> lock(m_write_requests_mtx);

  m_write_requests.push(request);
}

//!
//! set on disconnection handler
//!

void
tcp_client::set_on_disconnection_handler(const disconnection_handler_t& disconnection_handler) {
  m_disconnection_handler = disconnection_handler;
}

//!
//! returns whether the client is currently running or not
//!

bool
tcp_client::is_connected(void) const {
  return m_is_connected;
}

} //! network

} //! tacopie
