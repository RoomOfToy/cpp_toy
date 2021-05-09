//
// Created by Harold on 2021/5/9.
//

#include "equal.h"

#include <vector>
#include <list>
#include <iostream>
#include <tuple>

struct A {
    int a;
    
    friend bool operator==(A const& lhs, A const& rhs) {
        return M_Equal(lhs, rhs, &A::a);
    }
};

struct B {
    A a;
    double b;
    std::vector<int> c;
    std::list<int> d;

    friend bool operator==(B const& lhs, B const& rhs) {
        return M_Equal(lhs, rhs,
                      &B::a,
                      &B::b,
                      &B::c,
                      &B::d);
    }
};

struct C {
    B b;
    int* ptr = nullptr;

    friend bool operator==(C const& lhs, C const& rhs) {
        return M_Equal(lhs, rhs, &C::b) && ((lhs.ptr == nullptr && rhs.ptr == nullptr) || (lhs.ptr && rhs.ptr && *lhs.ptr == *rhs.ptr));  // ptr requires extra consideration
    }

    ~C() {
        if (ptr != nullptr)
            delete ptr;
    }
};

struct D {
    B b;
    int* ptr = nullptr;

    friend bool operator==(D const& lhs, D const& rhs) {
        return M_Equal(lhs, rhs, &D::b, &D::ptr);  // handle pointer specially in template
    }

    ~D() {
        if (ptr != nullptr)
            delete ptr;
    }
};

struct E {
    int a;
    double b;
    std::vector<int> c;
    std::list<int> d;

    friend bool operator==(E const& lhs, E const& rhs) {
        return std::tie(lhs.a, lhs.b, lhs.c, lhs.d) == std::tie(rhs.a, rhs.b, rhs.c, rhs.d);  // construct tuple to compare
    }

    friend bool operator<(E const& lhs, E const& rhs) {
        return std::tie(lhs.a, lhs.b, lhs.c, lhs.d) < std::tie(rhs.a, rhs.b, rhs.c, rhs.d);
    }
};

int main() {

    B b;
    b.a = A();
    b.b = 0.1;
    b.c.push_back(5);
    b.d.push_back(6);

    B bb = b;
    std::cout << (bb == b) << std::endl;

    bb.c.push_back(7);
    std::cout << (bb == b) << std::endl;

    C c;
    C cc = c;
    std::cout << (cc == c) << std::endl;

    cc.ptr = new int(10);
    std::cout << (cc == c) << std::endl;

    D d;
    D dd = d;
    std::cout << (dd == d) << std::endl;

    dd.ptr = new int(10);
    std::cout << (dd == d) << std::endl;

    E e;
    E ee = e;
    std::cout << (ee == e) << std::endl;

    ee.c.push_back(5);
    std::cout << (ee == e) << std::endl;
    std::cout << (e < ee) << std::endl;
    e.a = 100;
    std::cout << (ee < e) << std::endl;

    return 0;
}