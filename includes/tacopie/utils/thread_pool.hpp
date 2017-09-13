// MIT License
//
// Copyright (c) 2016-2017 Simon Ninon <simon.ninon@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace tacopie {

namespace utils {

class thread_pool {
public:
  //!
  //! ctor
  //! created the worker thread that start working immediately
  //!
  //! \param nb_threads number of threads to start the thread pool
  //!
  explicit thread_pool(std::size_t nb_threads);

  //! dtor
  ~thread_pool(void);

  //! copy ctor
  thread_pool(const thread_pool&) = delete;
  //! assignment operator
  thread_pool& operator=(const thread_pool&) = delete;

public:
  //!
  //! task typedef
  ///! simply a callable taking no parameter
  //!
  typedef std::function<void()> task_t;

  //!
  //! add tasks to thread pool
  //! task is enqueued and will be executed whenever all previously executed tasked have been executed (or are currently being executed)
  //!
  //! \param task task to be executed by the threadpool
  //!
  void add_task(const task_t& task);

  //!
  //! same as add_task
  //!
  //! \param task task to be executed by the threadpool
  //! \return current instance
  //!
  thread_pool& operator<<(const task_t& task);

  //!
  //! stop the thread pool and wait for workers completion
  //! if some tasks are pending, they won't be executed
  //!
  void stop(void);

public:
  //!
  //! \return whether the thread_pool is running or not
  //!
  bool is_running(void) const;

private:
  //!
  //! worker main loop
  //!
  void run(void);

  //!
  //! retrieve a new task
  //! fetch the first element in the queue, or wait if no task are available
  //! may return null if the threadpool is stopped
  //!
  task_t fetch_task(void);

private:
  //!
  //! threads
  //!
  std::vector<std::thread> m_workers;

  //!
  //! whether the thread_pool should stop or not
  //!
  std::atomic<bool> m_should_stop = ATOMIC_VAR_INIT(false);

  //!
  //! tasks
  //!
  std::queue<task_t> m_tasks;

  //!
  //! tasks thread safety
  //!
  std::mutex m_tasks_mtx;

  //!
  //! task condvar to sync on tasks changes
  //!
  std::condition_variable m_tasks_condvar;
};

} // namespace utils

} // namespace tacopie
