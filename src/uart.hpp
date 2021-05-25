#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include "pins.hpp"
#include "bitops.hpp"
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

constexpr auto CUDR0 = 0xC6;

template<uintptr_t BUFSIZE>
class SerialClass {

    
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
            if (!evaluateLookahead(mode, byte))
                return;
            receiveBuffer.remove();
        }
    }
    
    void skipTillAvaiable() {
        while (avaiable() == 0) {}
    }
    
public:
    RingBuffer<BUFSIZE> sendBuffer;
    RingBuffer<BUFSIZE> receiveBuffer;
    
    uint8_t avaiable() const {
        return receiveBuffer.count();
    }
    
    uint8_t avaiableForWrite() const {
        return sendBuffer.empty_capacity();
    }
    
    static constexpr auto CTXEN0 = 3;
    static constexpr auto CRXEN0 = 4;
    static constexpr auto CRXCIEn = 7;
    static constexpr auto CUDRIEn = 5;
    static constexpr uintptr_t CUCSR0B = 0xC1;
    
    void begin() {
        cli();
        {
            CPD0.setInputMode();
            CPD1.setOutputMode();
            // Baud rate 38.4k bps
            constexpr uintptr_t CUBRR0H = 0xC5;
            setByte(CUBRR0H, 416 >> 8);
            constexpr uintptr_t CUBRR0L = 0xC4;
            setByte(CUBRR0L, 416 & 0xFF);
        
            // Default usart mode
             constexpr auto CUCSR0C = 0xC2;
             setByte(CUCSR0C, 0x06);
            
            // Enable transmitter and receiver
            
            setBit(CUCSR0B, true, CTXEN0);
            setBit(CUCSR0B, true, CRXEN0);
            setBit(CUCSR0B, true, CRXCIEn);
            setBit(CUCSR0B, true, CUDRIEn);
        }
        sei();
    }
    
    void end() {
        cli();
        // Disabling transmitter and receiver
        setBit(CUCSR0B, false, CTXEN0);
        setBit(CUCSR0B, false, CRXEN0);
        setBit(CUCSR0B, false, CRXCIEn);
        setBit(CUCSR0B, false, CUDRIEn);
        sei();
    }
    
    static constexpr uintptr_t CUCSRnA = 0xC0;
    
    void flush() {
        while (sendBuffer.count() != 0) {}
        
        constexpr auto TXCn = 6;
        while (!readBit(CUCSRnA, TXCn)) {};
    }
    
    int peek() const {
        if (avaiable() == 0) {
            return -1;
        }
        return receiveBuffer.peek();
    }
    
    long parseInt(LookaheadMode mode = LookaheadMode::SKIP_ALL) {
        readAhead(mode);
        
        // Minus handling
        bool minus = false;
        if (peek() == '-') {
            minus = true;
            receiveBuffer.remove();
            skipTillAvaiable();
        }
        
        // First digit
        if (!isDigit(peek()))
            return 0;
        long val = peek() - '0';
        receiveBuffer.remove();
        skipTillAvaiable();
        
        // Digits beyond
        while(isDigit(peek())) {
            val = val * 10;
            val += (peek() - '0');
            receiveBuffer.remove();
            skipTillAvaiable();
        }
        
        return minus ? -val : val;
    }
    
    
    static constexpr auto UDREn = 5;
    
    uintptr_t write(uint8_t val) {
        
        while (avaiableForWrite() == 0) {}
        
        sendBuffer.add(val);
        setByte(0x25, sendBuffer.count());
        
        cli();
        // Set the byte if the buffer is empty
        if (readBit(CUCSRnA, UDREn)) {
            setByte(CUDR0, sendBuffer.peek());
            sendBuffer.remove();
        }
        sei();
        
        return 1;
    }
    
    uintptr_t write(const char* str) {
        uintptr_t count = 0;
        for (;; ++count) {
            char c = str[count];
            if (c == '\0')
                break;
            write(c);
        }
        return count;
    }
    
    uintptr_t print(const char* str) {
        return write(str);
    }
    
    uintptr_t print(long val, Format format = Format::DEC) {
        // Enough for 32bit signed minimum including null terminator
        char buf[33];
        
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

SerialClass<64> Serial;

ISR(USART_RX_vect) {
    uint8_t data = readByte(CUDR0);
    Serial.receiveBuffer.add(data);
}

ISR(USART_UDRE_vect) {
    setByte(0x25, Serial.sendBuffer.count());
    if (!Serial.sendBuffer.empty()) {
        setByte(CUDR0, Serial.sendBuffer.peek());
        Serial.sendBuffer.remove();
    }
}

}

