//
// Created by Harold on 2021/3/12.
//

#ifndef CPP_TOY_FUNCS_LOOKUP_H
#define CPP_TOY_FUNCS_LOOKUP_H

#include<functional>
#include<unordered_map>

class FuncBase
{
public:
    FuncBase() = default;
    virtual ~FuncBase() = default;
};

template <typename T>
class detail_func : public FuncBase
{
    static_assert(std::integral_constant<T, false>::value, "Template parameter needs to be function type.");
};

template <typename Ret, typename... Args>
class detail_func<Ret(Args...)> : public FuncBase
{
public:
    detail_func(std::function<Ret(Args &&...)> _func) : func(_func) {}
    Ret operator()(Args &&...args) { return func(std::forward<Args>(args)...); }

private:
    std::function<Ret(Args &&...)> func;
};

void func1();
unsigned func2(int);
float func3(double, double);

// functions table
static std::unordered_map<std::string, std::shared_ptr<FuncBase>> Funcs = {
    {"func1", std::shared_ptr<FuncBase>(new detail_func<void()>(
                  []() { return func1(); }))},
    {"func2", std::shared_ptr<FuncBase>(new detail_func<unsigned(int)>(
                  [](int i) -> unsigned { return func2(i); }))},
    {"func3", std::shared_ptr<FuncBase>(new detail_func<float(double, double)>(
                  [](double d1, double d2) -> float { return func3(d1, d2); }))}};

#endif // CPP_TOY_FUNCS_LOOKUP_H                  