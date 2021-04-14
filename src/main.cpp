#include "pins.hpp"

int main() {
    PB0.setOutputMode();
    for (int i = 0; i < 3; i++) {
        PB0.digitalWrite(true);
        PB0.digitalWrite(false);
    }
}
