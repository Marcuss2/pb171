#pragma once

#include <stdint.h>

#include "bitops.hpp"
#include "concepts.hpp"
#include "types.hpp"

namespace hal {

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

inline void nop() { __asm__ __volatile__("nop"); }

enum {
    INPUT = 0,
    OUTPUT = 1,
};

class DigitalPin {
    const uintptr_t PIN_MODE_ADDR;
    const uintptr_t PIN_OUTPUT_ADDR;
    const uintptr_t PIN_INPUT_ADDR;
    const uint8_t PIN_POS;

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
            nop();
        }
        setBit(PIN_MODE_ADDR, INPUT, PIN_POS);
        nop();
    }

    inline void setOutputMode() const {
        if (!_isPullUpEnabled()) {
            digitalWrite(false);
            nop();
        }
        setBit(PIN_MODE_ADDR, OUTPUT, PIN_POS);
        nop();
    }
};

void pinMode(const io_digital_pin auto& pin, bool mode) {
    if (mode == INPUT) {
        pin.setInputMode();
    } else {
        pin.setOutputMode();
    }
}

enum {
    LOW = 0,
    HIGH = 1,
};

void digitalWrite(const digital_writeable auto& pin, bool val) { pin.digitalWrite(val); }

bool digitalRead(const digital_readable auto& pin) { return pin.digitalRead(); }

class PWMPin : public DigitalPin {
    const uintptr_t COMPARE_REG_ADDR;
    const uintptr_t COUNTER_CTRL_ADDR;
    const uintptr_t PRESCALE_ADDR;

   public:
    constexpr PWMPin(uintptr_t mode_addr, uintptr_t output_addr, uintptr_t input_addr, unsigned char pin_pos,
                     uintptr_t compare_reg, uintptr_t ctrl_addr, uintptr_t prescale_addr)
        : DigitalPin(mode_addr, output_addr, input_addr, pin_pos),
          COMPARE_REG_ADDR(compare_reg),
          COUNTER_CTRL_ADDR(ctrl_addr),
          PRESCALE_ADDR(prescale_addr) {
              static_assert(pwm_pin<PWMPin>);
        }
        
        void setupPWM() const {
            setOutputMode();
            // Setting Fast PWM
            const auto WGMx0 = 0;
            setBit(COUNTER_CTRL_ADDR, true, WGMx0);
            const auto WGMx1 = 1;
            setBit(COUNTER_CTRL_ADDR, true, WGMx1);
            // Non-inverting mode
            const auto COMxA1 = 7;
            setBit(COUNTER_CTRL_ADDR, true, COMxA1);
            // Set the prescaler from disabled to Clock / 64
            const auto CSx0 = 0;
            setBit(PRESCALE_ADDR, true, CSx0);
            const auto CSx1 = 1;
            setBit(PRESCALE_ADDR, true, CSx1);
        }
        
        void setPWM(uint8_t val) const {
            setByte(COMPARE_REG_ADDR, val);
        }
};

template<uint8_t PRESCALE = 0b100>
class AnalogPin {
    const uint8_t MASK;
    
    const uintptr_t ADMUX = 0x7C;
    const uint8_t REFS0 = 6;
    const uintptr_t ADCSRA = 0x7A;
    
public:
    constexpr AnalogPin(uint8_t mask) : MASK(mask) {
        static_assert(analog_readable<AnalogPin>);
    }
    
    void setupAnalogRead() const {
        
        setBit(ADMUX, true, REFS0);
        
        const uint8_t ADEN = 7;
        const uint8_t prescale = PRESCALE | (1 << ADEN);
        setByte(ADCSRA, prescale);
    }
    
    uint16_t analogRead() const {
        const uint8_t mask = MASK | (1 << REFS0);
        setByte(ADMUX, mask);
        const uint8_t ADSC = 6;
        setBit(ADCSRA, true, ADSC);
        while (readBit(ADCSRA, ADSC));
        const uintptr_t ADCSHORT = 0x78;
        return readShort(ADCSHORT);
    }
};

constexpr uintptr_t PORTB = 0x25;
constexpr uintptr_t DDRB = 0x24;
constexpr uintptr_t PINB = 0x23;
constexpr auto CPB0 = DigitalPin(DDRB, PORTB, PINB, 0);
constexpr auto CPB1 = DigitalPin(DDRB, PORTB, PINB, 1);
constexpr auto CPB2 = DigitalPin(DDRB, PORTB, PINB, 2);
constexpr uintptr_t OCR2A = 0xB3;
constexpr uintptr_t TCCR2A = 0xB0;
constexpr uintptr_t TCCR2B = 0xB1;
constexpr auto CPB3 = PWMPin(DDRB, PORTB, PINB, 3, OCR2A, TCCR2A, TCCR2B);
constexpr auto CPB4 = DigitalPin(DDRB, PORTB, PINB, 4);
constexpr auto CPB5 = DigitalPin(DDRB, PORTB, PINB, 5);
constexpr auto CPB6 = DigitalPin(DDRB, PORTB, PINB, 6);
constexpr auto CPB7 = DigitalPin(DDRB, PORTB, PINB, 7);

constexpr uintptr_t PORTC = 0x28;
constexpr uintptr_t DDRC = 0x27;
constexpr uintptr_t PINC = 0x26;
constexpr auto CPC0 = DigitalPin(DDRC, PORTC, PINC, 0);
constexpr auto CPC1 = DigitalPin(DDRC, PORTC, PINC, 1);
constexpr auto CPC2 = DigitalPin(DDRC, PORTC, PINC, 2);
constexpr auto CPC3 = DigitalPin(DDRC, PORTC, PINC, 3);
constexpr auto CPC4 = DigitalPin(DDRC, PORTC, PINC, 4);
constexpr auto CPC5 = DigitalPin(DDRC, PORTC, PINC, 5);
constexpr auto CPC6 = DigitalPin(DDRC, PORTC, PINC, 6);

constexpr uintptr_t PORTD = 0x2B;
constexpr uintptr_t DDRD = 0x2A;
constexpr uintptr_t PIND = 0x29;
constexpr auto CPD0 = DigitalPin(DDRD, PORTD, PIND, 0);
constexpr auto CPD1 = DigitalPin(DDRD, PORTD, PIND, 1);
constexpr auto CPD2 = DigitalPin(DDRD, PORTD, PIND, 2);
constexpr uintptr_t OCR2B = 0xB4;
constexpr auto CPD3 = PWMPin(DDRD, PORTD, PIND, 3, OCR2B, TCCR2A, TCCR2B);
constexpr auto CPD4 = DigitalPin(DDRD, PORTD, PIND, 4);
constexpr uintptr_t OCR0B = 0x48;
constexpr uintptr_t TCCR0A = 0x44;
constexpr uintptr_t TCCR0B = 0x45;
constexpr auto CPD5 = PWMPin(DDRD, PORTD, PIND, 5, OCR0B, TCCR0A, TCCR0B);
constexpr uintptr_t OCR0A = 0x47;
constexpr auto CPD6 = PWMPin(DDRD, PORTD, PIND, 6, OCR0A, TCCR0A, TCCR0B);
constexpr auto CPD7 = DigitalPin(DDRD, PORTD, PIND, 7);

constexpr auto CADC0 = AnalogPin(0b0000);
constexpr auto CADC1 = AnalogPin(0b0001);
constexpr auto CADC2 = AnalogPin(0b0010);
constexpr auto CADC3 = AnalogPin(0b0011);
constexpr auto CADC4 = AnalogPin(0b0100);
constexpr auto CADC5 = AnalogPin(0b0101);
constexpr auto CADC6 = AnalogPin(0b0110);
constexpr auto CADC7 = AnalogPin(0b0111);
constexpr auto CADC8 = AnalogPin(0b1000);
constexpr auto CVBG  = AnalogPin(0b1110);
constexpr auto CVGND = AnalogPin(0b1111);

}
