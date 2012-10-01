#ifndef _SERIAL_H
#define _SERIAL_H

/** @file
 *
 * Serial driver functions
 *
 */

void serial_putc(int ch);
int serial_getc(void);
int serial_init(void);

#endif /* _SERIAL_H */
