#include "defines.h"
#include <xc.h>
#ifdef EnableUART
#include "uart.h"
#include "strings.h"

volatile char TXdatabuf[UARTTXBUFFERSIZE]; // UART Transmitted data buffer
volatile uint8_t TXdataPtr; // UART Transmitted data buffer pointer
volatile uint8_t TXdataLen; // UART Transmitted data buffer length
extern volatile unsigned int cumulativeMilliSecs;



//***********************************************************************
//          Initialize UART for the PIC16F1823
//***********************************************************************

void UARTiniialize(void) {
    SYNC = 0; // Async mode
    BRGH = 1; // High speed
    BRG16 = 1; // Use 16 bit baud rate generator
    SPBRGH = 0;
    SPBRGL = 68; // Set Baud Rate to 115,200
    //CREN = 1; // Enable UART Receiver
    CREN = 0; // Enable UART Receiver
    SPEN = 1; // Enable the UART
    //RCIE = 1; // Enable UART Receive Interrupt enable
    RCIE = 0; // Enable UART Receive Interrupt enable
    TXEN = 1; // Enable UART Transmitter
    TXdataPtr = 0;
    TXdataLen = 0;
}
//***********************************************************************
// 			Write Data String to TX buffer
//***********************************************************************

uint8_t WriteTXBuffer(char *str, int8_t newline) {
    char i;
    unsigned int startCnt = cumulativeMilliSecs;

    while (TXdataLen != 0 && ((cumulativeMilliSecs - startCnt) < 250)); // Loop or timeout after 250 msecs
    if (TXdataLen != 0) return (FALSE); // don't do the write if the buffer still has data
    for (i = 0; str[i] != '\0' && i < (UARTTXBUFFERSIZE - 2); i++) { // copy string to be transmitted to internal buffer
        TXdatabuf[i] = str[i];
    }
    if (newline) {
        TXdatabuf[i++] = 0x0D; // add newline character
        TXdatabuf[i++] = 0x0A; // add linefeed
    }
    TXdataLen = i;
    TXdataPtr = 0;
    TXIE = 1; // enable uart transmitter
    return (TRUE);
}

//***********************************************************************
// 			Write Const Data String to TX buffer
//***********************************************************************

uint8_t WriteTXBufferConst(const char * str, int8_t newline) {
    char i;
    unsigned int startCnt = cumulativeMilliSecs;

    while (TXdataLen != 0 && ((cumulativeMilliSecs - startCnt) < 250)); // Loop or timeout after 250 msecs
    if (TXdataLen != 0) return (FALSE); // don't do the write if the buffer still has data
    for (i = 0; str[i] != '\0' && i < (UARTTXBUFFERSIZE - 2); i++) {
        TXdatabuf[i] = str[i];
    }
    if (newline) {
    TXdatabuf[i++] = 0x0D; // add newline character
    TXdatabuf[i++] = 0x0A; // add linefeed
    }
    TXdataLen = i;
    TXdataPtr = 0;
    TXIE = 1; // enable uart transmitter
    return (TRUE);
}
//***********************************************************************
// 			Check to see if UART is busy sending a message
// Returns TRUE if busy, false if not.
//***********************************************************************

uint8_t TxBusy(void) {

    if (TXdataLen == 0) return (FALSE);
    return (TRUE);
}

#endif