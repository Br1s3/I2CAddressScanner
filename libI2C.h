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

// void START_condition_transmitted();
int TWI_start();
// int START_status_register();
// void SLA_W_condition_transmitted(char SLA_W);
// char SLA_W_status_register();
// void DATA_condition_transmitted(char DATA);
void TWI_write(uint8_t DATA);
// char DATA_status_register();
uint8_t TWI_read();
inline void TWI_stop();
void ERROR();


# ifdef LIBI2C_IMPLEMENTATION


void ERROR()
{
    TWI_stop();
}

int TWI_start()
{
    // 1. Send START condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    // 2. Wait for TWINT flag set. This indicates that the START condition has been transmitted
    while (!(TWCR & (1<<TWINT))) ;

    // 3. Check value of TWI status register. Mask prescaler bits. If status different from START go to ERROR
    if ((TWSR & NO_RELEVANT_STATE) != START) {
        ERROR();
	return -1;
    }
    return 0;
}
/*
// 3. Check value of TWI status register. Mask prescaler bits. If status different from START go to ERROR
int START_status_register()
{
    int res = TWSR & NO_RELEVANT_STATE;
    if (res != START)
        ERROR(res);
    return res;
}
*/

// Redondant because it's the seem with the data 
// void SLA_W_condition_transmitted(char SLA_W)
// {
//     // 3. Load SLA_W into TWDR register. Clear TWINT bit in TWCR to start transmission of address
//     TWDR = SLA_W;
//     TWCR = (1 << TWINT) | (1 << TWEN);
//     // 4. Wait for TWINT flag set. This indicates that the SLA+W has been transmitted, and ACK/NACK has been received.
//     while (!(TWCR & (1<<TWINT))) ;
// }

// // 5. Check value of TWI status register. Mask prescaler bits. If status different from MT_SLA_ACK go to ERROR
// char SLA_W_status_register()
// {
//     char res = TWSR & NO_RELEVANT_STATE;
//     if (res != MT_SLA_ACK)
//         ERROR(res);
//     return res;
// }

void TWI_write(uint8_t DATA)
{
    // 5. Load DATA into TWDR register. clear TWINT bit in TWCR to start transmission of data
    TWDR = DATA;
    TWCR = (1 << TWINT) | (1 << TWEN);
    // 6. Wait for TWINT flag set. This indicates that the DATA has been transmitted, and ACK/NACK has been received.
    while (!(TWCR & (1<<TWINT))) ;
}

// 7. Check value of TWI status register. Mask prescaler bits. If status different from MT_DATA_ACK go to ERROR
uint8_t TWI_read()
{
    uint8_t res = TWSR & NO_RELEVANT_STATE;
    if (res != MT_DATA_ACK)
        ERROR(res);
    return res;
}

// 7. Transmit STOP condition
inline void TWI_stop()
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

# endif // LIBI2C_IMPLEMENTATION
#endif // LIBI2C_H_INCLUDED
