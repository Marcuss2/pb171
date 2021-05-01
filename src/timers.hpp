#pragma once

#include <avr/interrupt.h>
#include "bitops.hpp"
#include "pins.hpp"


static volatile uint32_t ctime = 0;
static volatile uint8_t fraction = 0;

// 16 000 000 / 64 / 250 = 1000
// Perfect for prescale

ISR(TIMER0_OVF_vect) {
    auto local_time = ctime;
    ++local_time;
    ctime = local_time;
}

uint32_t millis() {
    cli();
    auto t = ctime;
    sei();
    return t;
}

namespace hal {

constexpr uintptr_t CTIMSK0 = 0x6E;

void setupTimer() {
    CPD6.setupPWM();
    /*const uintptr_t PRESCALE_ADDR = TCCR0B;
    const auto CSx0 = 0;
    setBit(PRESCALE_ADDR, true, CSx0);
    const auto CSx1 = 1;
    setBit(PRESCALE_ADDR, true, CSx1);
    */
    
    const auto CTOIE0 = 0;
    setBit(CTIMSK0, true, CTOIE0);
}





}



