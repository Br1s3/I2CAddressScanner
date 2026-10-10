#define F_CPU 16000000
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "print.h"

#define LIBI2C_IMPLEMENTATION
#include "libI2C.h"

int Research_ADDR(uint8_t sla_addr)
{
    int ret = 0;
    // START condition
    TWI_start();
    
    // SLA+W
    TWI_write((sla_addr << 1) | 0);
    
    if (TWI_read() == MT_SLA_NACK) {
	ret = -1;
    }
    TWI_stop();
    return ret; // TH real addr : 0x3c
}

void TWI_Init()
{
    // SCL_freq = CPU_clk_freq / (16 + 2*(TWBR) * PrescalerValue)
    // PrescalerValue = 1 => TWSR = 0
    // TWBR = 12; // 400kHz
    TWBR = 72; // 100kHz
}


int main()
{
    USART_Init(0);
    TWI_Init();
    DDRB = (1 << DDB5);
    
    uint8_t addr = 0x00;
    int addr_found = -1;
    while (1) {
	while (addr_found != 0) {
	    if (Research_ADDR(addr) == 0) {
		addr_found = 0;
		printf("PASS avec: 0x%02x\n", addr);
	    } else {
		printf("NACK Address: 0x%02x\n", addr);
		addr++;
	    }
	    _delay_ms(50);
	}

	PORTB ^= (1 << PORTB5);
	_delay_ms(100);
    }
}
