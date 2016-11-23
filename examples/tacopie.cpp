#include <tacopie/tacopie>

#include <signal.h>

static std::atomic_bool should_stop(false);

void
signint_handler(int) {
  should_stop = true;
}

int
main(void) {
  //! enable logging
  tacopie::active_logger = std::unique_ptr<tacopie::logger>(new tacopie::logger(tacopie::logger::log_level::debug));

  tacopie::network::tcp_server s;
  s.start("127.0.0.1", 3000);

  signal(SIGINT, &signint_handler);
  while (not should_stop);

  return 0;
}
