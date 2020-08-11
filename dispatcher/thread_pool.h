//
// Created by Harold on 2020/8/11.
//

// reference: https://github.com/vit-vit/CTPL

#ifndef CPP_TOY_THREAD_POOL_H
#define CPP_TOY_THREAD_POOL_H

#include <queue>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include <functional>
#include <memory>
#include <future>

template<typename T>
class safe_queue {
private:
    std::queue<T> q_;
    std::mutex lock_;
    using locker = std::unique_lock<std::mutex>;

public:
    safe_queue(): q_(), lock_() {}
    bool empty() {
        locker _(lock_);
        return q_.empty();
    }
    bool push(const T& v) {
        locker _(lock_);
        q_.push(v);
        return true;
    }
    bool pop(T& v) {
        locker _(lock_);
        if (q_.empty())
            return false;
        v = q_.front();
        q_.pop();
        return true;
    }
};

class thread_pool {
private:
    std::vector<std::unique_ptr<std::thread>> threads_;
    using flag_t = std::atomic<bool>;
    std::vector<std::shared_ptr<flag_t>> threads_stop_flags_;
    using task_t = std::function<void()>;
    safe_queue<task_t*> tasks_;
    std::mutex lock_;
    std::condition_variable condition_;
    // thread stop when tasks done or pool stop
    flag_t tasks_done_;
    flag_t pool_stop_;
    std::atomic<int> n_idle;
    using locker = std::unique_lock<std::mutex>;

public:
    thread_pool() : tasks_done_(false), pool_stop_(false), n_idle(0) { }
    explicit thread_pool(size_t n_threads) : thread_pool() {
        threads_.resize(n_threads);
        threads_stop_flags_.resize(n_threads);
        for (size_t i = 0; i < n_threads; i++) {
            threads_stop_flags_[i] = std::make_shared<flag_t>(false);
            setup_thread(i);
        }
    }
    // non-copyable
    thread_pool(const thread_pool &) = delete;
    thread_pool& operator=(const thread_pool &) = delete;
    // non-movable
    thread_pool(thread_pool &&) = delete;
    thread_pool& operator=(thread_pool &&) = delete;
    ~thread_pool() { stop(true); }

    // stop accepting new tasks and wait for all tasks done
    void stop(bool wait = false) {
        if (!wait) {
            if (pool_stop_)
                return;
            pool_stop_ = true;
            for (size_t i = 0, n = threads_.size(); i < n; i++)
                *threads_stop_flags_[i] = true;
            // clear tasks to avoid idle threads fetch task
            clear_tasks();
        } else {
            if (tasks_done_ || pool_stop_)
                return;
            tasks_done_ = true;
        }
        {
            locker _(lock_);
            // notify all waiting threads to stop
            condition_.notify_all();
        }
        // wait for finishing running task
        for (auto & thread : threads_)
            if (thread->joinable())
                thread->join();
        clear_tasks();
        threads_.clear();
        threads_stop_flags_.clear();
    }

    void clear_tasks() {
        task_t* tp = nullptr;
        while (tasks_.pop(tp))
            delete tp;
    }

private:
    void setup_thread(size_t i) {
        std::shared_ptr<flag_t> fp(threads_stop_flags_[i]);
        auto loop_f = [this, fp]() {
            flag_t& stop = *fp;
            task_t* tp = nullptr;
            bool has_next = tasks_.pop(tp);
            while (true) {
                while (has_next) {
                    // drop task function after execution
                    std::unique_ptr<task_t> utp(tp);
                    // execute task
                    (*tp)();
                    // if stop flag set, then stop loop
                    if (stop)
                        return;
                    // fetch next task
                    has_next = tasks_.pop(tp);
                }
                // no tasks now
                locker locker_(lock_);
                ++n_idle;
                // check whether new task coming or stop
                condition_.wait(locker_, [this, &tp, &has_next, &stop](){
                    has_next = tasks_.pop(tp);
                    return stop || tasks_done_ || has_next;
                });
                --n_idle;
                // no new task coming, then stop
                if (!has_next)
                    return;
            }
        };
        // bind loop_f to thread
        threads_[i].reset(new std::thread(loop_f));
    }

public:
    inline size_t size() const { return threads_.size(); }
    inline size_t idle_threads() const { return n_idle; }
    void resize(size_t n_threads) {
        if (!pool_stop_ && !tasks_done_) {
            size_t old_n_threads = threads_.size();
            // expand
            if (old_n_threads <= n_threads) {
                threads_.resize(n_threads);
                threads_stop_flags_.resize(n_threads);
                for (size_t i = old_n_threads; i < n_threads; i++) {
                    threads_stop_flags_[i] = std::make_shared<flag_t>(false);
                    setup_thread(i);
                }
            } else {
                // shrink
                for (size_t i = old_n_threads - 1; i >= n_threads; i--) {
                    *threads_stop_flags_[i] = true;
                    // detach first to let running task complete
                    threads_[i]->detach();
                }
                {
                    locker _(lock_);
                    // notify detached thread to stop
                    condition_.notify_all();
                }
                threads_.resize(n_threads);
                threads_stop_flags_.resize(n_threads);
            }
        }
    }

public:
    template<typename F, typename ...Args>
    auto push(F&& f, Args&& ...args)
    -> std::future<decltype(f(args...))> {
        auto pck = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        auto tp = new task_t([pck](){ (*pck)(); });
        tasks_.push(tp);
        locker _(lock_);
        condition_.notify_one();
        return pck->get_future();
    }

    template<typename F>
    auto push(F&& f)
    -> std::future<decltype(f())> {
        auto pck = std::make_shared<std::packaged_task<decltype(f())()>>(std::forward<F>(f));
        auto tp = new task_t([pck](){ (*pck)(); });
        tasks_.push(tp);
        locker _(lock_);
        condition_.notify_one();
        return pck->get_future();
    }

    task_t pop() {
        task_t* tp = nullptr;
        tasks_.pop(tp);
        task_t task;
        std::unique_ptr<task_t> utp(tp);
        if (tp)
            task = *tp;
        return task;
    }
};

#endif //CPP_TOY_THREAD_POOL_H
