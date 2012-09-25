/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

/*
  Author: Ben Leslie, Alex Webster
*/
#ifndef _SETJMP_H_
#define _SETJMP_H_

#include <arch/setjmp.h>

int setjmp(jmp_buf);
void longjmp(jmp_buf, int);

#endif /* _SETJMP_H_ */
