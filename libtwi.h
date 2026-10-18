#ifndef LIBTWI_H_INCLUDED
#define LIBTWI_H_INCLUDED

#include <util/twi.h>

int TWI_start();
int TWI_SLA_W(uint8_t SLA_W);
int TWI_write(uint8_t DATA);
// uint8_t TWI_read();
#ifdef TWI_stop
# undef TWI_stop
#endif


# ifdef LIBTWI_IMPLEMENTATION

// 7. Transmit STOP condition
#define TWI_stop()                       \
do {                                     \
    _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); \
} while(0)

int TWI_start()
{
    // 1. Send START condition
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);

    // 2. Wait for TWINT flag set. This indicates that the START condition has been transmitted
    loop_until_bit_is_set(TWCR, TWINT);

    // 3. Check value of TWI status register. Mask prescaler bits. If status different from START go to ERROR
    if (TW_STATUS != TW_START) {
        TWI_stop();
	return -1;
    }
    return 0;
}

int TWI_SLA_W(uint8_t sla_w)
{
    // 3. Load SLA_W into TWDR register. Clear TWINT bit in TWCR to start transmission of address
    TWDR = (sla_w << 1) | 0;
    TWCR = _BV(TWINT) | _BV(TWEN);
    // 4. Wait for TWINT flag set. This indicates that the SLA+W has been transmitted, and ACK/NACK has been received.
    loop_until_bit_is_set(TWCR, TWINT);

    // 5. Check value of TWI status register. Mask prescaler bits. If status different from MT_SLA_ACK go to TWI_stop
    if (TW_STATUS != TW_MT_SLA_ACK) { // TODO: Verify more condition (MR_LOST, SR_LOST_SLA, ...)
        TWI_stop();
	return -1;
    }
    return 0;
}

int TWI_write(uint8_t data)
{
    // 5. Load DATA into TWDR register. clear TWINT bit in TWCR to start transmission of data
    TWDR = data;
    TWCR = _BV(TWINT) | _BV(TWEN);

    // 6. Wait for TWINT flag set. This indicates that the DATA has been transmitted, and ACK/NACK has been received.
    loop_until_bit_is_set(TWCR, TWINT);

    // 7. Check value of TWI status register. Mask prescaler bits. If status different from MT_DATA_ACK go to TWI_stop
    if (TW_STATUS != TW_MT_DATA_ACK) { // TODO: Verify more condition (MR_LOST, ...)
        TWI_stop();
	return -1;
    }
    return 0;
}

// // 7. Check value of TWI status register. Mask prescaler bits. If status different from MT_DATA_ACK go to TWI_stop
// uint8_t TWI_read()
// {
//     uint8_t res = TWSR & NO_RELEVANT_STATE;
//     if (res != MT_DATA_ACK)
//         TWI_stop(res);
//     return res;
// }


# endif // LIBTWI_IMPLEMENTATION
#endif // LIBTWI_H_INCLUDED
