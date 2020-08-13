//
// Created by Harold on 2020/8/13.
//

#ifndef CPP_TOY_DEMANGLE_H
#define CPP_TOY_DEMANGLE_H

#include <string>
#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>

std::string demangle(const char* name) {
    int status = -4; // some arbitrary value to eliminate the compiler warning
    std::unique_ptr<char, void(*)(void*)> res {
            abi::__cxa_demangle(name, nullptr, nullptr, &status),
            std::free
    };
    return (status==0) ? res.get() : name ;
}

#else

// does nothing if not g++
std::string demangle(const char* name) {
    return name;
}

#endif

#endif //CPP_TOY_DEMANGLE_H
