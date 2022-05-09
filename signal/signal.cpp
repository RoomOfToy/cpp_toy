#include "signal.h"
#include <iostream>
#include <iomanip>

int main()
{
    auto f1 = [](int x, int y) -> int { return x + y; };
    auto f2 = [](int x, int y) -> int { return x * y; };
    Signal<int, int, int> signal;
    auto slot1 = signal.connect(f1);
    auto slot2 = signal.connect(f2);
    auto rets = signal(1, 2);
    for (auto && e : rets)
        std::cout << e << " ";
    std::cout << '\n';

    signal.disconnect(slot1);
    std::cout << std::boolalpha << slot1.disconnected << '\n';

    rets = signal(3, 4);
    for (auto && e : rets)
        std::cout << e << " ";
    std::cout << '\n';

    return 0;
}