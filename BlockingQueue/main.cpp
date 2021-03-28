#include <iostream>
#include <thread>

#include "blockingqueue.h"

BlockingQueue<int> q;


void custom() {
  while(1)
    std::cout << q.take() << std::endl;
}

void p1() {
  while(1)
    q.put(1);
}

void p2() {
  while(1)
    q.put(2);
}

int main(int argc, char *argv[])
{

  std::thread t1(custom);
  std::thread t2(p2);
  std::thread t3(p1);


  t1.join();
  t2.join();
  t3.join();


  return 0;
}
