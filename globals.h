//************************************************************************
//              Global Variables
//***********************************************************************
#ifndef GLOBALS_H
#define GLOBALS_H

#include "defines.h"

volatile char Revision[8] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
volatile uint8_t milliSecondTicFlag;
volatile int milliSecondTics;
volatile unsigned int cumulativeMilliSecs;
volatile uint8_t MachineState;
volatile unsigned int MotorDelayTimer;
volatile unsigned int ActionInProcessTimer;
volatile int BatteryVoltage;
volatile int Button_debounce_ON_cnt;
volatile int Button_debounce_OFF_cnt;
volatile int InactivityTimer;
#endif
