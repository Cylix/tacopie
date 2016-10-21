#include <cpp_http_server/cpp_http_server>

int
main(void) {
  //! enable logging
  cpp_http_server::active_logger = std::unique_ptr<cpp_http_server::logger>(new cpp_http_server::logger(cpp_http_server::logger::log_level::debug));

  return 0;
}
