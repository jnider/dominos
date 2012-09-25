/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

/*
 Authors: Cristan Szmajda, Ben Leslie
 Description: 
  Program diagnostics as per 7.2
 Status: Complete.
*/


/*
 * <assert.h> may safely be included multiple times with or without NDEBUG 
 */
#undef assert

/* Defining NDEBUG removes asserts */
#ifdef	NDEBUG
#define	assert(e)	((void)0)
#else
#define	assert(e)	((e) ? (void)0 : __assert(#e, __FILE__, __func__, __LINE__))
#endif

/* Implemented in src/assert.c */
#ifndef _ASSERT_H_
#define _ASSERT_H_

void __assert(const char *, const char *, const char *, int);

#endif /* _ASSERT_H_ */
