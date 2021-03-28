#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include <list>
#include <condition_variable>
#include <iostream>
template <typename Type>
class BlockingQueue
{
public:
  BlockingQueue(int size = INT_MAX) : _maxSize(size) {}

  void setMaxSize(int size) {_maxSize = size;}
  int maxSize() {return _maxSize;}

  /**
   * @brief take
   * @return take a value, if is empty wait until new value in
   */
  Type take() noexcept(noexcept(_q.pop_front())) {
    std::unique_lock<std::mutex> locker(_m);

    while(_q.empty()) {
      waitFor(_condiNotEnmpty, locker);
    }

    auto tmp = _q.front();
    _q.pop_front();

    _condiNotFull.notify_all();
    return tmp;
  }

  void put(const Type& el) {
    std::unique_lock<std::mutex> locker(_m);
    while(_q.size() == _maxSize)
    {
      waitFor(_condiNotFull, locker);
    }
    _q.push_back(el);
    _condiNotEnmpty.notify_all();
  }

  bool offer(const Type& el) {
    std::unique_lock<std::mutex> locker(_m);
    if (_q.size() == _maxSize)
      return false;
    _q.push_back(el);
    return true;
  }

  bool offer(const Type& el, int msDelay = -1) {
    std::unique_lock<std::mutex> locker(_m);
    while(_q.size() == _maxSize)
    {
      auto res = waitFor(_condiNotFull, locker, msDelay);
      //if timeout return false
      if(res == std::cv_status::timeout)
        return false;
    }
    _q.push_back(el);
    return true;
  }

private:
  std::cv_status waitFor(std::condition_variable& condition,
                         std::unique_lock<std::mutex> &locker,
                         int delay = -1) {
    auto res = std::cv_status::no_timeout;
    if(delay < 0)
      condition.wait(locker);
    else
      res = condition.wait_for(locker, std::chrono::duration<int, std::milli>(delay));

    return res;
  }


private:
  std::list<Type> _q;
  std::condition_variable _condiNotEnmpty, _condiNotFull;
  std::mutex _m;
  int _maxSize;
};

#endif // BLOCKINGQUEUE_H
