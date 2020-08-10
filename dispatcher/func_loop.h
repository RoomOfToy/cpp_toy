//
// Created by Harold on 2020/8/10.
//

#ifndef CPP_TOY_FUNC_LOOP_H
#define CPP_TOY_FUNC_LOOP_H

#include <mutex>
#include <functional>
#include <queue>
#include <atomic>
#include <future>
#include <thread>

class func_loop {
private:
    using task_t = std::function<void()>;
    using locker = std::unique_lock<std::mutex>;
    std::mutex lock_;
    std::condition_variable condition_;
    std::queue<task_t*> tasks_;
    std::atomic<bool> running_;
    std::thread thread_;

public:
    func_loop() : running_(false), tasks_() { }
    // non-copyable
    func_loop(const func_loop &) = delete;
    func_loop &operator=(const func_loop &) = delete;
    // non-movable
    func_loop(func_loop &&) noexcept = delete;
    ~func_loop() { stop(); }

    void start() {
        running_ = true;
        thread_ = std::thread(&func_loop::loop, this);
    }

    void stop() {
        running_ = false;
        clear_tasks();
        condition_.notify_one();
        thread_.join();
    }

public:
    template<typename F, typename ...Args>
    auto push(F&& f, Args&& ...args)
    -> std::future<decltype(f(args...))> {
        auto pck = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        auto task = new task_t([pck](){ (*pck)(); });
        locker _(lock_);
        tasks_.push(task);
        condition_.notify_one();
        return pck->get_future();
    }

    template<typename F>
    auto push(F&& f)
    -> std::future<decltype(f())> {
        auto pck = std::make_shared<std::packaged_task<decltype(f())()>>(std::forward<F>(f));
        auto tp = new task_t([pck](){ (*pck)(); });
        locker _(lock_);
        tasks_.push(tp);
        condition_.notify_one();
        return pck->get_future();
    }

    task_t pop() {
        locker _(lock_);
        std::unique_ptr<task_t> tp(tasks_.front());
        tasks_.pop();
        task_t task;
        if (tp)
            task = *tp;
        return task;
    }

    void clear_tasks() {
        locker _(lock_);
        task_t* tp;
        while (true) {
            tp = tasks_.front();
            if (tp) {
                tasks_.pop();
                delete tp;
            } else return;
        }
    }

    size_t size() {
        locker _(lock_);
        return tasks_.size();
    }

private:
    void loop() {
        locker locker_(lock_, std::defer_lock);
        while (running_) {
            locker_.lock();
            condition_.wait(locker_, [this]() {
                return !running_ || !tasks_.empty();
            });
            std::unique_ptr<task_t> tp(tasks_.front());
            tasks_.pop();
            locker_.unlock();
            (*tp)();
        }
    }
};

#endif //CPP_TOY_FUNC_LOOP_H
