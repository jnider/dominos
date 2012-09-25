/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

#ifndef _MACROS_H_
#define _MACROS_H_

/*
 * Some compilers attempt to pack enums into the smallest possible type.
 * For ABI compatability with the kernel, we need to ensure they remain
 * the same size as an 'int'.
 */
#define SEL4_FORCE_LONG_ENUM(type) \
        _enum_pad_ ## type = (1 << ((sizeof(int)*8) - 1))

#endif /* _MACROS_H_ */

