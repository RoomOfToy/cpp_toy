//
// Created by Harold on 2021/5/9.
//

#ifndef CPP_TOY_EQUAL_H
#define CPP_TOY_EQUAL_H

#include <utility>
#include <type_traits>

// Pointer-to-Member Operators: .* and ->*
template <typename T, typename Field>
bool M_Equal(T const& lhs, T const& rhs, Field&& field) {
	return lhs.*field == rhs.*field;
}

// handle pointer specially (deep equal, but be careful about the real semantics)
template <typename T, typename Field, typename = typename std::enable_if<std::is_pointer<Field>::value>::type>
bool M_Equal(T const& lhs, T const& rhs, Field&& field) {
	return (lhs.*field == nullptr && rhs.*field == nullptr) || (lhs.*field && rhs.*field && *(lhs.*field) == *(rhs.*field));
}

template<typename T, typename Field, typename ...Fields>
bool M_Equal(T const& lhs, T const& rhs, Field&& field, Fields&& ...fields) {
	return M_Equal(lhs, rhs, field) && M_Equal(lhs, rhs, std::forward<Fields>(fields)...);
}

#endif // CPP_TOY_EQUAL_H