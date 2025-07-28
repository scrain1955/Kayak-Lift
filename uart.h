#include "defines.h"

#ifndef UART_H
#define	UART_H
#ifdef EnableUART

#define NEWLINE 1
#define NONEWLINE 0
#define EOM_CHAR 0x0D
#ifndef UARTRXBUFFERSIZE
#define UARTRXBUFFERSIZE 80
#endif
#ifndef UARTTXBUFFERSIZE
#define UARTTXBUFFERSIZE 40
#endif

// Function Prototypes
void UARTiniialize(void);
uint8_t WriteTXBuffer(char * str, int8_t newline);
uint8_t WriteTXBufferConst(const char * str, int8_t newline);
uint8_t TxBusy(void);

#endif
#endif	/* UART_H */

