#ifndef LIBTWI_H_INCLUDED
#define LIBTWI_H_INCLUDED

#include <util/twi.h>
#include <stdint.h>
#include <stdbool.h>

#define TWI_REG_START    do {TWCR = (_BV(TWINT) | _BV(TWSTA) | _BV(TWEN));} while(0)
#define TWI_REG_SLA_RW   do {TWCR = (_BV(TWINT) | _BV(TWEN));} while(0)
#define TWI_REG_WRITE    do {TWCR = (_BV(TWINT) | _BV(TWEN));} while(0)
#define TWI_REG_READACK  do {TWCR = (_BV(TWINT) | _BV(TWEA) | _BV(TWEN));} while(0)
#define TWI_REG_READNACK do {TWCR = (_BV(TWINT) | _BV(TWEN));} while(0)
#define TWI_REG_STOP     do {TWCR = (_BV(TWINT) | _BV(TWSTO) | _BV(TWEN));} while(0)
#define TWI_REG_RESTART  do {TWCR = (_BV(TWINT) | _BV(TWSTA) | _BV(TWEN));} while(0)

#define ACK  1
#define NACK 0

int TWI_start();
int TWI_sla_w(uint8_t sla);
int TWI_data_w(uint8_t data);
int TWI_sla_r(uint8_t sla);
int TWI_data_r(bool ack, uint8_t *data);
static inline void TWI_stop();

# ifdef TWI_DEBUG

#define twierror(name) TWI_NAME[(name) >> 3]

static char *TWI_NAME[] = {
    [TW_BUS_ERROR]                = "TW_BUS_ERROR",
    [TW_START>>3]                 = "TW_START",
    [TW_REP_START>>3]             = "TW_REP_START",
    [TW_MT_SLA_ACK>>3]            = "TW_MT_SLA_ACK",
    [TW_MT_SLA_NACK>>3]           = "TW_MT_SLA_NACK",
    [TW_MT_DATA_ACK>>3]           = "TW_MT_DATA_ACK",
    [TW_MT_DATA_NACK>>3]          = "TW_MT_DATA_NACK",
    [TW_MT_ARB_LOST>>3]           = "TW_MT_ARB_LOST",
    [TW_MR_SLA_ACK>>3]            = "TW_MR_SLA_ACK",
    [TW_MR_SLA_NACK>>3]           = "TW_MR_SLA_NACK",
    [TW_MR_DATA_ACK>>3]           = "TW_MR_DATA_ACK",
    [TW_MR_DATA_NACK>>3]          = "TW_MR_DATA_NACK",
    [TW_SR_SLA_ACK>>3]            = "TW_SR_SLA_ACK",
    [TW_SR_ARB_LOST_SLA_ACK>>3]   = "TW_SR_ARB_LOST_SLA_ACK",
    [TW_SR_GCALL_ACK>>3]          = "TW_SR_GCALL_ACK",
    [TW_SR_ARB_LOST_GCALL_ACK>>3] = "TW_SR_ARB_LOST_GCALL_ACK",
    [TW_SR_DATA_ACK>>3]           = "TW_SR_DATA_ACK",
    [TW_SR_DATA_NACK>>3]          = "TW_SR_DATA_NACK",
    [TW_SR_GCALL_DATA_ACK>>3]     = "TW_SR_GCALL_DATA_ACK",
    [TW_SR_GCALL_DATA_NACK>>3]    = "TW_SR_GCALL_DATA_NACK",
    [TW_SR_STOP>>3]               = "TW_SR_STOP",
    [TW_ST_SLA_ACK>>3]            = "TW_ST_SLA_ACK",
    [TW_ST_ARB_LOST_SLA_ACK>>3]   = "TW_ST_ARB_LOST_SLA_ACK",
    [TW_ST_DATA_ACK>>3]           = "TW_ST_DATA_ACK",
    [TW_ST_DATA_NACK>>3]          = "TW_ST_DATA_NACK",
    [TW_ST_LAST_DATA>>3]          = "TW_ST_LAST_DATA",
    [TW_NO_INFO>>3]               = "TW_NO_INFO"
};

# endif // DEBUG

# ifdef LIBTWI_IMPLEMENTATION

int TWI_start()
{
    // 1. Send START condition
    TWI_REG_START;

    // 2. Wait for TWINT flag set. This indicates that the START condition has been transmitted
    loop_until_bit_is_set(TWCR, TWINT);

    // 3. Check value of TWI status register. Mask prescaler bits. If status different from START
    if ((TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START)) {
	TWI_REG_STOP;
	return TW_STATUS;
    }

    return 0;
}

int TWI_sla_w(uint8_t sla)
{
    // 3. Load SLA_W into TWDR register. Clear TWINT bit in TWCR to start transmission of address
    TWDR = (sla << 1) | 0;
    TWI_REG_SLA_RW;

    // 4. Wait for TWINT flag set. This indicates that the SLA+W has been transmitted, and ACK/NACK has been received.
    loop_until_bit_is_set(TWCR, TWINT);

    // 5. Check value of TWI status register. Mask prescaler bits. If status different from MT_SLA_ACK...
    if (TW_STATUS == TW_MT_SLA_NACK) {
	TWI_REG_STOP;
	return TW_STATUS;
    }

    // (INFO. TW_MT_ARB_LOST is not test because if we quit the funtion with something different than 0 the goal is to not send a TWI_REG_STOP)

    return 0;
}

int TWI_data_w(uint8_t data)
{
    // 5. Load DATA into TWDR register. clear TWINT bit in TWCR to start transmission of data
    TWDR = data;
    TWI_REG_WRITE;

    // 6. Wait for TWINT flag set. This indicates that the DATA has been transmitted, and ACK/NACK has been received.
    loop_until_bit_is_set(TWCR, TWINT);

    // 7. Check value of TWI status register. Mask prescaler bits. If status different from MT_DATA_ACK...
    if (TW_STATUS == TW_MT_DATA_NACK) {
	TWI_REG_STOP;
	return TW_STATUS;
    }

    return 0;
}

int TWI_sla_r(uint8_t sla)
{
    // 3. Load SLA_R into TWDR register. Clear TWINT bit in TWCR to start transmission of address
    TWDR = (sla << 1) | 1;
    TWI_REG_SLA_RW;

    // 4. Wait for TWINT flag set. This indicates that the SLA+W has been transmitted, and ACK/NACK has been received.
    loop_until_bit_is_set(TWCR, TWINT);

    // 5. Check value of TWI status register. Mask prescaler bits. If status different from MT_SLA_ACK go to TWI_stop
    if (TW_STATUS == TW_MR_SLA_NACK) {
	TWI_REG_STOP;
	return TW_STATUS;
    }

    // (INFO. TW_MR_ARB_LOST is not test because if we quit the funtion with something different than 0 the goal is to not send a TWI_REG_STOP)

    return 0;
}

int TWI_data_r(bool ack, uint8_t *data)
{
    if (ack == ACK) TWI_REG_READACK;
    else            TWI_REG_READNACK;

    // 6. Wait for TWINT flag set. This indicates that the DATA has been received, and ACK/NACK has been received.
    loop_until_bit_is_set(TWCR, TWINT);

    // 7. Check value of TWI status register. Mask prescaler bits. If status different from MR_DATA_ACK go to TWI_stop
    if (((ack == ACK) && (TW_STATUS != TW_MR_DATA_ACK)) ||
        ((ack == NACK) && (TW_STATUS != TW_MR_DATA_NACK))) {
	TWI_REG_STOP;
	return TW_STATUS;
    }

    // 5. Read DATA into TWDR register. clear TWINT bit in TWCR to start receiving of data
    *data = TWDR;

    return 0;
}

static inline void TWI_stop()
{
    TWI_REG_STOP;
}

# endif // LIBTWI_IMPLEMENTATION
#endif // LIBTWI_H_INCLUDED


/*
 * ## References:
 * /usr/lib/avr/include/util/twi.h
 * /usr/lib/avr/include/avr/sfr_defs.h
 */
