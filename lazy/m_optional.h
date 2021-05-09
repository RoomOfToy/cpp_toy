//
// Created by Harold on 2021/5/9.
//

#ifndef CPP_TOY_M_OPTIONAL_H
#define CPP_TOY_M_OPTIONAL_H

#include <type_traits>
#include <memory>

template<typename T>
class M_Optional {
public:
    constexpr M_Optional() noexcept : m_valid(false) { }
    M_Optional(T const& obj) noexcept(std::is_nothrow_copy_constructible<T>::value) : m_valid(true) {
        ::new (&m_storage) T(obj);
    }
    constexpr T& operator*() const {
        return *(T*)(&m_storage);
    }
    constexpr explicit operator bool() const noexcept {
        return m_valid;
    }
    constexpr bool has_value() const noexcept {
        return m_valid;
    }
    ~M_Optional() {
        if (m_valid)
            operator*().~T();
    }
private:
    typename std::aligned_storage<sizeof(T), alignof(T)>::type m_storage;
    bool m_valid;
};

#endif //CPP_TOY_M_OPTIONAL_H
