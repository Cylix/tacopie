#include <tacopie/error.hpp>
#include <tacopie/logger.hpp>
#include <tacopie/network/tcp_client.hpp>

namespace tacopie {

//!
//! ctor & dtor
//!

tcp_client::tcp_client(void)
: m_io_service(get_default_io_service())
, m_is_connected(false)
, m_disconnection_handler(nullptr) { __TACOPIE_LOG(debug, "create tcp_client"); }

tcp_client::~tcp_client(void) {
  __TACOPIE_LOG(debug, "destroy tcp_client");
  disconnect();
}

//!
//! custom ctor
//! build socket from existing socket
//!

tcp_client::tcp_client(tcp_socket&& socket)
: m_io_service(get_default_io_service())
, m_socket(std::move(socket))
, m_is_connected(true)
, m_disconnection_handler(nullptr) {
  __TACOPIE_LOG(debug, "create tcp_client");
  m_io_service->track(m_socket);
}

//!
//! start & stop the tcp client
//!

void
tcp_client::connect(const std::string& host, std::uint32_t port) {
  if (is_connected()) { __TACOPIE_THROW(warn, "tcp_client is already connected"); }

  m_socket.connect(host, port);
  m_io_service->track(m_socket);

  m_is_connected = true;

  __TACOPIE_LOG(info, "tcp_client connected");
}

void
tcp_client::disconnect(void) {
  if (not is_connected()) { return; }

  m_is_connected = false;

  m_io_service->untrack(m_socket);
  m_socket.close();

  __TACOPIE_LOG(info, "tcp_client disconnected");
}

//!
//! Call disconnection handler
//!
void
tcp_client::call_disconnection_handler(void) {
  if (m_disconnection_handler) {
    __TACOPIE_LOG(debug, "call disconnection handler");
    m_disconnection_handler();
  }
}

//!
//! io service read callback
//!

void
tcp_client::on_read_available(fd_t) {
  __TACOPIE_LOG(info, "read available");

  read_result result;
  auto callback = process_read(result);

  if (not result.success) {
    __TACOPIE_LOG(warn, "read operation failure");
    disconnect();
  }

  if (callback) { callback(result); }

  if (not result.success) { call_disconnection_handler(); }
}

//!
//! io service write callback
//!

void
tcp_client::on_write_available(fd_t) {
  __TACOPIE_LOG(info, "write available");

  write_result result;
  auto callback = process_write(result);

  if (not result.success) {
    __TACOPIE_LOG(warn, "write operation failure");
    disconnect();
  }

  if (callback) { callback(result); }

  if (not result.success) { call_disconnection_handler(); }
}

//!
//! process read & write operations when available
//!

tcp_client::async_read_callback_t
tcp_client::process_read(read_result& result) {
  std::lock_guard<std::mutex> lock(m_read_requests_mtx);

  if (m_read_requests.empty()) { return nullptr; }

  const auto& request = m_read_requests.front();
  auto callback       = request.async_read_callback;

  try {
    result.buffer  = m_socket.recv(request.size);
    result.success = true;
  }
  catch (const tacopie::tacopie_error&) {
    result.success = false;
  }

  m_read_requests.pop();

  if (m_read_requests.empty()) { m_io_service->set_rd_callback(m_socket, nullptr); }

  return callback;
}

tcp_client::async_write_callback_t
tcp_client::process_write(write_result& result) {
  std::lock_guard<std::mutex> lock(m_write_requests_mtx);

  if (m_write_requests.empty()) { return nullptr; }

  const auto& request = m_write_requests.front();
  auto callback       = request.async_write_callback;

  try {
    result.size    = m_socket.send(request.buffer, request.buffer.size());
    result.success = true;
  }
  catch (const tacopie::tacopie_error&) {
    result.success = false;
  }

  m_write_requests.pop();

  if (m_write_requests.empty()) { m_io_service->set_wr_callback(m_socket, nullptr); }

  return callback;
}

//!
//! async read & write operations
//!

void
tcp_client::async_read(const read_request& request) {
  if (not is_connected()) { __TACOPIE_THROW(warn, "tcp_client is disconnected"); }

  std::lock_guard<std::mutex> lock(m_read_requests_mtx);

  __TACOPIE_LOG(info, "store async_read request");

  m_read_requests.push(request);
  m_io_service->set_rd_callback(m_socket, std::bind(&tcp_client::on_read_available, this, std::placeholders::_1));
}

void
tcp_client::async_write(const write_request& request) {
  if (not is_connected()) { __TACOPIE_THROW(warn, "tcp_client is disconnected"); }

  std::lock_guard<std::mutex> lock(m_write_requests_mtx);

  __TACOPIE_LOG(info, "store async_write request");

  m_write_requests.push(request);
  m_io_service->set_wr_callback(m_socket, std::bind(&tcp_client::on_write_available, this, std::placeholders::_1));
}

//!
//! socket getter
//!

tacopie::tcp_socket&
tcp_client::get_socket(void) {
  return m_socket;
}

const tacopie::tcp_socket&
tcp_client::get_socket(void) const {
  return m_socket;
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

//!
//! comparison operator
//!
bool
tcp_client::operator==(const tcp_client& rhs) const {
  return m_socket == rhs.m_socket;
}

bool
tcp_client::operator!=(const tcp_client& rhs) const {
  return not operator==(rhs);
}

} //! tacopie
