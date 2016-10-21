#include <cpp_http_server/cpp_http_server>

int
main(void) {
  //! enable logging
  cpp_http_server::active_logger = std::unique_ptr<cpp_http_server::logger>(new cpp_http_server::logger(cpp_http_server::logger::log_level::debug));

  __CPP_HTTP_SERVER_LOG(info, "Hello World!");

  return 0;
}
