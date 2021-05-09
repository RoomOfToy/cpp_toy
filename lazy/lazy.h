//
// Created by Harold on 2021/5/9.
//

#ifndef CPP_TOY_LAZY_H
#define CPP_TOY_LAZY_H

#include "m_optional.h"

#include <functional>

template<typename T>
struct Lazy {
public:
	template<typename Func, typename ...Args>
	Lazy(Func& f, Args && ...args) { func = [&]() { return f(args...); }; }

	bool HasValue() { return value.has_value(); }

	T& GetValue() {
		if (!value.has_value())
			value = func();
		return *value;
	}

private:
	std::function<T()> func;
	M_Optional<T> value;
};

template<typename Func, typename ...Args>
Lazy<typename std::result_of<Func(Args...)>::type>
lazy_eval(Func&& f, Args&& ...args) {
	return Lazy<typename std::result_of<Func(Args...)>::type>(std::forward<Func>(f), std::forward<Args>(args)...);
}

#endif //CPP_TOY_LAZY_H