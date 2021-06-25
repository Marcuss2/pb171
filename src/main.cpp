#include "pins.hpp"
#include "timers.hpp"
#include "uart.hpp"
#include "stopwatch.hpp"

void tracemode() {
    using namespace hal;
    auto watch = StopWatch::StopWatch(StopWatch::Resolution::SECONDS);
    watch.start();
    Serial.begin(38400);
    Serial.println("Hello World!");
    while (true) {
        auto mil = millis();
        auto el = watch.elapsed();
        Serial.print(static_cast<long>(mil));
        Serial.write(" ");
        Serial.print(static_cast<long>(el));
        Serial.println("");
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

void setup() {
    using namespace hal;
    // Your setup code here
    
}

void loop() {
    using namespace hal;
    // Your loop code here
}

int main() {
    hal::setupTimer();
    tracemode();
    setup();
    while (true) {
        loop();
    }
}
