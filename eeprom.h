#ifndef EEPROM_H
#define	EEPROM_H

unsigned int EERead(int addr);
char EEWrite(int addr, unsigned int val);

#endif	/* EEPROM_H */

