//
// Created by Harold on 2021/5/9.
//

#include "lazy.h"

#include <iostream>

int main() {
    auto f = [](int i, int j) { std::cout << "lazy evaluated: " << i + j << std::endl; return i + j; };
    auto res = lazy_eval(f, 1, 2);
    std::cout << "not evaluted yet" << std::endl;
    res.GetValue();

    return 0;
}