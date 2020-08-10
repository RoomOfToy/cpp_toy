//
// Created by Harold on 2020/8/7.
//

#ifndef CPP_TOY_EVLOOP_H
#define CPP_TOY_EVLOOP_H

#include <memory>
#include <thread>
#include <map>
#include <functional>
#include <chrono>
#include <vector>

//######################### helper ###########################
template<typename T>
struct is_chrono_duration {
    static constexpr bool value = false;
};

template<typename Rep, typename Period>
struct is_chrono_duration<std::chrono::duration<Rep, Period>> {
    static constexpr bool value = true;
};
//###################### end of helper ########################

template<typename EventType>
class evloop {
public:
    using callback_t = std::function<void(const EventType &)>;
    using defer_t = std::function<void(std::unique_lock<std::mutex> &)>;
private:
    std::atomic<bool> running_;
    std::thread thread_;
    std::mutex events_lock_;
    std::condition_variable events_condition_;
    std::multimap<int, callback_t> callbacks_;
    std::multimap<std::chrono::time_point<std::chrono::high_resolution_clock>, defer_t> events_;
public:
    evloop() : running_(false) {};

    // non-copyable
    evloop(const evloop &) = delete;
    evloop &operator=(const evloop &) = delete;

    // movable
    evloop(evloop &&) noexcept = default;

    void start() {
        running_ = true;
        thread_ = std::thread(&evloop<EventType>::loop, this);
    }

    void pause() {
        running_ = false;
        events_condition_.notify_one();
        thread_.join();
    }

    void stop() {
        running_ = false;
        events_condition_.notify_one();
        events_.clear();
        callbacks_.clear();
        thread_.join();
    }

    // register event and its callback
    void register_event(int event_id, callback_t function) {
        std::unique_lock<std::mutex> lock(events_lock_);
        callbacks_.insert(std::make_pair(event_id, std::move(function)));
    }

    void unregister_event(int event_id) {
        std::unique_lock<std::mutex> lock(events_lock_);
        callbacks_.erase(event_id);
    }

    // send event for immediate execution
    void send(int event_id, EventType evt) {
        post(event_id, std::move(evt), 0);
    }

    // post event for delayed execution, default duration is in milliseconds
    template<typename Duration = std::chrono::milliseconds>
    void post(int event_id, EventType evt, int duration_value = 10) {
        static_assert(is_chrono_duration<Duration>::value, "Duration must be a std::chrono::duration");
        auto duration = Duration(duration_value);
        {
            std::unique_lock<std::mutex> lock(events_lock_);
            events_.insert(std::make_pair(std::move(std::chrono::high_resolution_clock::now() + duration),
                                          std::bind(&evloop::defer,
                                                    this, event_id, std::move(evt), std::placeholders::_1)));
        }
        // wake up when new event coming
        events_condition_.notify_one();
    }

private:
    void loop() {
        std::unique_lock<std::mutex> lock(events_lock_);
        while (running_) {
            auto next_event = std::chrono::time_point<std::chrono::high_resolution_clock>::max();
            if (!events_.empty())
                next_event = events_.begin()->first;
            // wait until:
            // 1. new event coming
            // 2. terminate or event with smaller timestamp detected when refresh events
            events_condition_.wait_until(lock, next_event, [&]() {
                return !running_ || (!events_.empty() && events_.begin()->first < next_event);
            });
            if (next_event <= std::chrono::high_resolution_clock::now()) {
                auto it = events_.begin();
                auto func = it->second;
                events_.erase(it);
                // temporarily releases lock
                func(lock);
            }
        }
    }

    void defer(int event_id, EventType evt, std::unique_lock<std::mutex> &lock) {
        // make a copy in case events changed
        auto range = callbacks_.equal_range(event_id);
        std::vector<callback_t> functions;
        for (auto it = range.first; it != range.second; it++) {
            functions.push_back(it->second);
        }
        // run copy without lock
        lock.unlock();
        for (auto &function: functions) {
            function(evt);
        }
        // afterwards lock again
        lock.lock();
    }
};

#endif //CPP_TOY_EVLOOP_H
