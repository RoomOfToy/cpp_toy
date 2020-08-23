//
// Created by Harold on 2020/8/23.
//

#include "range.h"
#include <iostream>

int main() {
    int expected = 0;
    for (auto i : range(10)) {
        if (i != expected)
            std::cout << "wrong: " << i << std::endl;
        else
            std::cout << i << '\n';
        ++expected;
    }

    expected = 10;
    for (auto i : range(10, 15)) {
        if (i != expected)
            std::cout << "wrong: " << i << std::endl;
        else
            std::cout << i << '\n';
        ++expected;
    }

    expected = 15;
    for (auto i : range(15, 10, -1)) {
        if (i != expected)
            std::cout << "wrong: " << i << std::endl;
        else
            std::cout << i << '\n';
        --expected;
    }

    auto expected_f = 3.14;
    for (auto i : range(expected_f, 10.0, expected_f)) {
        if ((i - expected_f) > 1e-6)
            std::cout << "wrong: " << i << std::endl;
        else
            std::cout << i << '\n';
        expected_f += expected_f;
    }

    expected_f = 20.0;
    auto step = 3.14;
    int cnt = 0;
    for (auto i : range(20.0, 10.0, -step)) {
        if ((i - expected_f) > 1e-6)
            std::cout << "wrong: " << i << std::endl;
        else
            std::cout << i << '\n';
        ++cnt;
        expected_f = 20.0 - cnt * step;
    }

    return 0;
}
