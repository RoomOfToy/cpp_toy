//
// Created by Harold on 2020/8/13.
//

#ifndef CPP_TOY_META_FUNC_CLASS_H
#define CPP_TOY_META_FUNC_CLASS_H

template<template<typename ...> class Func, typename Container>
struct m_transform;

template<template<typename ...> class Func,
         template<typename ...> class Container,
         typename ...Elements>
struct m_transform<Func, Container<Elements...>> {
    using type = Container<Func<Elements>...>;
};

template<template<typename ...> class Func, typename Container>
using transform_a = typename m_transform<Func, Container>::type;

//#############################################################//

template<typename Func, typename Container>
struct n_transform;

template<typename Func,
         template<typename ...> class Container,
         typename ...Elements>
struct n_transform<Func, Container<Elements...>> {
    template<typename T>
    using call = typename Func::template apply<T>::type;
    using type = Container<call<Elements>...>;
};

template<typename Func, typename Container>
using transform_b = typename n_transform<Func, Container>::type;

#endif //CPP_TOY_META_FUNC_CLASS_H
