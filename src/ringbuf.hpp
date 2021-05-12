#pragma once
#include <stdint.h>

namespace hal {

template<uint8_t BUFSIZE>
class RingBuffer {
    volatile uint8_t buffer[BUFSIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
    
public:
    bool empty() const { return head == tail; }
    
    void remove() {
        if (empty())
            return;
        tail = tail + 1 % BUFSIZE;
    }
    
    uint8_t peek() const {
        assert(!empty());
        return buffer[tail];
    }
    
    void add(uint8_t val) {
        buffer[head] = val;
        head = head + 1 % BUFSIZE;
        if (head == tail)
            tail = tail + 1 % BUFSIZE;
    }
    
    uint8_t count() const {
        return head >= tail ? head - tail : BUFSIZE - tail + head;
    }
    
    uint8_t empty_capacity() const {
        return BUFSIZE - count();
    }
};
}
