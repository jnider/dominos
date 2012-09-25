/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

/*
 Author: Ben Leslie
 Description:
   Implementation based on C99 Section 7.16 Boolean type and values
*/

#ifndef __STDBOOL_H__
#define __STDBOOL_H__

#ifndef __cplusplus

#ifndef __bool_is_defined
#define __bool_is_defined
typedef _Bool bool;
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#endif /* __cplusplus */

#endif /* __STDBOOL_H__ */
