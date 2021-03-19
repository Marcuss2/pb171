#pragma once
#include "concepts.hpp"

using vol_ptr = volatile unsigned char* const;

class DigitalPin {
    vol_ptr PIN_MODE_ADDR;
    vol_ptr PIN_OUTPUT_ADDR;
    vol_ptr PIN_INPUT_ADDR;
    const unsigned char PIN_POS;

    void _setBit(vol_ptr addr, bool val) const {
        unsigned char temp = *addr;
        temp ^= (-val & temp) & (val << PIN_POS);
        *addr = temp;
    }

   public:
    constexpr DigitalPin(vol_ptr mode_addr, vol_ptr output_addr, vol_ptr input_addr, unsigned char pin_pos)
        : PIN_MODE_ADDR(mode_addr), PIN_OUTPUT_ADDR(output_addr), PIN_INPUT_ADDR(input_addr), PIN_POS(pin_pos) {
        static_assert(DigitalWriteable<DigitalPin>);
    }

    inline void digitalWrite(bool val) const { _setBit(PIN_OUTPUT_ADDR, val); }

    // inline bool digitalRead(bool val) const { return _readBit(PIN_INPUT_ADDR); }
};
