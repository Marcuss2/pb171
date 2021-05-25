#include "pins.hpp"
#include "timers.hpp"
#include "uart.hpp"

void tracemode() {
    using namespace hal;
    Serial.begin();
    Serial.write("Hello world!\r\n");
    Serial.println(634);
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
    tracemode();
    
    volatile bool a = true;
    while (true) {
        a = !a;
    }
}
