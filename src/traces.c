#include <avr_mcu_section.h>
    
const struct avr_mmcu_vcd_trace_t _mytrace[]  _MMCU_ = {
	{ AVR_MCU_VCD_SYMBOL("PB"), .what = (void*)0x25, },
	{ AVR_MCU_VCD_SYMBOL("PC"), .what = (void*)0x28, },
	{ AVR_MCU_VCD_SYMBOL("PD"), .what = (void*)0x2B, },
	{ AVR_MCU_VCD_SYMBOL("UDR0"), .what = (void*)0xC6, },
};
