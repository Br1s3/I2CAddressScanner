#ifndef LIBI2C_H_INCLUDED
#define LIBI2C_H_INCLUDED

///////////////////////////////////////////////////
/* All this part is done by the arduino hardware */

#define START        0x08
#define RE_START     0x10

#define MT_SLA_ACK   0x18
#define MT_SLA_NACK  0x20
#define MT_DATA_ACK  0x28
#define MT_DATA_NACK 0x30
#define MT_LOST      0x38

#define MR_LOST      0x38
#define MR_SLA_ACK   0x40
#define MR_SLA_NACK  0x48
#define MR_DATA_ACK  0x50
#define MR_DATA_NACK 0x58

#define SR_SLA_ACK   0x60
#define SR_LOST_SLA  0x68
#define SR_CALL_ADDR 0x70
#define SR_LOST_CALL_ADDR 0x78
#define SR_CALL_ADDR_ACK  0x80
#define SR_CALL_ADDR_NACK 0x88
#define SR_GGWP1          0x90
#define SR_GGWP2          0x98
#define SR_STOP           0xA0

#define ST_SLA_ACK       0xA8
#define ST_LOST_SLA_ACK  0xB0
#define ST_DATA_ACK      0xB8
#define ST_DATA_NACK     0xC0
#define ST_LOST_DATA_ACK 0xC8

#define NO_RELEVANT_STATE 0xF8
#define ILLEGAL_START     0x00

///////////////////////////////////////////////////

int TWI_start();
int TWI_SLA_W(uint8_t SLA_W);
int TWI_write(uint8_t DATA);
// uint8_t TWI_read();
inline void TWI_stop();
void ERROR();


# ifdef LIBI2C_IMPLEMENTATION


int TWI_start()
{
    // 1. Send START condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    // 2. Wait for TWINT flag set. This indicates that the START condition has been transmitted
    while (!(TWCR & (1<<TWINT))) ;

    // 3. Check value of TWI status register. Mask prescaler bits. If status different from START go to ERROR
    if ((TWSR & NO_RELEVANT_STATE) != START) {
        TWI_stop();
	return -1;
    }
    return 0;
}

int TWI_SLA_W(uint8_t SLA_W)
{
    // 3. Load SLA_W into TWDR register. Clear TWINT bit in TWCR to start transmission of address
    TWDR = (SLA_W << 1) | 0;
    TWCR = (1 << TWINT) | (1 << TWEN);
    // 4. Wait for TWINT flag set. This indicates that the SLA+W has been transmitted, and ACK/NACK has been received.
    while (!(TWCR & (1<<TWINT))) ;

    // 5. Check value of TWI status register. Mask prescaler bits. If status different from MT_SLA_ACK go to TWI_stop
    if ((TWSR & NO_RELEVANT_STATE) != MT_SLA_ACK) { // TODO: Verify more condition (MR_LOST, SR_LOST_SLA, ...)
        TWI_stop();
	return -1;
    }
    return 0;
}

int TWI_write(uint8_t DATA)
{
    // 5. Load DATA into TWDR register. clear TWINT bit in TWCR to start transmission of data
    TWDR = DATA;
    TWCR = (1 << TWINT) | (1 << TWEN);

    // 6. Wait for TWINT flag set. This indicates that the DATA has been transmitted, and ACK/NACK has been received.
    while (!(TWCR & (1<<TWINT))) ;

    // 7. Check value of TWI status register. Mask prescaler bits. If status different from MT_DATA_ACK go to TWI_stop
    if ((TWSR & NO_RELEVANT_STATE) != MT_DATA_ACK) { // TODO: Verify more condition (MR_LOST, ...)
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

// 7. Transmit STOP condition
inline void TWI_stop()
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

# endif // LIBI2C_IMPLEMENTATION
#endif // LIBI2C_H_INCLUDED
