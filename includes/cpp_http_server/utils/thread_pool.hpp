#pragma once

#include <atomic>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <thread>
#include <vector>

namespace cpp_http_server {

namespace utils {

class thread_pool {
public:
  //! ctor & dtor
  thread_pool(std::size_t nb_threads);
  ~thread_pool(void);

  //! copy ctor & assignment operator
  thread_pool(const thread_pool&) = delete;
  thread_pool& operator=(const thread_pool&) = delete;

public:
  //! task typedef
  typedef std::function<void()> task_t;

  //! add tasks to thread pool
  void add_task(const task_t& task);
  thread_pool& operator<<(const task_t& task);

private:
  //! worker main loop
  void run(void);

  //! retrieve a new task
  task_t fetch_task(void);

private:
  //! threads
  std::vector<std::thread> m_workers;

  //! whether the thread_pool should stop or not
  std::atomic_bool m_should_stop;

  //! tasks
  std::queue<task_t> m_tasks;

  //! thread safety
  std::mutex m_tasks_mtx;
  std::condition_variable m_tasks_condvar;
};

} //! utils

} //! cpp_http_server
