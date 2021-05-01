#pragma once

#include <avr/interrupt.h>
#include "bitops.hpp"
#include "pins.hpp"

namespace hal {
    
static volatile uint32_t ctime = 0;
static volatile uint8_t fraction = 0;
static volatile uint32_t overflows = 0;

// 16 000 000 / 64 / 256 = 976.5625 | 9.765.625 time units
// so assuming one milisecond is 10.000.000 time units
// We can work with divider of 78125 and work with 125 and 128 time units respectively

ISR(TIMER0_OVF_vect) {
    auto local_fraction = fraction;
    local_fraction += 125;
    if (local_fraction >= 128) {
        auto local_time = ctime;
        ctime = ++local_time;
        local_fraction -= 128;
    }
    fraction = local_fraction;
    auto local_overflows = overflows;
    overflows = ++local_overflows;
}

constexpr uintptr_t CTIMSK0 = 0x6E;

uint32_t millis() {
    cli();
    auto local_time = ctime;
    sei();
    return local_time;
}

void setupTimer() {
    // Somehow setupPWM works, but the commented code which should
    // do the same thing does not, go figure
    CPD6.setupPWM();
    /*const auto COUNTER_CTRL_ADDR = TCCR0A;
    const auto WGMx0 = 0;
    setBit(COUNTER_CTRL_ADDR, true, WGMx0);
    const auto WGMx1 = 1;
    setBit(COUNTER_CTRL_ADDR, true, WGMx1);
    const auto CCOMxA1 = 7;
    setBit(COUNTER_CTRL_ADDR, true, CCOMxA1);
    const uintptr_t PRESCALE_ADDR = TCCR0B;
    const auto CSx0 = 0;
    setBit(PRESCALE_ADDR, true, CSx0);
    const auto CSx1 = 1;
    setBit(PRESCALE_ADDR, true, CSx1);*/
    const auto CTOIE0 = 0;
    setBit(CTIMSK0, true, CTOIE0);
}

void delay(uint32_t ms) {
    auto start = millis();
    auto end = start + ms;
    while (millis() != end) {}
}

uint32_t micros() {
    cli();
    auto local_overflows = overflows;
    sei();
    const uintptr_t CTCNT0 = 0x46;
    auto ticks = readByte(CTCNT0);
    return (local_overflows * 256 + ticks) * 4;
}

bool in_range(uint32_t start, uint32_t end, uint32_t val) {
    return val >= start && val <= end;
}

void delay_us(uint32_t us) {
    auto start = micros();
    auto end = start + us;
    auto end_adjusted = end + 16;
    // Very rare case around an overflow
    if (end_adjusted < end) {
        end = 0;
        end_adjusted = 16;
    }
    while (!in_range(end, end_adjusted, micros())) {}
}

}



