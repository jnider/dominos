/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

/*
 Authors: Ben Leslie
*/

#ifndef _INT_TYPES_
#define _INT_TYPES_

#include <stdint.h>

#include <arch/inttypes.h>

/* 7.8.1 Macros for format specifies */

/* 7.8.1.2 signed integers */
#define PRId8 __LENGTH_8_MOD "d"
#define PRId16 __LENGTH_16_MOD "d"
#define PRId32 __LENGTH_32_MOD "d"
#define PRId64 __LENGTH_64_MOD "d"

#define PRIi8 __LENGTH_8_MOD "i"
#define PRIi16 __LENGTH_16_MOD "i"
#define PRIi32 __LENGTH_32_MOD "i"
#define PRIi64 __LENGTH_64_MOD "i"

#define PRIdLEAST8 __LENGTH_LEAST8_MOD "d"
#define PRIdLEAST16 __LENGTH_LEAST16_MOD "d"
#define PRIdLEAST32 __LENGTH_LEAST32_MOD "d"
#define PRIdLEAST64 __LENGTH_LEAST64_MOD "d"

#define PRIiLEAST8 __LENGTH_LEAST8_MOD "i"
#define PRIiLEAST16 __LENGTH_LEAST16_MOD "i"
#define PRIiLEAST32 __LENGTH_LEAST32_MOD "i"
#define PRIiLEAST64 __LENGTH_LEAST64_MOD "i"

#define PRIdFAST8 __LENGTH_FAST8_MOD "d"
#define PRIdFAST16 __LENGTH_FAST16_MOD "d"
#define PRIdFAST32 __LENGTH_FAST32_MOD "d"
#define PRIdFAST64 __LENGTH_FAST64_MOD "d"

#define PRIiFAST8 __LENGTH_FAST8_MOD "i"
#define PRIiFAST16 __LENGTH_FAST16_MOD "i"
#define PRIiFAST32 __LENGTH_FAST32_MOD "i"
#define PRIiFAST64 __LENGTH_FAST64_MOD "i"

#define PRIdMAX __LENGTH_MAX_MOD "d" 
#define PRIiMAX __LENGTH_MAX_MOD "i"

#define PRIdPTR __LENGTH_PTR_MOD "d"
#define PRIiPTR __LENGTH_PTR_MOD "i"

/* 7.8 __LENGTH_8_MOD.1.3 unsigned integers */

#define PRIo8 __LENGTH_8_MOD "o"
#define PRIo16 __LENGTH_16_MOD "o"
#define PRIo32 __LENGTH_32_MOD "o"
#define PRIo64 __LENGTH_64_MOD "o"

#define PRIu8 __LENGTH_8_MOD "u"
#define PRIu16 __LENGTH_16_MOD "u"
#define PRIu32 __LENGTH_32_MOD "u"
#define PRIu64 __LENGTH_64_MOD "u"

#define PRIx8 __LENGTH_8_MOD "x"
#define PRIx16 __LENGTH_16_MOD "x"
#define PRIx32 __LENGTH_32_MOD "x"
#define PRIx64 __LENGTH_64_MOD "x"

#define PRIX8 __LENGTH_8_MOD "X"
#define PRIX16 __LENGTH_16_MOD "X"
#define PRIX32 __LENGTH_32_MOD "X"
#define PRIX64 __LENGTH_64_MOD "X"

#define PRIoLEAST8 __LENGTH_LEAST8_MOD "o"
#define PRIoLEAST16 __LENGTH_LEAST16_MOD "o"
#define PRIoLEAST32 __LENGTH_LEAST32_MOD "o"
#define PRIoLEAST64 __LENGTH_LEAST64_MOD "o"

#define PRIuLEAST8 __LENGTH_LEAST8_MOD "u"
#define PRIuLEAST16 __LENGTH_LEAST16_MOD "u"
#define PRIuLEAST32 __LENGTH_LEAST32_MOD "u"
#define PRIuLEAST64 __LENGTH_LEAST64_MOD "u"

#define PRIxLEAST8 __LENGTH_LEAST8_MOD "x"
#define PRIxLEAST16 __LENGTH_LEAST16_MOD "x"
#define PRIxLEAST32 __LENGTH_LEAST32_MOD "x"
#define PRIxLEAST64 __LENGTH_LEAST64_MOD "x"

#define PRIXLEAST8 __LENGTH_LEAST8_MOD "X"
#define PRIXLEAST16 __LENGTH_LEAST16_MOD "X"
#define PRIXLEAST32 __LENGTH_LEAST32_MOD "X"
#define PRIXLEAST64 __LENGTH_LEAST64_MOD "X"

#define PRIoFAST8 __LENGTH_FAST8_MOD "o"
#define PRIoFAST16 __LENGTH_FAST16_MOD "o"
#define PRIoFAST32 __LENGTH_FAST32_MOD "o"
#define PRIoFAST64 __LENGTH_FAST64_MOD "o"

#define PRIuFAST8 __LENGTH_FAST8_MOD "u"
#define PRIuFAST16 __LENGTH_FAST16_MOD "u"
#define PRIuFAST32 __LENGTH_FAST32_MOD "u"
#define PRIuFAST64 __LENGTH_FAST64_MOD "u"

#define PRIxFAST8 __LENGTH_FAST8_MOD "x"
#define PRIxFAST16 __LENGTH_FAST16_MOD "x"
#define PRIxFAST32 __LENGTH_FAST32_MOD "x"
#define PRIxFAST64 __LENGTH_FAST64_MOD "x"

#define PRIXFAST8 __LENGTH_FAST8_MOD "X"
#define PRIXFAST16 __LENGTH_FAST16_MOD "X"
#define PRIXFAST32 __LENGTH_FAST32_MOD "X"
#define PRIXFAST64 __LENGTH_FAST64_MOD "X"

#define PRIoMAX __LENGTH_MAX_MOD "o"
#define PRIuMAX __LENGTH_MAX_MOD "u"
#define PRIxMAX __LENGTH_MAX_MOD "x"
#define PRIXMAX __LENGTH_MAX_MOD "X"

#define PRIoPTR __LENGTH_PTR_MOD "o"
#define PRIuPTR __LENGTH_PTR_MOD "u" 
#define PRIxPTR __LENGTH_PTR_MOD "x"
#define PRIXPTR __LENGTH_PTR_MOD "X"

#endif
