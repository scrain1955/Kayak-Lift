#ifndef INIT_EEPROM_H
#define	INIT_EEPROM_H
// write in 8 byte blocks
__EEPROM_DATA(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF); // 0x00
__EEPROM_DATA(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF); // 0x08
__EEPROM_DATA('K', 'a', 'y', 'a', 'k', ' ', 'L', 'i'); // 0x10
__EEPROM_DATA('f', 't', ' ', ' ', ' ', ' ', ' ', ' '); // 0x18
__EEPROM_DATA(' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '); // 0x20
__EEPROM_DATA('R', 'e', 'v', 'i', 's', 'i', 'o', 'n'); // 0x28
__EEPROM_DATA(' ', '1', '.', '0', ' ', ' ', ' ', ' '); // 0x30
#endif	/* INIT_EEPROM_H */

