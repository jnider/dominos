/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

/*
 Author: Ben Leslie
*/
#ifndef _LIMITS_H_
#define _LIMITS_H_

#define CHAR_BIT 8
#define SCHAR_MIN (-128)
#define SCHAR_MAX 127
#define UCHAR_MAX 255

#ifdef __CHAR_UNSIGNED__
# define CHAR_MIN 0
# define CHAR_MAX UCHAR_MAX
#else
# define CHAR_MIN SCHAR_MIN
# define CHAR_MAX SCHAR_MAX
#endif

#define MB_LEN_MAX 1

#define SHRT_MIN (-32768)
#define SHRT_MAX  32767
#define USHRT_MAX 65535U

#define INT_MIN (-2147483648)
#define INT_MAX  2147483647
#define UINT_MAX 4294967295U

#define LONG_MIN (-2147483648)
#define LONG_MAX  2147483647
#define ULONG_MAX 4294967295U

#define LLONG_MIN (-9223372036854775808LL)
#define LLONG_MAX  9223372036854775807LL
#define ULLONG_MAX 18446744073709551615ULL

#endif /* _LIMITS_H_ */
