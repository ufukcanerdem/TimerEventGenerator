#include"Timer.h"

void Timer::timerThread() {
    while (true) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        cv.wait(lock, [&] { return !timer_queue.empty() || exit_flag; });
        
        if (exit_flag && timer_queue.empty()) return;

        auto next_event = timer_queue.top();
        if (CLOCK::now() < next_event.end_time) {
            cv.wait_until(lock, next_event.end_time);
        }

        if (CLOCK::now() >= next_event.end_time) {
            timer_queue.pop();
            next_event.callback();
            if (next_event.period.count() > 0 && (next_event.predicate == nullptr || !next_event.predicate())) {
                next_event.end_time = CLOCK::now() + next_event.period;
                timer_queue.push(next_event);
            }
        }
    }
}

void Timer::registerTimer(const TimerEvent& event) {
    std::lock_guard<std::mutex> lock(queue_mutex);
    timer_queue.push(event);
    cv.notify_all();
}

Timer::Timer() {
    timer_thread = std::thread(&Timer::timerThread, this);
}

Timer::~Timer() {
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        exit_flag = true;
    }
    cv.notify_all();
    timer_thread.join();
}

void Timer::registerTimer(const Timepoint& tp, const TTimerCallback& cb) {
    registerTimer({tp, Millisecs(0), cb, nullptr});
}

void Timer::registerTimer(const Millisecs& period, const TTimerCallback& cb) {
    registerTimer({CLOCK::now() + period, period, cb, nullptr});
}

void Timer::registerTimer(const Timepoint& tp, const Millisecs& period, const TTimerCallback& cb) {
    registerTimer({tp, period, cb, nullptr});
}

void Timer::registerTimer(const TPredicate& pred, const Millisecs& period, const TTimerCallback& cb) {
    registerTimer({CLOCK::now() + period, period, cb, pred});
}
