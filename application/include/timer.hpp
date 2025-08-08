#ifndef TIMER_HPP
#define TIMER_HPP

#include <functional>

class Timer {
public:
    Timer(std::function<void(void)> callback);

    Timer(const Timer&) = delete;

    Timer(Timer&&) = delete;

    Timer& operator=(const Timer&) = delete;

    Timer& operator=(Timer&&) = delete;

    ~Timer();

private:
    std::function<void(void)> m_callback;

#if defined(_WIN32)
    void* m_timer_queue;
#endif
};

#endif // TIMER_HPP
