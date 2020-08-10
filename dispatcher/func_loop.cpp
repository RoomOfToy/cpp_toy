//
// Created by Harold on 2020/8/10.
//

#include "func_loop.h"
#include <assert.h>
#include <iostream>
#include <chrono>

int main() {
    func_loop fl;
    assert(fl.size() == 0);
    fl.start();
    auto t1 = fl.push([](){
        std::cout << "lambda task 1" << "\n";
    });
    t1.get();

    auto t2 = fl.push([](int x){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "lambda task 2: after 2 seconds, get " << x << "\n";
    }, 2);
    t2.get();

    for (int i = 0; i < 10; i ++) {
        fl.push([i](){ std::cout << "number: " << i << "\n"; });
    }
    auto task = fl.pop();
    std::cout << typeid(task).name() << "\n";

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << std::endl;

    return 0;
}
