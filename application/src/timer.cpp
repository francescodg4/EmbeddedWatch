#if defined(_WIN32)
#include <Windows.h>
#endif

#include <stdexcept>
#include "timer.hpp"

static VOID CALLBACK CallbackRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
    if ((lpParam != NULL) && TimerOrWaitFired) {
        std::function<void(void)>& callback = *static_cast<std::function<void(void)>*>(lpParam);
        callback();
    }
}

Timer::Timer(std::function<void(void)> callback)
    : m_callback(callback)
{
    m_timer_queue = CreateTimerQueue();

    if (m_timer_queue == NULL) {
        throw std::runtime_error("CreateTimerQueue failed"); // (%d)\n", GetLastError());
    }

    HANDLE hTimer = NULL;
    if (!CreateTimerQueueTimer(&hTimer, m_timer_queue, (WAITORTIMERCALLBACK)CallbackRoutine, &m_callback, 0, 100, 0)) {
        // printf("CreateTimerQueueTimer failed (%d)\n", GetLastError());
        throw std::runtime_error("CreateTimerQueueTimer failed");
    }
}

Timer::~Timer()
{
    DeleteTimerQueue((HANDLE)m_timer_queue);
}
