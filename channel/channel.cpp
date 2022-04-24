//
// Created by Harold on 2022/4/24.
//

#include "channel.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <thread>

void expect(std::string const& str, bool v)
{
    std::cout << std::boolalpha;
    std::cout << str << ": " << v << '\n';
}

int main()
{
    channel<int> ichan;
    0 >> ichan;
    1 >> ichan;
    2 >> ichan;
    expect("channel size == 3", ichan.size() == 3);

    int iout = -1;
    iout << ichan;
    expect("0 == 0", iout == 0);
    iout << ichan;
    expect("1 == 1", iout == 1);
    iout << ichan;
    expect("2 == 2", iout == 2);
    expect("channel size == 0", ichan.size() == 0);

    channel<std::string> schan;
    std::string("hello") >> schan;
    std::string sout;
    sout << schan;
    expect("hello == hello", sout == "hello");

    ichan.close();
    expect("channel closed", ichan.closed());
    try
    {
        1 >> ichan;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    channel<int> chan(10);
    unsigned num = 1000;
    unsigned expected_sum = 500500;
    std::atomic<unsigned> cnt{0};
    std::atomic<unsigned> sum{0};

    // feed in one element to make sure no starvation
    // TODO: solve it more wisely
    0 >> chan;

    auto producer = [&]()
    {
        for (int i = 1; i <= num; i++)
            i >> chan;
    };
    auto consumer = [&]()
    {
        while (cnt < num)
        {
            int out = 0;
            out << chan;
            sum += out;
            ++cnt;
        }
    };
    std::thread t1(producer);
    std::thread t2(consumer);
    std::thread t3(consumer);

    t1.join();
    t2.join();
    t3.join();

    expect("expected_sum == sum", expected_sum = sum);
    
    return 0;
}