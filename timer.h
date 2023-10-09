#include<queue>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<chrono>

using CLOCK = std::chrono::high_resolution_clock;
using TTimerCallback = std::function<void()>;
using Millisecs = std::chrono::milliseconds;
using Timepoint = CLOCK::time_point;
using TPredicate = std::function<bool()>;

class ITimer {
public:
    virtual void registerTimer(const Timepoint& tp, const TTimerCallback& cb) = 0;
    virtual void registerTimer(const Millisecs& period, const TTimerCallback& cb) = 0;
    virtual void registerTimer(const Timepoint& tp, const Millisecs& period, const TTimerCallback& cb) = 0;
    virtual void registerTimer(const TPredicate& pred, const Millisecs& period, const TTimerCallback& cb) = 0;
};

struct TimerEvent {
    Timepoint end_time;
    Millisecs period;
    TTimerCallback callback;
    TPredicate predicate;

    bool operator>(const TimerEvent& e) const {
        return end_time > e.end_time;
    }
};

class Timer : public ITimer {
private:
    std::priority_queue<TimerEvent, std::vector<TimerEvent>, std::greater<>> timer_queue;   //Queue that keeps tasks
    std::thread timer_thread;   //Single thread
    std::mutex queue_mutex;     //Mutex for synchronization
    std::condition_variable cv; //condition variable for synchronization
    bool exit_flag = false;

    void timerThread();

    void registerTimer(const TimerEvent& event);

public:
    //Constructor
    Timer();

    //Destructor
    ~Timer();

    // run the callback once at time point tp.
    void registerTimer(const Timepoint& tp, const TTimerCallback& cb) override;
    
    // run the callback periodically forever. The first call will be executed after the first period. 
    void registerTimer(const Millisecs& period, const TTimerCallback& cb) override;
    
    // Run the callback periodically until time point tp. The first call will be executed after the first period.
    void registerTimer(const Timepoint& tp, const Millisecs& period, const TTimerCallback& cb) override;
    
    //termination criterion is satisfied. If the predicate returns false, stop calling the callback
    void registerTimer(const TPredicate& pred, const Millisecs& period, const TTimerCallback& cb) override;
};
