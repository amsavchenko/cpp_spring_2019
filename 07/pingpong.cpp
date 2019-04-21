// ping-pong multithreading cout

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

const size_t N = 5e5;
std::mutex m;
std::atomic_int cur_steps {0};
std::condition_variable condition;

void pingpong(const size_t flag)
{
    while (cur_steps < N)
    {
        std::unique_lock<std::mutex> lock(m);
        while((cur_steps % 2) != flag)
        {
            condition.wait(lock);
        }
        if (flag == 0)
            std::cout << "ping" << std::endl;
        else
            std::cout << "pong" << std::endl;
        cur_steps += 1;
        condition.notify_one();
    }
}

int main()
{
    std::thread t1(pingpong, 0);
    std::thread t2 (pingpong, 1);
    t1.join();
    t2.join();
}
