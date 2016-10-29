#include <cpp_http_server/cpp_http_server>

#include <signal.h>

static std::atomic_bool should_stop(false);

void
signint_handler(int) {
  should_stop = true;
}

int
main(void) {
  //! enable logging
  cpp_http_server::active_logger = std::unique_ptr<cpp_http_server::logger>(new cpp_http_server::logger(cpp_http_server::logger::log_level::debug));

  cpp_http_server::network::tcp_server s;
  s.start("127.0.0.1", 3000);

  signal(SIGINT, &signint_handler);
  while (not should_stop);

  return 0;
}
