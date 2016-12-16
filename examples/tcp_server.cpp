#include <tacopie/tacopie>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <signal.h>

std::condition_variable cv;

void
signint_handler(int) {
  cv.notify_all();
}

void
on_new_message(const std::shared_ptr<tacopie::tcp_client>& client, const tacopie::tcp_client::read_result& res) {
  if (res.success) {
    std::cout << "Client recv data" << std::endl;
    client->async_write({res.buffer, nullptr});
    client->async_read({1024, std::bind(&on_new_message, client, std::placeholders::_1)});
  }
  else {
    std::cout << "Client disconnected" << std::endl;
    client->disconnect();
  }
}

int
main(void) {
  tacopie::tcp_server s;
  s.start("127.0.0.1", 3001, [](const std::shared_ptr<tacopie::tcp_client>& client) -> bool {
    std::cout << "New client" << std::endl;
    client->async_read({1024, std::bind(&on_new_message, client, std::placeholders::_1)});
    return true;
  });

  signal(SIGINT, &signint_handler);

  std::mutex mtx;
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock);

  return 0;
}
