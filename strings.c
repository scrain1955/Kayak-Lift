
#include <xc.h>
#include <string.h>
#include <ctype.h>
//***********************************************************************
//              Split function
// returns 0 if search string was found, 1 otherwise
//***********************************************************************
int split(char * left, char * right, char * haystack, const char * needle) {
    
    left[0]='\0';
    right[0]='\0';
    char * ret;
    if ((ret = strstr(haystack,needle)) != 0)
    {
        strncpy(left,haystack,(unsigned)(ret - haystack));
        left[(unsigned char)ret - (unsigned char)haystack] = '\0';
        strcpy(right,&ret[0 + strlen(needle)]);
        return 0;
    }    
return 1;    
}

//***********************************************************************
//              mystrcpytolower function
// lower cases the resulting buffer
//***********************************************************************
void mystrcpytolower(char *buf, volatile char *str) {
    int i;
    for ( i=0; str[i] != '\0'; i++) {
        buf[i] = tolower(str[i]);
    }
    buf[i] = '\0';
}
//***********************************************************************
//              mystrcat function
// Catenates two strings, result is in first string
//***********************************************************************
void mystrcat(char *buf, volatile char *str) {
    int i,j;
    
    for (j=0; buf[j] != '\0'; j++) ;
    for (i=0; str[i] != '\0'; i++) {
        buf[j] = str[i];
    }
    buf[i] = '\0';
}

