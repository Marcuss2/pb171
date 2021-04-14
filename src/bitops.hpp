#pragma once

#include <stdint.h>
#include "concepts.hpp"
#include "types.hpp"

uint32_t bit(unsigned_integral auto n) {
    return 1u << n;
}

template<integral T>
T bitClear(T x, unsigned_integral auto n) {
    return x & ~(bit(n));
}

bool bitRead(integral auto x, unsigned_integral auto n) {
    return x & bit(n);
}

template<integral T>
T bitSet(T x, unsigned_integral auto n) {
    return x | bit(n);
}

template<integral T>
T bitWrite(T x, unsigned_integral auto n, bool b) {
    x = bitClear(x, n);
    if (b) {
        x = bitSet(x, n);
    }
    return x;
}

uint8_t lowByte(integral auto x) {
    const uint8_t MASK = 0xff;
    return x & MASK;
}

uint8_t highByte(integral auto x) {
    x = x >> 8;
    return lowByte(x);
}

void setBit(uintptr_t addr, bool val, uint8_t pos) {
    vol_ptr ptr = reinterpret_cast<vol_ptr>(addr);
    uint8_t temp = *ptr;
    *ptr = bitWrite(temp, pos, val);
}

bool readBit(uintptr_t addr, uint8_t pos) {
    vol_ptr ptr = reinterpret_cast<vol_ptr>(addr);
    uint8_t temp = *ptr;
    return temp & (bit(pos));
}
