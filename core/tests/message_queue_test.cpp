#include <iostream>
#include "MessageQueue.hpp"

int main() {
    MessageQueue mq;

    mq.send(1, 2, "Hi, want to study Calculus?");
    mq.send(2, 1, "Yes, let's start with Calculus I.");
    mq.send(1, 2, "Cool, I will share notes.");

    while (!mq.empty()) {
        Message m = mq.receive();
        std::cout << "User " << m.from
                  << " -> User " << m.to
                  << ": " << m.content << "\n";
    }

    return 0;
}
