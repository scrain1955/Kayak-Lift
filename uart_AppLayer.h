#ifndef UART_APPLAYER_H
#define	UART_APPLAYER_H
#ifdef EnableUART

#ifndef UARTRXBUFFERSIZE
#define UARTRXBUFFERSIZE 40
#endif
#ifndef UARTTXBUFFERSIZE
#define UARTTXBUFFERSIZE 80
#endif

void ProcessUARTMessages(void);
void ProcessMessage(void);
int toggleEcho(void);
int setTime(char *left, char *right, char *buf);
void TxCmdError(char * msg);
void TxEcho(void);
void TxErrors(void);
void TxRevision(void);
void TxNoResponseErrors(void);
char TxBusy(void);
void TxHelp(void);
#endif
#endif	/* UART_APPLAYER_H */

