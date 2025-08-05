#include <xc.h>
#include "Main.h"
#include "defines.h"
#include "globals.h"
#include "initialize.h"
#include "init_eeprom.h"
#include "eeprom.h"
#include "strings.h"
#ifdef EnableUART
#include "uart.h"
#endif

void main() {
    int seconds = 0;
    int minutes = 0;
    uint8_t Receiver_Status = 0;
    char buf[4] = {'x', 'x', 0x0D, '\0'};
    initialize();

    MachineState = MACHINE_STATE_IDLE;
    InactivityTimer = 0;
    cumulativeMilliSecs = 0;
    CLRWDT();
    while (cumulativeMilliSecs < STARTUP_DELAY_TIME); // delay until continuing

#ifdef EnableUART
    WriteTXBufferConst("Kayak Lift Revision ", NONEWLINE);
    WriteTXBuffer((char *) Revision, NEWLINE);
#endif

    //***********************************************************************
    //                   Main Loop    
    //***********************************************************************
    while (1) {
        //***********************************************************************
        //   Occurs Every Millisecond
        //***********************************************************************
        if (milliSecondTicFlag == TRUE) {
            milliSecondTicFlag = FALSE;
            Receiver_Status = Poll_RcvrAndButtons();
            CheckForActionEvents(Receiver_Status);
        }
        //***********************************************************************
        //   Occurs Every Second
        //***********************************************************************
        if (milliSecondTics >= 1000) {
            milliSecondTics = 0;
            seconds++;
            if (ActionInProcessTimer != 0) ActionInProcessTimer--;
            if (InactivityTimer < INACTIVITY_TIME_TILL_SLEEP) {
                InactivityTimer++;
            }
            if (InactivityTimer >= INACTIVITY_TIME_TILL_SLEEP) {
#ifdef EnableUART
                WriteTXBufferConst("Going to Sleep now.", NEWLINE);
                while (TxBusy()); // wait for message to be sent
#endif
                InactivityTimer = 0;
                SLEEP(); // go to sleep
                NOP();
            }
        }
        //***********************************************************************
        //   Occurs Every minute
        //***********************************************************************
        if (seconds >= 60) {
            seconds = 0;
            minutes++;
#ifdef EnableUART
            PrepareTXBuffer(buf, minutes);
            WriteTXBuffer(buf, NEWLINE);
#endif
        }
        //***********************************************************************
        //   Occurs Every Hour
        //***********************************************************************
        if (minutes >= 60) {
            minutes = 0;
        }
        CLRWDT(); // Clear the Watchdog timer
    }
}

//***********************************************************************
// Check status to see if action is needed
//***********************************************************************

void CheckForActionEvents(uint8_t buttonStatus) {

    if (buttonStatus == 0xFF) {
        return;
    } // de-bouncing in process so do nothing and return
    if (buttonStatus == 0x00) {
        UP_SOLENOID = OFF;
        DOWN_SOLENOID = OFF;
        MachineState = MACHINE_STATE_IDLE;
        ActionInProcessTimer = 0;
    }
    switch (MachineState) {
        case MACHINE_STATE_IDLE:
            if (buttonStatus == BUTTON_UP_MASK && UP_LIMIT_SWITCH == OFF) {
                MachineState = MACHINE_STATE_UP_LIMIT_REACHED;
#ifdef EnableUART
                WriteTXBufferConst("Max UP Limit reached", NEWLINE);
#endif 
            }

            if (buttonStatus == BUTTON_UP_MASK && UP_LIMIT_SWITCH == ON) {
                DOWN_SOLENOID = OFF;
                UP_SOLENOID = ON;
                MachineState = MACHINE_STATE_RUNNING;
#ifdef EnableUART
                WriteTXBufferConst("Up Button Pressed.", NEWLINE);
#endif
                ActionInProcessTimer = UP_ACTION_MAX_TIME; // set max run time
            }

            if (buttonStatus == BUTTON_DOWN_MASK) {
                UP_SOLENOID = OFF;
                DOWN_SOLENOID = ON; // Down Solenoid on
                MachineState = MACHINE_STATE_RUNNING;
#ifdef EnableUART
                WriteTXBufferConst("Down Button Pressed.", NEWLINE);
#endif
                ActionInProcessTimer = DOWN_ACTION_MAX_TIME;
            }
            break;
        case MACHINE_STATE_RUNNING:
            if (buttonStatus == BUTTON_UP_MASK && UP_LIMIT_SWITCH == OFF) {
                UP_SOLENOID = OFF;
                DOWN_SOLENOID = OFF;
                MachineState = MACHINE_STATE_UP_LIMIT_REACHED;
#ifdef EnableUART
                WriteTXBufferConst("UP Limit reached.", NEWLINE);
#endif
            }
            if (ActionInProcessTimer == 1) {
                UP_SOLENOID = OFF;
                DOWN_SOLENOID = OFF;
                MachineState = MACHINE_STATE_TIMEOUT;
#ifdef EnableUART
                WriteTXBufferConst("Timed Out.", NEWLINE);
#endif
            }
            break;
        case MACHINE_STATE_UP_LIMIT_REACHED: // stays off until buttons are released
            MachineState = MACHINE_STATE_UP_LIMIT_REACHED;
            break;
        case MACHINE_STATE_TIMEOUT: // stays off until buttons are released
            MachineState = MACHINE_STATE_TIMEOUT;
            break;
        default: // fall thru, should not happen
            MachineState = MACHINE_STATE_IDLE;
            break;
    }
    if (MachineState != MACHINE_STATE_IDLE) {
        InactivityTimer = 0; // reset inactive timer to prevent sleep for all states except idle
    }
}

//***********************************************************************
//     Poll the RF receiver and the Manual buttons. Return de-bounced status 
//     of buttons pressed
// Important - call this every millisecond to keep accurate button press 
//             time keeping. 
// Note - Return value of 0xFF means de-bouncing is in process
//
//***********************************************************************

uint8_t Poll_RcvrAndButtons(void) {

    uint8_t buttonStatus;

    buttonStatus = 0;
    if (PORTAbits.RA5 == ON || PORTAbits.RA1 == 0) buttonStatus |= BUTTON_UP_MASK; // Up button or Rf code
    if (PORTAbits.RA4 == ON || PORTAbits.RA0 == 0) buttonStatus |= BUTTON_DOWN_MASK; // Down button
    if (PORTCbits.RC1 == ON) buttonStatus |= RF_BUTTON_C_MASK;
    if (PORTAbits.RA2 == ON) buttonStatus |= RF_BUTTON_D_MASK;
    switch (buttonStatus) // only respond to a single button press, ignore multiples
    {
        case 0: // no buttons pressed
            Button_debounce_ON_cnt = 0;
            if (Button_debounce_OFF_cnt < BUTTON_DEBOUNCE_TIME_OFF) Button_debounce_OFF_cnt++;
            break;
        case RF_BUTTON_D_MASK:
        case RF_BUTTON_C_MASK:
        case BUTTON_DOWN_MASK:
        case BUTTON_UP_MASK:
            Button_debounce_OFF_cnt = 0;
            if (Button_debounce_ON_cnt < BUTTON_DEBOUNCE_TIME_ON) Button_debounce_ON_cnt++;
            break;
        default: // multiple buttons pressed, so clear counts and ignore
            Button_debounce_ON_cnt = 0;
            Button_debounce_OFF_cnt = 0;
    }
    if (Button_debounce_ON_cnt >= BUTTON_DEBOUNCE_TIME_ON || Button_debounce_OFF_cnt >= BUTTON_DEBOUNCE_TIME_OFF) return buttonStatus;
    return 0xFF; // return FF to indicate de-bouncing is in process
}

//***********************************************************************
//     Prepare the TX buffer to transmit seconds string
//***********************************************************************

void PrepareTXBuffer(char *buf, int minutes) {
    switch (minutes / 10) {
        case 0: buf[0] = ' ';
            break;
        case 1: buf[0] = '1';
            break;
        case 2: buf[0] = '2';
            break;
        case 3: buf[0] = '3';
            break;
        case 4: buf[0] = '4';
            break;
        case 5: buf[0] = '5';
            break;
        case 6: buf[0] = '6';
            break;
        case 7: buf[0] = '7';
            break;
        case 8: buf[0] = '8';
            break;
        case 9: buf[0] = '9';
            break;
        default: buf[0] = '0';
            break;
    }
    switch (minutes % 10) {
        case 0: buf[1] = '0';
            break;
        case 1: buf[1] = '1';
            break;
        case 2: buf[1] = '2';
            break;
        case 3: buf[1] = '3';
            break;
        case 4: buf[1] = '4';
            break;
        case 5: buf[1] = '5';
            break;
        case 6: buf[1] = '6';
            break;
        case 7: buf[1] = '7';
            break;
        case 8: buf[1] = '8';
            break;
        case 9: buf[1] = '9';
            break;
        default: buf[1] = '0';
            break;
    }
}
