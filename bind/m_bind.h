//
// Created by Harold on 2020/8/6.
//

#ifndef CPP_TOY_M_BIND_H
#define CPP_TOY_M_BIND_H

#include <type_traits>
#include <tuple>
#include <utility>
#include <functional>

//######################### helper ###########################
template<std::size_t N>
using index_constant = std::integral_constant<std::size_t, N>;

template<bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template<typename T>
using remove_reference_t = typename std::remove_reference<T>::type;

template<typename T>
using remove_pointer_t = typename std::remove_pointer<T>::type;

template<size_t ...elements>
struct seq{};
template<size_t max, size_t... elements>
struct make_seq : make_seq<max-1, max-1, elements...> {};
template<size_t... elements>
struct make_seq<0U, elements...> {
    typedef seq<elements...> type;
};
template<size_t max>
using Make_Seq = typename make_seq<max>::type;
//###################### end of helper ########################

// args_list to hold all args
template<typename ...Args>
class args_list {
private:
    std::tuple<Args&& ...> bounded_args;
public:
    template<typename ...BArgs>
    args_list(BArgs&& ...args) noexcept
            : bounded_args(std::forward<BArgs>(args)...) { }

    // not placeholder
    template<typename T,
            enable_if_t<(std::is_placeholder<remove_reference_t<T>>::value == 0)>* = nullptr>
    auto operator[](T&& t) noexcept -> T&& {
        return std::forward<T>(t);
    }
    // placeholder, _1, _2, _3, ...
    template<typename T,
            enable_if_t<std::is_placeholder<T>::value != 0>* = nullptr>
    auto operator[](T) noexcept
    -> typename std::tuple_element<std::is_placeholder<T>::value - 1, std::tuple<Args...>>::type {
        return std::get<std::is_placeholder<T>::value - 1>(std::move(bounded_args));
    }
};

template<typename F, typename ...BoundedArgs>
class binder {
private:
    std::function<remove_reference_t<remove_pointer_t<F>>> func;
    std::tuple<BoundedArgs...> bounded_args;
    using res_t = typename std::function<remove_reference_t<remove_pointer_t<F>>>::result_type;
public:
    template<typename BF, typename ...BArgs>
    binder(BF&& f, BArgs&& ...args) noexcept
            : func{std::forward<BF>(f)},
              bounded_args{std::forward<BArgs>(args)...} { }

    template<typename ...Args>
    auto operator()(Args&& ...args)
    -> res_t {
        return call(Make_Seq<sizeof...(BoundedArgs)>{}, std::forward<Args>(args)...);
    }
private:
    template<typename ...Args, size_t ...S>
    auto call(seq<S...>, Args&& ...args)
    -> res_t {
        return func((args_list<Args...>{std::forward<Args>(args)...}
        [std::get<index_constant<S>{}>(bounded_args)])...);
    }
};

namespace m_bind {
    template<typename F, typename ...Args>
    binder<F, Args...> bind(F&& f, Args&& ...args) {
        return binder<F, Args...>{std::forward<F>(f), std::forward<Args>(args)...};
    }
}

#endif //CPP_TOY_M_BIND_H
