//
// Created by Harold on 2020/8/7.
//

#include "evloop.h"
#include <iostream>

struct event {
    evloop<event>& evloop;
    static const int id = 0;
};

int main() {
    evloop<event> dispatcher;
    dispatcher.register_event(event::id, [](const event& evt) {
        std::cout << "repeated event run after 1000 milli-seconds\n";
        // repeat
        evt.evloop.post(event::id, evt, 1000);
    });
    dispatcher.start();
    auto start = std::chrono::high_resolution_clock::now();
    event evt{dispatcher};
    dispatcher.post(event::id, evt, 1000);
    std::this_thread::sleep_for(std::chrono::seconds(10));
    dispatcher.pause();
    std::cout << "time consumption: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::high_resolution_clock::now() - start).count()
              << "ms"
              << std::endl;

    std::cout << "-------------------------------------------" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // former is pause, so here need unregister first
    dispatcher.unregister_event(event::id);
    dispatcher.register_event(event::id, [](const event& evt) {
        std::cout << "repeated event run after 2 seconds\n";
        // change duration base to seconds
        evt.evloop.post<std::chrono::seconds>(event::id, evt, 2);
    });
    dispatcher.start();
    start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    dispatcher.stop();
    std::cout << "time consumption: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::high_resolution_clock::now() - start).count()
              << "ms"
              << std::endl;

    std::cout << "-------------------------------------------" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // former is stop, no need to unregister
    dispatcher.register_event(event::id, [](const event& evt) {
        std::cout << "call once" << std::endl;
    });
    dispatcher.start();
    dispatcher.send(event::id, evt);
    start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    dispatcher.stop();
    std::cout << "time consumption: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::high_resolution_clock::now() - start).count()
              << "ms"
              << std::endl;

    return 0;
}
