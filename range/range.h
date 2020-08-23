//
// Created by Harold on 2020/8/23.
//

#ifndef CPP_TOY_RANGE_H
#define CPP_TOY_RANGE_H

namespace detail {
    template<typename T>
    class Range {
    private:
        T const begin_;
        T const step_;
        T const end_;
    public:
        Range(T const& begin, T const& end, T const& step)
                : begin_(begin), end_(end), step_(step) {
            if (step_ == 0 ||
               (begin_ > end_ && step_ > 0) ||
               (begin_ < end_ && step_ < 0))
                throw;
        }

    public:
        // used in range based for
        // https://en.cppreference.com/w/cpp/language/range-for
        class iterator {
        private:
            T value_;  // value here, no ptr
            T const step_;
            T const bound_;
            bool const direction_;  // true: forward(+), false: backward(-)
        public:
            iterator(T value, T step, T bound)
                    : value_(value), step_(step), bound_(bound), direction_(step > 0) { }
            iterator operator++() {
                value_ += step_;
                return *this;
            }
            // it == end_ to stop iteration
            // consider floating point value_, need value_ >= end_.value
            bool operator==(iterator const& rhs) {
                return direction_ ? (value_ >= rhs.value_ && value_ <= bound_)
                                  : (value_ <= rhs.value_ && value_ >= bound_);
            }
            bool operator!=(iterator const& rhs) {
                return direction_ ? (value_ < rhs.value_ && value_ < bound_)
                                  : (value_ > rhs.value_ && value_ > bound_);
            }
            T& operator*() {
                return value_;
            }
            T const* operator->() {
                return &value_;
            }
        };

    public:
        iterator begin() const {
            return iterator(begin_, step_, end_);
        }
        iterator end() const {
            return iterator(end_, step_, end_);
        }
    };
}

template<typename T>
detail::Range<T> range(T const& begin, T const& end, T const& step) {
    return detail::Range<T>(begin, end, step);
}

// 1 as default step
template<typename T>
detail::Range<T> range(T const& begin, T const& end) {
    return detail::Range<T>(begin, end, T{1});
}

// 0 as default begin, 1 as default step
template<typename T>
detail::Range<T> range(T const&end) {
    return detail::Range<T>(T{0}, end, T{1});
}

#endif //CPP_TOY_RANGE_H
