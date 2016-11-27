#include <tacopie/tacopie>

#include <iostream>
#include <signal.h>

static std::atomic_bool should_stop(false);

void
signint_handler(int) {
  should_stop = true;
}

void
on_new_client(tacopie::network::tcp_client& client, const tacopie::network::tcp_client::read_result& res) {
  if (res.success) {
    std::cout << "Client recv data" << std::endl;
    client.async_write({ res.buffer, nullptr });
    client.async_read({ 1024, &on_new_client });
  }
  else {
    std::cout << "Client disconnected" << std::endl;
    client.disconnect();
  }
}

int
main(void) {
  //! enable logging
  tacopie::active_logger = std::unique_ptr<tacopie::logger>(new tacopie::logger(tacopie::logger::log_level::debug));

  tacopie::network::tcp_server s;
  s.start("127.0.0.1", 3000, [] (tacopie::network::tcp_client& client) -> bool {
    std::cout << "New client" << std::endl;
    client.async_read({ 1024, &on_new_client });
    return true;
  });

  signal(SIGINT, &signint_handler);
  while (not should_stop);

  return 0;
}
