#pragma once
#include <queue>
#include "models.hpp"

/*
 * MessageQueue:
 * - FIFO chat simulation
 */

class MessageQueue {
private:
    std::queue<Message> q;

public:
    bool empty() const {
        return q.empty();
    }

    void send(int from, int to, const std::string& text) {
        Message m;
        m.from = from;
        m.to = to;
        m.content = text;
        q.push(m);
    }

    Message receive() {
        Message m = q.front();
        q.pop();
        return m;
    }
};
