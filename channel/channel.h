//
// Created by Harold on 2022/4/24.
//

#pragma once

#include <queue>
#include <mutex>
#include <type_traits>
#include <iterator>
#include <stdexcept>

struct channel_closed_exception : public std::runtime_error
{
    explicit channel_closed_exception(char const* msg) : std::runtime_error{ msg } { }
};

template<typename T>
class channel
{
public:
    using value_type = T;
    using size_type = std::size_t;

    // set max capacity or infinite
    explicit constexpr channel(size_type capacity = 0) : cap_(capacity) { }
    virtual ~channel() = default;

    // no copy/move
    channel(channel const&) = delete;
    channel(channel&&) = delete;
    channel& operator=(channel const&) = delete;
    channel& operator=(channel&&) = delete;

    constexpr size_type size() const noexcept { return que_.size(); }
    constexpr bool empty() const noexcept { return que_.empty(); }
    bool closed() const noexcept { return closed_.load(); }
    void close() noexcept { closed_.store(true); cnv_.notify_all(); }

    template<typename U>
    friend void operator>>(U&& in, channel<typename std::remove_cv<typename std::remove_reference<U>::type>::type>& chan);  // C++11 std::remove_cv<std::remove_reference<T>::type>::type; C++14 std::remove_cv_t<std::remove_reference_t<T>>; C++20 std::remove_cvref_t<T>

    template<typename U>
    friend void operator<<(U& out, channel<U>& chan);

private:
    size_type const cap_;
    std::queue<T> que_;
    std::mutex mut_;
    std::condition_variable cnv_;
    std::atomic<bool> closed_{ false };
};

// enqueue
template <typename U>
void operator>>(U &&in, channel<typename std::remove_cv<typename std::remove_reference<U>::type>::type> &chan) // C++11 std::remove_cv<std::remove_reference<T>::type>::type; C++14 std::remove_cv_t<std::remove_reference_t<T>>; C++20 std::remove_cvref_t<T>
{
    // not closed
    if (chan.closed()) throw channel_closed_exception{"write to closed channel is invalid"};

    {
        // wait if has max capacity and full
        std::unique_lock<std::mutex> lk(chan.mut_); // lock
        if (chan.cap_ > 0 && chan.cap_ == chan.que_.size()) chan.cnv_.wait(lk, [&chan]() { return chan.cap_ > chan.que_.size(); });

        chan.que_.push(std::forward<U>(in));
    } // unlock

    chan.cnv_.notify_one();
}

// dequeue
template <typename U>
void operator<<(U &out, channel<U> &chan)
{
    // can read from closed channel
    if (chan.closed() && chan.empty()) return;

    {
        // wait if empty and not closed
        std::unique_lock<std::mutex> lk(chan.mut_); // lock
        chan.cnv_.wait(lk, [&chan]() { return !chan.empty() || chan.closed(); });

        if (!chan.empty())
        {
            out = std::move(chan.que_.front());
            chan.que_.pop();
        }
    } // unlock

    chan.cnv_.notify_one();
}
