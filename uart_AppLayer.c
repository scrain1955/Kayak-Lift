//***********************************************************************
//          This is the API level of the UART module
// The functions in here should be used by the application layer
//
//***********************************************************************
#include "defines.h"
#ifdef EnableUART
#include "uart_AppLayer.h"
#include "uart.h"
#include "strings.h"

volatile char RXdatabuf[UARTRXBUFFERSIZE]; // UART received data buffer
volatile char TXdatabuf[UARTTXBUFFERSIZE]; // UART Transmitted data buffer
volatile unsigned char RXdataPtr; // UART received data buffer pointer
volatile unsigned char TXdataPtr; // UART Transmitted data buffer pointer
volatile int TXdataLen; // UART Transmitted data buffer length
volatile char UARTmessageAvailableFlag;
volatile char echoFlag = TRUE;

extern volatile char Revision[];
extern volatile unsigned int Statistic_i2c_ErrorCnt;
extern volatile unsigned int Statistic_i2c_ErrorNoResponseCnt;
extern volatile int TimeofDayH;
extern volatile int TimeofDayM;
extern volatile int TimeofDayS;

//***********************************************************************
// 			Check UART function
// This routine checks to see if there is a message waiting in the receive 
// buffer. If there is, then it will process the messages.
//
// This routine should be called at the Application level in a timed loop.
// Its what makes it go.
//***********************************************************************

void ProcessUARTMessages() {
    if (UARTmessageAvailableFlag == TRUE && TXdataLen == 0) { // must make sure transmit buffer is empty if echo is enabled
        ProcessMessage();
        UARTmessageAvailableFlag = FALSE;
    }
}

//***********************************************************************
// 			Process UART Messages that were received
// Valid messages are:
//              echo = on/off
//              time = HH:MM:SS
//***********************************************************************

void ProcessMessage() {
    char left[20];
    char right[20];
    char buffer[UARTRXBUFFERSIZE];

    mystrcpytolower(buffer, RXdatabuf); // copy receive buffer immediately 
    RXdataPtr = 0;
    if (RXdatabuf[0] == '\0') {
        TxHelp();
        return;
    }
    //
    // Is it a command to write or set a variable?
    //
    if (split(left, right, buffer, "=") == 0) { // write commands have an '=' sign
        if (strncmp(left, "time",1) == 0) {
            strcpy(buffer, right);
            setTime(left, right, buffer);
        } else {
            TxCmdError(buffer);
        }
    }//
        // or was it was a read request?
        //
    else if (strncmp(buffer, "help",1) == 0) TxHelp();
    else if (strncmp(buffer, "revision",1) == 0) TxRevision();
    else if (strncmp(buffer, "errors",2) == 0) TxErrors();
    else if (strncmp(buffer, "noresponse",1) == 0) TxNoResponseErrors();
    else if (strncmp(buffer, "echo",1) == 0) toggleEcho();
        //
        // else an unrecognized message
        //
    else TxCmdError(buffer);
}

//***********************************************************************
//      Functions to set variables in response to write type messages
//***********************************************************************

int toggleEcho(void) {
    echoFlag = !echoFlag;
    TxEcho(); // transmit response
    return (0);
}

int setTime(char *left, char *right, char *buf) {
    int h, m, s;
    char buffer[UARTTXBUFFERSIZE];

    if (split(left, right, buf, ":") == 0) {
        h = atoi(left);
        strcpy(buffer, right);
        if (split(left, right, buffer, ":") == 0) {
            m = atoi(left);
            s = atoi(right);
            TimeofDayH = h;
            TimeofDayM = m;
            TimeofDayS = s;
            sprintf(buffer, "Time set to %d:%d:%d", TimeofDayH, TimeofDayM, TimeofDayS);
            WriteTXBuffer(buffer,NEWLINE);
            WriteTXBufferConst("Time set");
        } else {
            TxCmdError(buf);
        }

    } else {
        TxCmdError(buf);
    }
    return (0);
}
//***********************************************************************
//          Functions to transmit values to receiving device
//***********************************************************************

void TxEcho(void) {

    if (echoFlag == ON) {
        WriteTXBufferConst("Echo On");
    } else {
        WriteTXBufferConst("Echo off");
    }
}

void TxHelp(void) {
    WriteTXBufferConst("\nValid commands are:");
    WriteTXBufferConst("E or Echo => Toggles keyboard echo");
    WriteTXBufferConst("T=HH:MM:SS or time=HH:MM:SS => Set time (24 hour format)");
    WriteTXBufferConst("R or revision => Show program revision number");
    WriteTXBufferConst("Er or errors => Number of errors encountered on I2C interface");
    WriteTXBufferConst("N or noresponse => Number of no response errors encountered on I2C interface");
}

void TxRevision(void) {
    char buf[UARTTXBUFFERSIZE];

    sprintf(buf, "Revision %s", Revision);
    WriteTXBuffer(buf,NEWLINE);
}

void TxErrors(void) {
    char buf[UARTTXBUFFERSIZE];

    sprintf(buf, "I2C errors = %d", Statistic_i2c_ErrorCnt);
    WriteTXBuffer(buf,NEWLINE);
}

void TxNoResponseErrors(void) {
    char buf[UARTTXBUFFERSIZE];
    sprintf(buf, "I2C No Response errors = %d", Statistic_i2c_ErrorNoResponseCnt);
    WriteTXBuffer(buf,NEWLINE);
}

void TxCmdError(char * msg) {
    char buf[UARTTXBUFFERSIZE];

    sprintf(buf, "Command not recognized => %s", msg);
    WriteTXBuffer(buf,NEWLINE);
    TxHelp();
}

char TxBusy(void) {

    if (TXdataLen == 0) return (FALSE);
    return (TRUE);
}

#endif



