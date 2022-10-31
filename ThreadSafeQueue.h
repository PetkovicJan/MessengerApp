#pragma once

#include <queue>
#include <mutex>

template<typename T>
class ThreadSafeQueue
{
public:
  size_t size() const;
  bool empty() const;

  void push(T const& val);
  T pop();

private:
  std::queue<T> queue_;
  mutable std::mutex mtx_;
};

template<typename T>
size_t ThreadSafeQueue<T>::size() const
{
  std::lock_guard<std::mutex> lg(mtx_);
  return queue_.size();
}

template<typename T>
bool ThreadSafeQueue<T>::empty() const
{
  std::lock_guard<std::mutex> lg(mtx_);
  return queue_.empty();
}

template<typename T>
void ThreadSafeQueue<T>::push(T const& val)
{
  std::lock_guard<std::mutex> lg(mtx_);
  queue_.push(val);
}

template<typename T>
T ThreadSafeQueue<T>::pop()
{
  std::lock_guard<std::mutex> lg(mtx_);
  auto front = queue_.front();
  queue_.pop();

  return front;
}

using MessageQueue = ThreadSafeQueue<std::string>;
