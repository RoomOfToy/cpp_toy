//
// Created by Harold on 2022/5/9.
//

#pragma once

#include <functional>
#include <vector>

struct Slot
{
    int idx = -1;
    bool disconnected = false;
};

template<typename Ret, typename ...Args>
struct Signal
{
    using func_type = std::function<Ret(Args...)>;

    Slot connect(func_type&& func)
    {
        funcs.emplace_back(std::forward<func_type>(func), false);
        return { int(funcs.size()) };
    }

    void disconnect(Slot& slot)
    {
        if (slot.disconnected) return;
        slot.disconnected = true;
        funcs[slot.idx].second = true;
    }

    std::vector<Ret> operator()(Args ...args)
    {
        std::vector<Ret> rets;
        for (auto const& f : funcs)
        {
            if (f.second == true) continue;
            rets.push_back(f.first(args...));
        }
        return rets;
    }

private:
    std::vector<std::pair<func_type, bool>> funcs;  // <func, disconnected>
};