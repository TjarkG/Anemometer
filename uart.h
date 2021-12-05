#ifndef uart_H
#define uart_H

#include <avr/io.h>
#include <string.h>

#define BAUD 9600UL      // Baudrate

#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD)

#if ((BAUD_ERROR<990) || (BAUD_ERROR>1010))
  #error Systematischer Fehler der Baudrate grösser 1% 
#endif

/* reverse:  reverse string s in place */
 void reverse(char s[])
 {
     int i, j;
     char c;

     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
} 
 
 /* itoa:  convert n to characters in s */
 void _itoa(long n, char s[])
 {
     unsigned long i, sign;

     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
}

void USART_Transmit( unsigned char data )       //Send one Char via UART
{
    while ( !( UCSRA & (1<<UDRE)) )
        ;
    UDR = data;
}

void uartWriteString(const char *in)
{
    while(*in)
        USART_Transmit(*in++);
}

void uartWriteInt(const signed int in)  //send an int in hex
{
    USART_Transmit((in>>8));
    USART_Transmit(in & 0xFF);
}

void uartWriteIntLine(long in)
{
    char tmp[12];
    _itoa(in,tmp);
    uartWriteString(tmp);
    uartWriteString("\r\n");
}

void uartInit(void)
{
    UBRRH = UBRR_VAL >> 8;
    UBRRL = UBRR_VAL & 0xFF;
    UCSRB |= (1<<TXEN);  // UART TX einschalten
    UCSRC = (1<<USBS) | (3<<UCSZ0);  // Asynchron 8N1 
}

#endif