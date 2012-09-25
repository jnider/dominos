/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

/*
 Authors: Ben Leslie
 Description:
  Errors as per 7.5
 Status: Complete
 Restrictions: Single threaded
*/

#ifndef _ERRNO_H_
#define _ERRNO_H_

#define EDOM 1
#define EILSEQ 2
#define ERANGE 3

#ifndef THREAD_SAFE
extern int errno;
#else
#include <l4/thread.h>
 
#define errno (*((int *)__L4_TCR_ThreadLocalStorage()))
#endif

#endif /* _ERRNO_H_ */

