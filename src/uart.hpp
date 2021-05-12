#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "ringbuf.hpp"

namespace hal {



enum LookaheadMode {
    SKIP_ALL,
    SKIP_NONE,
    SKIP_WHITESPACE,
};

enum Format {
    BIN = 2,
    OCT = 8,
    DEC = 10,
    HEX = 16,
};

template<uintptr_t BUFSIZE>
class SerialClass {
    RingBuffer<64> sendBuffer;
    RingBuffer<64> receiveBuffer;
    
    bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }
    
    bool evaluateLookahead(LookaheadMode mode, char c) {
        if (isDigit(c) || c == '-')
            return false;
        if (mode == LookaheadMode::SKIP_NONE)
            return false;
        if (mode == LookaheadMode::SKIP_WHITESPACE && c != '\t' && c != '\n' && c != '\r' && c != ' ')
            return false;
        return true;
    }
    
    void readAhead(LookaheadMode mode) {
        while (true) {
            skipTillAvaiable();
            char byte = receiveBuffer.peek();
            if (!evaluateLookahead(byte))
                return;
            receiveBuffer.remove();
        }
    }
    
    void skipTillAvaiable() {
        while (avaiable() == 0) {}
    }
    
public:
    uint8_t avaiable() const {
        return receiveBuffer.count();
    }
    
    uint8_t avaiableForWrite() const {
        return sendBuffer.empty_capacity();
    }
    
    void begin() {
        // TODO
    }
    
    void end() {
        // TODO
    }
    
    void flush() {
        while (sendBuffer.count() != 0) {}
        // TODO check if internal register is empty
    }
    
    // TODO add interrupts
    
    int peek() const {
        if (avaiable() == 0) {
            return -1;
        }
        return receiveBuffer.peek();
    }
    
    long parseInt(LookaheadMode mode = LookaheadMode::SKIP_ALL) {
        readAhead(mode);
        bool minus = false;
        if (peek() == '-') {
            minus = true;
            receiveBuffer.remove();
            skipTillAvaiable();
        }
        if (!isDigit(peek()))
            return 0;
        long val = peek() - '0';
        receiveBuffer.remove();
        skipTillAvaiable();
        while(isDigit(peek())) {
            val = val * 10;
            val += (peek() - '0');
            receiveBuffer.remove();
            skipTillAvaiable();
        }
        return minus ? -val : val;
    }
    
    uintptr_t write(uint8_t val) {
        while (avaiableForWrite() == 0) {}
        
        sendBuffer.add(val);
        
        return 1;
    }
    
    uintptr_t write(const char* str) {
        uintptr_t count = 0;
        for (char c = str[count]; c != '\0'; count++) {
            write(c);
        }
        return count;
    }
    
    uintptr_t print(const char* str) {
        return write(str);
    }
    
    uintptr_t print(long val, Format format = Format::DEC) {
        // Enough for 32bit signed minimum including null terminator
        char buf[12];
        
        const char* str = ltoa(val, buf, format);
        
        return print(str);
    }
    
    uintptr_t println(const char* str) {
        auto count = print(str);
        count += print("\r\n");
        return count;
    }
    
    uintptr_t println(long val, Format format = Format::DEC) {
        auto count = print(val, format);
        count += print("\r\n");
        return count;
    }
    
    int read() {
        if (avaiable() == 0)
            return -1;
        auto byte = receiveBuffer.peek();
        receiveBuffer.remove();
        return byte;
    }
    
    uintptr_t readBytes(uint8_t* buf, uintptr_t len) {
        uintptr_t readCount = 0;
        while (avaiable() != 0 && readCount < len) {
            buf[readCount] = static_cast<uint8_t>(read());
            ++readCount;
        }
        return readCount;
    }
};

}
