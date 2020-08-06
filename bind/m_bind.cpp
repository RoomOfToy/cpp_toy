//
// Created by Harold on 2020/8/6.
//

#include "m_bind.h"
#include <iostream>
#include <memory>

void sf(int i) {
    std::cout << i << std::endl;
}

void f(int n1, int n2, int n3, const int& n4, int n5)
{
    std::cout << n1 << ' ' << n2 << ' ' << n3 << ' ' << n4 << ' ' << n5 << '\n';
}

int g(int i)
{
    return i;
}

struct Foo {
    void print_sum(int n1, int n2)
    {
        std::cout << n1+n2 << '\n';
    }
    int data = 10;
};

int main() {

    m_bind::bind(sf, 2)();
    std::bind(sf, 2)();
    auto sg = [](int i){
        std::cout << i << std::endl;
    };
    std::bind(sg, 2)();

    // m_bind::bind not support lambda function
    // can not construct a std::function from lambda directly
    // need to assign lambda to it,
    // e.g. std::function<void(int i)> lambda = [](int i) { return; };

    //m_bind::bind(sg, 2)();


    using namespace std::placeholders;

    int n = 7;
    auto f1 = m_bind::bind(f, _2, 42, _1, std::cref(n), n);
    n = 10;
    f1(1, 2, 1001);

    // not support nested
    //auto f2 = m_bind::bind(f, _3, m_bind::bind(g, _3), _3, 4, 5);
    //f2(10, 11, 12);

    // not support to bind to a pointer to member function
    // related to std::function
    //Foo foo;
    //auto f3 = m_bind::bind(&Foo::print_sum, &foo, 95, _1);
    //f3(5);

    // neither to bind to a pointer to data member
    // also related to std::function
    //auto f4 = m_bind::bind(&Foo::data, _1);
    //std::cout << f4(foo) << '\n';

    return 0;
}

