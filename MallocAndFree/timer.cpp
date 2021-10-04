#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <ctime>
#ifndef _WIN32
#include<sys/time.h>
#else 
#ifdef _WIN32
int gettimeofday(struct timeval* tp, void* tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;

    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;
    clock = mktime(&tm);
    tp->tv_sec = clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;

    return (0);
}
#endif
#endif

#include "timer.h"

// global declaration
class Timer;
class TimerManager;



// implemetation of Timer

Timer::Timer(TimerManager& manager) : manager_(manager)
{
    // to-do
    heapIndex_ = (-1);
}

Timer::~Timer()
{
    Stop();
}

void Timer::Start(Fun fun, unsigned interval, TimerType timeType)
{
    Stop();
    interval_ = interval;
    timerFun_ = fun;
    timerType_ = timeType;
    this->expires_ = this->interval_ + TimerManager::GetCurrentMillisecs();
    manager_.AddTimer(this);
}

void Timer::Stop()
{
    if (heapIndex_ != -1)
    {
        manager_.RemoveTimer(this);
        heapIndex_ = -1;
    }
}

void Timer::OnTimer(unsigned long long now)
{
    if (timerType_ == Timer::CIRCLE)
    {
        expires_ = interval_ + now;
        manager_.AddTimer(this);
    }
    else
    {
        heapIndex_ = -1;
    }
    timerFun_();
}

// implemetation of TimerManager

void TimerManager::AddTimer(Timer* timer)
{
    timer->heapIndex_ = heap_.size();
    HeapEntry entry = { timer->expires_, timer };
    heap_.push_back(entry);
    UpHeap(heap_.size() - 1);
}

void TimerManager::RemoveTimer(Timer* timer)
{
    int index = timer->heapIndex_;
    if (!heap_.empty() && index < heap_.size())
    {
        if (index == heap_.size() - 1)
        {
            heap_.pop_back();
        }
        else
        {
            SwapHeap(index, heap_.size() - 1);
            heap_.pop_back();
            int parent = (index - 1) / 2;
            if (index > 0 && heap_[index].time < heap_[parent].time)
            {
                UpHeap(index);
            }
            else
            {
                DownHeap(index);
            }
        }
    }
}

void TimerManager::DetectTimers()
{
    unsigned long long now = GetCurrentMillisecs();

    while (!heap_.empty() && heap_[0].time <= now)
    {
        Timer* timer = heap_[0].timer;
        RemoveTimer(timer);
        timer->OnTimer(now);
    }
}

void TimerManager::UpHeap(int index)
{
    int parent = index >> 1;
    while (index > 0 && heap_[index].time < heap_[parent].time)
    {
        SwapHeap(index, parent);
        index = parent;
        parent = index >> 1;
    }
}

void TimerManager::DownHeap(int index)
{
    int child = (index << 1) + 1;
    while (child < heap_.size())
    {
        int minChild = (child + 1 == heap_.size() || heap_[child].time < heap_[child + 1].time) ? child : child + 1;
        if (heap_[index].time < heap_[minChild].time)
            break;
        SwapHeap(index, minChild);
        index = minChild;
        child = (index << 1) + 1;
    }
}

void TimerManager::SwapHeap(int index1, int index2)
{
    HeapEntry tmp = heap_[index1];
    heap_[index1] = heap_[index2];
    heap_[index2] = tmp;
    heap_[index1].timer->heapIndex_ = index1;
    heap_[index2].timer->heapIndex_ = index2;
}

unsigned long long TimerManager::GetCurrentMillisecs()
{
    timeval tv;
    ::gettimeofday(&tv, 0);
    unsigned long long ret = tv.tv_sec;
    return ret * 1000 + tv.tv_usec / 1000;
}