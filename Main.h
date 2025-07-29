#ifndef MAIN_H
#define	MAIN_H
void CheckForActionEvents(uint8_t buttonStatus);
uint8_t Poll_RcvrAndButtons(void);
void PrepareTXBuffer(char *buf, int minutes);
#endif	/* MAIN_H */

