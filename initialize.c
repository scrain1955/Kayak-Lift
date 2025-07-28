// PIC16F1823 Configuration Bit Settings
// 'C' source line config statements
// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = NSLEEP    // Watchdog Timer Enable (WDT enabled except when sleeping)
#pragma config PWRTE = ON       // Power-up Timer Enable (PWRT enabled)
#pragma config MCLRE = ON       // MCLR/VPP Pin Function is MCLR
#pragma config CP = ON          // Flash Program Memory Code Protection (Program memory code protection is enabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = ALL        // Flash Memory Self-Write Protection (000h to 7FFh write protected, no addresses may be modified by EECON control)
#pragma config PLLEN = ON       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#include <xc.h>
#include <pic16f1823.h>
#include "defines.h"
#include "eeprom.h"
#ifdef EnableUART
#include "uart.h"
#endif

//
// Initialization routine
// Device is PIC16F1823
//

extern volatile char Revision[8];
extern volatile int TimeofDayH;
extern volatile int TimeofDayM;
extern volatile int TimeofDayS;

void initialize(void) {
    unsigned int i, j, tmp;

    PORTA = 0b00000000;
    // (Pin x)  PORTA.7 = unused, not pinned out
    // (Pin x)  PORTA.6 = unused, not pinned out
    // (Pin 2)  PORTA.5 = input, REMOTE3
    // (Pin 3)  PORTA.4 = input, REMOTE2
    // (Pin 4)  PORTA.3 = input,  MCLR, pull up enabled
    // (Pin 11) PORTA.2 = input, AtoD Battery Voltage, AN2
    // (Pin 12) PORTA.1 = input, Manual UP, ICSP Clock, pull up enabled
    // (Pin 13) PORTA.0 = input, Manual DOWN, ICSP Data, pull up enabled
    TRISA = 0b00111111; // 1 = input, 0 = output
    ANSELA = 0x04; // (1 = Analog, 0 = Digital)
    WPUA = 0x0B; // enable pull ups on serial programming pins MCLR,ISCPD,ISCPC (1 = pullup, 0 = no pull)

    PORTC = 0x00;
    // (Pin x)  PORTC.7 = unused, not pinned out
    // (Pin x)  PORTC.6 = unused, not pinned out
    // (Pin 5)  PORTC.5 = input, REMOTE1 and UART RX
    // (Pin 6)  PORTC.4 = input for REMOTE0, Output for UART TX
    // (Pin 7)  PORTC.3 = output, Down Solenoid
    // (Pin 8)  PORTC.2 = output, UP Solenoid
    // (Pin 9)  PORTC.1 = output, Motor Solenoid
    // (Pin 10) PORTC.0 = output, Solar Panel on/off switch
    TRISC = 0x20; // 1 = input, 0 = output
    ANSELC = 0x00; // 0 = Digital, 1 = Analog
    WPUC = 0xC0; // pull ups on unused

    nWPUEN = 0x00; // enable global pull ups (0=Enable)
    OSCCON = 0xF0; // switch to 32MHz Internal clock source w/ PLL enabled by config register
    WDTCON = 0x20; // 64 second Watchdog timer timeout period
    //
    // Configure Timer 2
    // Timer 2 will be used to create interrupts every millisecond for the purpose of general time
    // keeping functions
    //
    PR2 = 125; // set compare register to 125 for terminal count
    T2CON = 0x07; // post-scaler=1:1, timer=on, pre-scaler=64
    // timer clocks at 32MHz/4 = 8MHz => 8MHz/64(pre-scale) = 125KHz => 125KHz/125(PR2) = 1000Hz => 1000/1(post-scale) = 1000hz (1 msec)

    //
    // Configure Alternate Port functions for UART
    //
    //APFCONbits.RXDTSEL = 0; // UART RX default is on RC5 pin for PIC16F1823 
    //APFCON0bits.TXCKSEL = 0; // UART TX default is on RC4 pin for PIC16F1823 

    //
    // Configure the AtoD
    //
    FVRCON = 0x83; // Configure voltage reference to on, temperature indicator off, Reference voltage is 4.096V
    ADCON0 = 0x09; // Select channel AN2 (RA.2),enable AtoD, don't start yet
    ADCON1 = 0xE3; // Right Justified result, Fosc/64 clock (0.5Mhz), voltage reference is FVR and VSS
    // conversion will take 11.5 * 2uSec = 23 uSec
    ADIF = 0; // Clear any lingering interrupt
    ADIE = 0; // Disable AtoD interrupts, polling will be used
    //
    // Initialize variables from EEPROM
    //
    TMR2IF = 0;
    if (EERead(EEINITIALIZED0) != 0xAA55 || EERead(EEINITIALIZED1) != 0x55AA) { // Initialize EEprom if it has not been initialized
        EEWrite(EEINITIALIZED0, 0xAA55);
        EEWrite(EEINITIALIZED1, 0x55AA);
        EEWrite(EE_POR_CNT, 0x0000);
        EEWrite(EE_BROWN_CNT, 0x0000);
        EEWrite(EE_MCLR_CNT, 0x0000);
        EEWrite(EE_WDT_CNT, 0x0000);
        EEWrite(EE_INT_CNT, 0x0000);
        EEWrite(EE_TOTAL_RESTART_CNT, 0x0000);
    }
    //
    // Read Software Revision code from EEPROM
    //
    for (i = 0, j = 0; i < 8; i += 2) {
        tmp = EERead(EEREVISION + (int) i);
        if ((tmp & 0x00FF) != ' ') {
            Revision[j++] = tmp & 0x00FF;
            Revision[j] = '\0';
        }
        if (((tmp >> 8) & 0x00FF) != ' ') {
            Revision[j++] = (tmp >> 8) & 0x00FF;
            Revision[j] = '\0';
        }
    }
    //
    // Configure Interrupt on Change
    //
    // Rf Remote bits are on Ports:
    // PORTA.RA5 & PORTA.RA1 = UP
    // PORTA.RA4 & PORTA.RA0 = DOWN
    // IOC is only on PORTA
    IOCAP = 0x30; // enable Positive IOC. Note - RA5 & RA4 are positive edge
    IOCAN = 0x03; // enable Positive IOC. Note - RA1 & RA0 are Negative edge
    IOCAF = 0x00; // clear any lingering flags

    //
    // Configure and Start the UART
    //
#ifdef EnableUART
    UARTiniialize(); // Baud Rate is 115,200
#endif

    //
    // Enable the Interrupts
    //
    TMR2IE = 1; // enable timer 2 interrupt
    PEIE = 1; // enable peripheral interrupts
    IOCIE = 1; // enable the IOC interrupt
    GIE = 1; // enable master interrupt bit
    ADCON0bits.GO = 1; // start the AtoD conversion (23 uSec conversion time)
}
