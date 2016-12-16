#include <tacopie/logger.hpp>
#include <tacopie/utils/thread_pool.hpp>

namespace tacopie {

namespace utils {

//!
//! ctor & dtor
//!

thread_pool::thread_pool(std::size_t nb_threads)
: m_should_stop(false) {
  __TACOPIE_LOG(debug, "create thread_pool");

  for (std::size_t i = 0; i < nb_threads; ++i) { m_workers.push_back(std::thread(std::bind(&thread_pool::run, this))); }
}

thread_pool::~thread_pool(void) {
  __TACOPIE_LOG(debug, "destroy thread_pool");
  stop();
}

//!
//! worker main loop
//!

void
thread_pool::run(void) {
  __TACOPIE_LOG(debug, "start run() worker");

  while (not m_should_stop) {
    task_t task = fetch_task();

    if (task) {
      __TACOPIE_LOG(debug, "execute task");
      task();
    }
  }

  __TACOPIE_LOG(debug, "stop run() worker");
}

//!
//! stop the thread pool and wait for workers completion
//!

void
thread_pool::stop(void) {
  if (not is_running()) { return; }

  m_should_stop = true;
  m_tasks_condvar.notify_all();

  for (auto& worker : m_workers) { worker.join(); }

  m_workers.clear();

  __TACOPIE_LOG(debug, "thread_pool stopped");
}

//!
//! whether the thread_pool is running or not
//!
bool
thread_pool::is_running(void) const {
  return not m_should_stop;
}

//!
//! retrieve a new task
//!

thread_pool::task_t
thread_pool::fetch_task(void) {
  std::unique_lock<std::mutex> lock(m_tasks_mtx);

  __TACOPIE_LOG(debug, "waiting to fetch task");

  m_tasks_condvar.wait(lock, [&] { return m_should_stop or not m_tasks.empty(); });

  if (m_tasks.empty()) { return nullptr; }

  task_t task = std::move(m_tasks.front());
  m_tasks.pop();
  return task;
}

//!
//! add tasks to thread pool
//!

void
thread_pool::add_task(const task_t& task) {
  std::lock_guard<std::mutex> lock(m_tasks_mtx);

  __TACOPIE_LOG(debug, "add task to thread_pool");

  m_tasks.push(task);
  m_tasks_condvar.notify_all();
}

thread_pool&
thread_pool::operator<<(const task_t& task) {
  add_task(task);

  return *this;
}

} //! utils

} //! tacopie
