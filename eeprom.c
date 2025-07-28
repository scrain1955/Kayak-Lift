#include	<xc.h>
#include	"defines.h"
#include "eeprom.h"

//***********************************************************************
//		 			Read EEPROM Memory
// This routine only reads 16 bit values
//***********************************************************************

unsigned int EERead(int addr) {
    unsigned int retdata;
    int i;
    int j;

    retdata = 0;
    for (j = 0, i = 1; j < 2; j++, i--) {
        EEADRL = (unsigned char) (addr + i);
        EECON1bits.RD = 1; // Start the read cycle
        retdata = retdata << 8;
        retdata = retdata | EEDATL;
    }
    return (retdata);
}

//***********************************************************************
//		 			Write EEPROM Memory
// This routine only writes 16 bit values
//***********************************************************************

char EEWrite(int addr, unsigned int val) {
    char i;
    unsigned int tmp;
    char rc;

    EECON1bits.WREN = 1;
    for (i = 0, rc = 0, tmp = val; i < 2; i++) {
        EEADRL = (unsigned char) (addr + i);
        EEDATL = (unsigned char) tmp; // write lower byte
        GIE = 0; // disable interrupts
        EECON2 = 0x55; // unlock the write cycle
        EECON2 = 0xAA;
        EECON1bits.WR = 1; // Start the write cycle
        GIE = 1; // Re-enable the interrupts
        tmp = tmp >> 8;
        while (EECON1bits.WR); // wait until write completes
        if (EECON1bits.WRERR) rc = 1;
    }
    return (rc);
}

