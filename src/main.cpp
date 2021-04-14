#include "pins.hpp"

int main() {
    pinMode(PB0, OUTPUT);
    pinMode(PC1, OUTPUT);
    pinMode(PD2, OUTPUT);
    for (int i = 0; i < 3; i++) {
        digitalWrite(PB0, HIGH);
        digitalWrite(PC1, HIGH);
        digitalWrite(PD2, HIGH);
        digitalWrite(PB0, LOW);
        digitalWrite(PC1, LOW);
        digitalWrite(PD2, LOW);
    }
}
