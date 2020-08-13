//
// Created by Harold on 2020/8/13.
//

#include "meta_func_class.h"
#include <type_traits>
#include <tuple>
#include <iostream>
#include "type.h"

template<typename T>
using remove_ptr_a = typename std::remove_pointer<T>::type;

struct remove_ptr_b {
    template<typename T>
    struct apply {
        using type = typename std::remove_pointer<T>::type;
    };
};

int main() {
    using input = std::tuple<char*, int*, float**>;
    using expected = std::tuple<char, int, float*>;

    using result_a = transform_a<remove_ptr_a, input>;
    using result_b = transform_b<remove_ptr_b, input>;

    static_assert(std::is_same<result_a, expected>::value, "transform_a failed");
    static_assert(std::is_same<result_b, expected>::value, "transform_b failed");

    std::cout << type(result_a()) << std::endl;
    std::cout << type(result_b()) << std::endl;

    return 0;
}
