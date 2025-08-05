//************************************************************
// Revision 1.0 7/29/2025
// MPLabX v6.15; Compiler XC8 v2.46 Pro ; Toolpack 1.7.242
//   PIC16F1823 Checksum 21E4 (dashboard) FB80 (ID memory) with uart on
// - Revision is set in init_eeprom.h
// - First release of completed Kayak Lift
//************************************************************
//#define DEBUG
#define EnableUART
#define RISINGEDGE 0x05
#define FALLINGEDGE 0x04
#define TRUE 1u
#define FALSE 0u
#define ON 1u
#define OFF 0u

#define BUTTON_DEBOUNCE_TIME_ON 50 // milliseconds
#define BUTTON_DEBOUNCE_TIME_OFF 50 // milliseconds
#define UP_ACTION_MAX_TIME 12 // seconds
#define DOWN_ACTION_MAX_TIME 10 // seconds
#define STARTUP_DELAY_TIME 3000 // millisecs

// machine States
#define MACHINE_STATE_IDLE 0
#define MACHINE_STATE_RUNNING 10
#define MACHINE_STATE_TIMEOUT 20
#define MACHINE_STATE_UP_LIMIT_REACHED 30

#define INACTIVITY_TIME_TILL_SLEEP 300 // seconds

#define UP_LIMIT_SWITCH PORTCbits.RC0
#define UP_SOLENOID PORTCbits.RC2
#define DOWN_SOLENOID PORTCbits.RC3

#define BUTTON_UP_MASK 0x01
#define BUTTON_DOWN_MASK 0x02
#define RF_BUTTON_C_MASK 0x04
#define RF_BUTTON_D_MASK 0x08
//*********************************************************************
// 			EEPROM memory Map
//*********************************************************************
#define EEINITIALIZED0                  0x0000
#define EEINITIALIZED1                  0x0002
#define EE_POR_CNT                      0x0004
#define EE_BROWN_CNT                    0x0006
#define EE_MCLR_CNT                     0x0008
#define EE_WDT_CNT                      0x000A
#define EE_INT_CNT                      0x000C
#define EE_TOTAL_RESTART_CNT            0x000E
#define EEREVISION                      0x0030
