#include <tacopie/tacopie>

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <signal.h>

std::condition_variable cv;

void
signint_handler(int) {
  cv.notify_all();
}

void
on_new_message(tacopie::tcp_client& client, const tacopie::tcp_client::read_result& res) {
  if (res.success) {
    std::cout << "Client recv data" << std::endl;
    client.async_write({ res.buffer, nullptr });
    client.async_read({ 1024, &on_new_message });
  }
  else {
    std::cout << "Client disconnected" << std::endl;
    client.disconnect();
  }
}

int
main(void) {
  tacopie::tcp_client client;
  client.connect("127.0.0.1", 3001);
  client.async_read({ 1024, &on_new_message });

  signal(SIGINT, &signint_handler);

  std::mutex mtx;
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock);

  return 0;
}
