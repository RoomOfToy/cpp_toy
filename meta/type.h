//
// Created by Harold on 2020/8/13.
//

#ifndef CPP_TOY_TYPE_H
#define CPP_TOY_TYPE_H

#include <string>
#include <typeinfo>
#include "demangle.h"

template <class T>
std::string type(const T& t) {
    return demangle(typeid(t).name());
}

#endif //CPP_TOY_TYPE_H
