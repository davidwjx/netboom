/*Taken from https://morestina.net/blog/1400/minimalistic-blocking-bounded-queue-for-c*/

#include <mutex>
#include <condition_variable>
#include  <deque>
	
namespace blocking_queue
{
template<typename T>
class queue {
  std::deque<T> content;
  size_t capacity;

  std::mutex mutex;
  std::condition_variable not_empty;
  std::condition_variable not_full;

  queue(const queue &) = delete;
  queue(queue &&) = delete;
  queue &operator = (const queue &) = delete;
  queue &operator = (queue &&) = delete;

 public:
  queue(size_t capacity): capacity(capacity) {}

  void push(T &&item) {
    {
      std::unique_lock<std::mutex> lk(mutex);
      not_full.wait(lk, [this]() { return content.size() < capacity; });
      content.push_back(std::move(item));
    }
    not_empty.notify_one();
  }

  bool try_push(T &&item) {
    {
      std::unique_lock<std::mutex> lk(mutex);
      if (content.size() == capacity)
        return false;
      content.push_back(std::move(item));
    }
    not_empty.notify_one();
    return true;
  }

  void pop(T &item) {
    {
      std::unique_lock<std::mutex> lk(mutex);
      not_empty.wait(lk, [this]() { return !content.empty(); });
      item = std::move(content.front());
      content.pop_front();
    }
    not_full.notify_one();
  }

  bool try_pop(T &item) {
    {
      std::unique_lock<std::mutex> lk(mutex);
      if (content.empty())
        return false;
      item = std::move(content.front());
      content.pop_front();
    }
    not_full.notify_one();
    return true;
  }
  bool pop_timed(T &item, uint32_t timeout_ms) {
    
      std::unique_lock<std::mutex> lk(mutex);
      auto timeout = std::chrono::milliseconds(1)*timeout_ms;
      if(not_empty.wait_for(lk,timeout, [this]() { return !content.empty(); }))
      {
        #ifdef DEBUG_IPC
        if(content.size() > 1)
          std::cout << "Queue size is " << content.size() << std::endl;
        #endif
        item = std::move(content.front());
        content.pop_front();
        not_full.notify_one();
        return true;
      }
      else
      {
        return false;
      }
  }

  int size()
  {
    mutex.lock();
    const int size = content.size();
    mutex.unlock();
    return size;
  }
};
};