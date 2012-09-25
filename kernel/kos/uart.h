#ifndef _UART__H
#define _UART__H

enum UARTRate
{
   UART_2400,
   UART_4800,
   UART_9600,
   UART_14400,
   UART_28800,
   UART_33600,
   UART_57600,
   UART_115200
};

#define COM1   0x3f8

bool InitUART(unsigned int baseAddr, UARTRate rate);
char UARTReceive(unsigned int baseAddr);
void UARTTransmit(unsigned int baseAddr, char a);

#endif // _UART__H

