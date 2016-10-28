#include <cpp_http_server/cpp_http_server>

int
main(void) {
  //! enable logging
  cpp_http_server::active_logger = std::unique_ptr<cpp_http_server::logger>(new cpp_http_server::logger(cpp_http_server::logger::log_level::debug));

  cpp_http_server::network::tcp_server s;
  s.start("127.0.0.1", 3000);

  while (1);

  return 0;
}
