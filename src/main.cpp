#include "pins.hpp"
#include "timers.hpp"
#include "uart.hpp"

void tracemode() {
    using namespace hal;
    setupTimer();
    CPB0.setOutputMode();
    CPB1.setOutputMode();
    CPB2.setOutputMode();
    CPB3.setOutputMode();
    CPB4.setOutputMode();
    CPB5.setOutputMode();
    CPB6.setOutputMode();
    CPB7.setOutputMode();
    CPD0.setOutputMode();
    CPD1.setOutputMode();
    CPD2.setOutputMode();
    CPD3.setOutputMode();
    CPD4.setOutputMode();
    CPD5.setOutputMode();
    CPD6.setOutputMode();
    CPD7.setOutputMode();
    
    constexpr uintptr_t CTCNT0 = 0x46;
    while (true) {
        auto t = readByte(CTCNT0);
        CPB0.digitalWrite(t & 1);
        CPB1.digitalWrite(t & 2);
        CPB2.digitalWrite(t & 4);
        CPB3.digitalWrite(t & 8);
        CPB4.digitalWrite(t & 16);
        CPB5.digitalWrite(t & 32);
        CPB6.digitalWrite(t & 64);
        CPB7.digitalWrite(t & 128);
        auto t2 = millis();
        CPD0.digitalWrite(t2 & 1);
        CPD1.digitalWrite(t2 & 2);
        CPD2.digitalWrite(t2 & 4);
        CPD3.digitalWrite(t2 & 8);
        CPD4.digitalWrite(t2 & 16);
        CPD5.digitalWrite(t2 & 32);
        CPD6.digitalWrite(t2 & 64);
        CPD7.digitalWrite(t2 & 128);
    }
}

void realmode() {
    using namespace hal;
    //setupTimer();
    volatile double a = 100000.0;
    CPD6.setOutputMode();
    bool val = false;
    while (true) {
        val = !val;
        // 10000 - nothing
        // 100000 - fast blinking
        for (uint32_t i = 0; i < 2000000; i++) {
            CPD6.digitalWrite(val);
            a = a / 2.005;
        }
    }
}

int main() {
    realmode();
}
