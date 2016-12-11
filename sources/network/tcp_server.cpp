#include <tacopie/error.hpp>
#include <tacopie/network/tcp_server.hpp>
#include <tacopie/logger.hpp>

#include <algorithm>

namespace tacopie {

//!
//! ctor & dtor
//!

tcp_server::tcp_server(void)
: m_io_service(get_default_io_service())
, m_is_running(false)
, m_on_new_connection_callback(nullptr)
{ __TACOPIE_LOG(debug, "create tcp_server"); }

tcp_server::~tcp_server(void) {
  __TACOPIE_LOG(debug, "destroy tcp_server");
  stop();
}

//!
//! start & stop the tcp server
//!

void
tcp_server::start(const std::string& host, std::uint32_t port, const on_new_connection_callback_t& callback) {
  if (is_running()) {
    __TACOPIE_LOG(warn, "tcp_server is already running");
    __TACOPIE_THROW("tcp_server::start: tcp_server is already running");
  }

  m_socket.bind(host, port);
  m_socket.listen(__TACOPIE_CONNECTION_QUEUE_SIZE);

  m_io_service->track(m_socket);
  m_io_service->set_rd_callback(m_socket, std::bind(&tcp_server::on_read_available, this, std::placeholders::_1));
  m_on_new_connection_callback = callback;

  m_is_running = true;

  __TACOPIE_LOG(info, "tcp_server running");
}

void
tcp_server::stop(void) {
  if (not is_running())
    { return ; }

  m_is_running = false;

  m_io_service->untrack(m_socket);
  m_socket.close();

  std::lock_guard<std::mutex> lock(m_clients_mtx);
  for (auto& client : m_clients)
    { client.disconnect(); }
  m_clients.clear();

  __TACOPIE_LOG(info, "tcp_server stopped");
}

//!
//! io service read callback
//!

void
tcp_server::on_read_available(fd_t) {
  try {
    __TACOPIE_LOG(info, "tcp_server received new connection");

    std::lock_guard<std::mutex> lock(m_clients_mtx);
    m_clients.emplace_back(m_socket.accept());
    m_clients.back().set_on_disconnection_handler(std::bind(&tcp_server::on_client_disconnected, this, std::cref(m_clients.back())));

    if (m_on_new_connection_callback && !m_on_new_connection_callback(m_clients.back())) {
      __TACOPIE_LOG(info, "tcp_server dismissed new connection");
      m_clients.pop_back();
    }
  }
  catch (const tacopie::tacopie_error&) {
    __TACOPIE_LOG(warn, "accept operation failure");
    stop();
  }
}

//!
//! client disconnected
//!

void
tcp_server::on_client_disconnected(const tcp_client& client) {
  //! If we are not running the server
  //! Then it means that this function is called by tcp_client::disconnect() at the destruction of all clients
  if (not is_running())
    { return ; }

  __TACOPIE_LOG(debug, "handle server's client disconnection");

  std::lock_guard<std::mutex> lock(m_clients_mtx);
  auto it = std::find(m_clients.begin(), m_clients.end(), client);

  if (it != m_clients.end())
    { m_clients.erase(it); }
}

//!
//! returns whether the server is currently running or not
//!

bool
tcp_server::is_running(void) const {
  return m_is_running;
}

//!
//! comparison operator
//!
bool
tcp_server::operator==(const tcp_server& rhs) const {
  return m_socket == rhs.m_socket;
}

bool
tcp_server::operator!=(const tcp_server& rhs) const {
  return not operator==(rhs);
}

} //! tacopie
