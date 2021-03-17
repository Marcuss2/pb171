namespace volatiles {
    volatile unsigned char *const PORTD = reinterpret_cast<volatile unsigned char *>(0x2B);
    volatile unsigned char *const DDRD = reinterpret_cast<volatile unsigned char *>(0x2A);
    volatile unsigned char *const PIND = reinterpret_cast<volatile unsigned char *>(0x29);
}

int main() {
    *volatiles::DDRD = 0b00000100;
    *volatiles::DDRD = 0b00000100; // To make sure enough time has passed
    *volatiles::PORTD = 0b00000100;
}
