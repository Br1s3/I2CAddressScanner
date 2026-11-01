#ifndef PRINTF_H_INCLUDED
#define PRINTF_H_INCLUDED

#include <stdio.h> // Used for: FILE, stdout

/*
165ATmega328P [DATASHEET]
Table 19-12. Examples of UBRRn Settings for Commonly Used Oscillator Frequencies
|_________________________________________________|
|                 |      f osc = 16.0000MHz       |
|                 |    U2Xn = 0   |    U2Xn = 1   |
| Baud Rate (bps) | UBRRn | Error | UBRRn | Error |
|-----------------|-------|-------|-------|-------|
|       2400      |  416  | –0.1% |  832  |  0.0% |
|       4800      |  207  |  0.2% |  416  | –0.1% |
|       9600      |  103  |  0.2% |  207  |  0.2% |
|       14.4k     |   68  |  0.6% |  138  | –0.1% |
|       19.2k     |   51  |  0.2% |  103  |  0.2% |
|       28.8k     |   34  | –0.8% |   68  |  0.6% |
|       38.4k     |   25  |  0.2% |   51  |  0.2% |
|       57.6k     |   16  |  2.1% |   34  | –0.8% |
|       76.8k     |   12  |  0.2% |   25  |  0.2% |
|      115.2k     |    8  | –3.5% |   16  |  2.1% |
|      230.4k     |    3  |  8.5% |    8  | –3.5% |
|        250k     |    3  |  0.0% |    7  |  0.0% |
|        0.5M     |    1  |  0.0% |    3  |  0.0% |
|          1M     |    0  |  0.0% |    1  |  0.0% |
|      Max. (1)   |     1Mbps     |     2Mbps     |
|_________________________________________________|
Note: 1. UBRRn = 0, error = 0.0%
*/

void USART_Init(unsigned int baudrate);
static int USART_Transmit(char data, FILE *stream);

# ifdef PRINTF_IMPLEMENTATION

#include <avr/io.h>


void USART_Init(unsigned int baudrate)
{
    /* Apply the baudrate to UBRR0 register */
    UBRR0H = (unsigned char) (baudrate>>8);
    UBRR0L = (unsigned char) baudrate;
    
    /* Reset the A control register */
    // UCSR0A = 0x00 ;
    /* U2X0: double speed mode */
    UCSR0A = _BV(U2X0) ;

    /* Activate the UART receiver and transmitter */
    UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
    
    // /* Formats the frame: 8 bytes with 2 stop bits */
    // UCSR0C = (1 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00);

    /* Formats the frame: 8 bytes with 1 stop bits */
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);

    /* Redirect the flow UDR0 in the stdout */
    static FILE mystdout = FDEV_SETUP_STREAM(USART_Transmit, NULL, _FDEV_SETUP_WRITE);
    stdout = &mystdout;
}

/* Transmit data in UDR0 register */
static int USART_Transmit(char data, FILE *stream)
{
    if (data == '\n') {
	/* Wait for empty transmit buffer */
	loop_until_bit_is_set(UCSR0A, UDRE0);
	/* Start transmission */
	UDR0 = '\r';
    }

    /* Wait for empty transmit buffer */
    loop_until_bit_is_set(UCSR0A, UDRE0);

    /* Start transmission */
    UDR0 = data;
    return 0;
}


# endif // PRINTF_IMPLEMENTATION
#endif // PRINT_H_INCLUDED
