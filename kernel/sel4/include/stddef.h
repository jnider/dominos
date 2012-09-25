/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

/*
  Authors: Ben Leslie
*/

#ifndef _STDDEF_H_
#define _STDDEF_H_

/* See C99 spec 7.17 */

/* We rely on the compiler telling us the right types to go in 
   here -- this makes a lot of sense, the compiler knows how big
   these things are
*/

#ifndef __SIZE_TYPE__
#ifdef __mips__
#define __SIZE_TYPE__ long unsigned int
#endif
#endif

#ifndef __PTRDIFF_TYPE__
#ifdef __mips__
#define __PTRDIFF_TYPE__ long int
#endif
#endif

typedef __PTRDIFF_TYPE__ ptrdiff_t ;
typedef __SIZE_TYPE__ size_t;
#ifndef __cplusplus /* FIXME: This is the one concession to C++ programmers that I make.
		       Allowing them to compile and use the standard C library. This is
		       more because C++ people shouldn't be restricted from using wonderful
		       things such as varargs and printf */
typedef __WCHAR_TYPE__ wchar_t;
#endif
#define NULL ((void *)0)
#define offsetof(type, member) ((size_t) &((type *)0)->member)

#endif /* _STDDEF_H_ */
