#include "pins.hpp"

int main() {
    
    PD6.setupPWM();
    
    for (int i = 0; i < 0x7FFE; i++) {
        for (int j = 0; j < 224; j++) {
            PD6.setPWM(j);
            for (int k = 0; k < 0x7FFE; k++) {
                nop();
            }
        }
    }
    PD6.setPWM(0);
    while (true) {
        
    }
}
