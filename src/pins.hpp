#pragma once
#include "concepts.hpp"

using uint8_t = unsigned char;
using uint16_t = unsigned int;
using uint32_t = unsigned long int;
using uintptr_t = uint16_t;
using vol_ptr = volatile uint8_t*;

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

class InternalVariable {
    const uintptr_t ADDR;
    const uint8_t POS;

   public:
    constexpr InternalVariable(uintptr_t addr, uint8_t pos) : ADDR(addr), POS(pos) {}

    bool read() const { return readBit(ADDR, POS); }

    void set(bool val) const { setBit(ADDR, val, POS); }
};

constexpr uintptr_t MCUCR = 0x55;

constexpr InternalVariable PUD = InternalVariable(MCUCR, 4);

inline void NOP() {
    __asm__ __volatile__("nop");
}

class DigitalPin {
    const uintptr_t PIN_MODE_ADDR;
    const uintptr_t PIN_OUTPUT_ADDR;
    const uintptr_t PIN_INPUT_ADDR;
    const uint8_t PIN_POS;
    
    enum PinMode {
        InputMode = 0,
        OutputMode = 1,
    };
    
    bool _isPullUpEnabled() const { return !PUD.read(); }

   public:
    constexpr DigitalPin(uintptr_t mode_addr, uintptr_t output_addr, uintptr_t input_addr, unsigned char pin_pos)
        : PIN_MODE_ADDR(mode_addr), PIN_OUTPUT_ADDR(output_addr), PIN_INPUT_ADDR(input_addr), PIN_POS(pin_pos) {
        static_assert(io_digital_pin<DigitalPin>);
    }

    inline void digitalWrite(bool val) const { setBit(PIN_OUTPUT_ADDR, val, PIN_POS); }

    inline bool digitalRead() const { return readBit(PIN_INPUT_ADDR, PIN_POS); }

    inline void setInputMode() const {
        if (!_isPullUpEnabled()) {
            digitalWrite(false);
            NOP();
        }
        setBit(PIN_MODE_ADDR, PinMode::InputMode, PIN_POS);
        NOP();
    }
    
    inline void setOutputMode() const {
        if (!_isPullUpEnabled()) {
            digitalWrite(false);
            NOP();
        }
        setBit(PIN_MODE_ADDR, PinMode::OutputMode, PIN_POS);
        NOP();
    }
};

constexpr uintptr_t PORTB = 0x25;
constexpr uintptr_t DDRB = 0x24;
constexpr uintptr_t PINB = 0x23;
constexpr auto PB0 = DigitalPin(DDRB, PORTB, PINB, 0);
constexpr auto PB1 = DigitalPin(DDRB, PORTB, PINB, 1);
constexpr auto PB2 = DigitalPin(DDRB, PORTB, PINB, 2);
constexpr auto PB3 = DigitalPin(DDRB, PORTB, PINB, 3);
constexpr auto PB4 = DigitalPin(DDRB, PORTB, PINB, 4);
constexpr auto PB5 = DigitalPin(DDRB, PORTB, PINB, 5);
constexpr auto PB6 = DigitalPin(DDRB, PORTB, PINB, 6);
constexpr auto PB7 = DigitalPin(DDRB, PORTB, PINB, 7);

constexpr uintptr_t PORTC = 0x28;
constexpr uintptr_t DDRC = 0x27;
constexpr uintptr_t PINC = 0x26;
constexpr auto PC0 = DigitalPin(DDRC, PORTC, PINC, 0);
constexpr auto PC1 = DigitalPin(DDRC, PORTC, PINC, 1);
constexpr auto PC2 = DigitalPin(DDRC, PORTC, PINC, 2);
constexpr auto PC3 = DigitalPin(DDRC, PORTC, PINC, 3);
constexpr auto PC4 = DigitalPin(DDRC, PORTC, PINC, 4);
constexpr auto PC5 = DigitalPin(DDRC, PORTC, PINC, 5);
constexpr auto PC6 = DigitalPin(DDRC, PORTC, PINC, 6);

constexpr uintptr_t PORTD = 0x2B;
constexpr uintptr_t DDRD = 0x2A;
constexpr uintptr_t PIND = 0x29;
constexpr auto PD0 = DigitalPin(DDRD, PORTD, PIND, 0);
constexpr auto PD1 = DigitalPin(DDRD, PORTD, PIND, 1);
constexpr auto PD2 = DigitalPin(DDRD, PORTD, PIND, 2);
constexpr auto PD3 = DigitalPin(DDRD, PORTD, PIND, 3);
constexpr auto PD4 = DigitalPin(DDRD, PORTD, PIND, 4);
constexpr auto PD5 = DigitalPin(DDRD, PORTD, PIND, 5);
constexpr auto PD6 = DigitalPin(DDRD, PORTD, PIND, 6);
constexpr auto PD7 = DigitalPin(DDRD, PORTD, PIND, 7);
