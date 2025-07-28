#include <xc.h>
#include "defines.h"

extern volatile char milliSecondTicFlag;
extern volatile int milliSecondTics;
extern volatile unsigned int MotorDelayTimer;
extern volatile unsigned int cumulativeMilliSecs;
#ifdef EnableUART
#include "uart.h"
extern volatile char TXdatabuf[UARTTXBUFFERSIZE]; // UART Transmitted data buffer
extern volatile uint8_t TXdataPtr; // UART Transmitted data buffer pointer
extern volatile uint8_t TXdataLen; // UART Transmitted data buffer length
#endif

//
// Interrupt Routine
//

void __interrupt() isr(void) {
    //***********************************************************************
    //          Timer2 Interrupt
    // Occurs every millisecond
    //***********************************************************************
    if (TMR2IF && TMR2IE) {
        TMR2IF = 0;
        cumulativeMilliSecs++;
        milliSecondTics++;
        if (MotorDelayTimer != 0) MotorDelayTimer--;
        milliSecondTicFlag = TRUE;
    }
    //***********************************************************************
    //          IOC Interrupt
    // Occurs when Interupt on Change pins have detected an edge.
    // Nothing is done here, its just the result of IOC waking the device
    // from sleep.
    //***********************************************************************
    if (IOCIF && IOCIE) {
        IOCAF = 0; // clear all the IOC bits
    }
#ifdef EnableUART
    // ***********************************************************************
    //			 UART Transmit interrupt
    // TXIF is set whenever TX buffer is empty so handle this interrupt
    // by setting/clearing TXIE.
    // This routine will constantly be called as long as the uart transmitter 
    // buffer is empty and the TXIE bit is set. When the message buffer has sent
    // all data then the TXIE will be turned off.
    // ***Important*** re-enable TXIE when the buffer has data to write.
    // ***********************************************************************
    if ((TXIE) && (TXIF)) { // TXIF Flag will be set whenever the buffer is empty and ready for data
        TXREG = TXdatabuf[TXdataPtr++]; // Write to UART transmitter
        if (TXdataPtr == TXdataLen) { // transmission is done
            TXdataPtr = 0; // reset the pointers
            TXdataLen = 0;
            TXIE = 0; // Disable Interrupt so it won't needlessly be called with empty buffer. 
            // NOTE- re-enable when buffer is filled
        }
    }
#endif
}