/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

#ifndef	_STDLIB_H_
#define	_STDLIB_H_

#include <stdint.h>
#include <stddef.h>

/* ISOC99 7.20 General Utilities */

/* 7.20.2 div types */
typedef struct {
	int quot, rem;
} div_t;

typedef struct {
	long quot, rem;
} ldiv_t;

typedef struct {
	long long quot, rem;
} lldiv_t;


/* 7.20.3 EXIT_ macros */  
#define	EXIT_FAILURE	1
#define	EXIT_SUCCESS	0

#define	RAND_MAX	INT_MAX
#define MB_CUR_MAX      1

/* 7.20.1 Numeric conversion functions */

/* 7.20.1-3 The strtod, strtof and strtold functions */
double strtod(const char *s, char **endp);
float strtof(const char *s, char **endp);
long double strtold(const char *s, char **endp);

/* 7.20.1-4 The strtol, stroll, stroul, strtoull functions */
long strtol(const char *s, char **endp, int base);
long long strtoll(const char *s, char **endp, int base);
unsigned long strtoul(const char *s, char **endp, int base);
unsigned long long strtoull(const char *s, char **endp, int base);

/* 7.20.1-1 atof function */
static inline double atof(const char *nptr)
{
	return strtod(nptr, (char **)NULL);
}

/* 7.20.1-2 The atoi, atol and atoll functions */
static inline int atoi(const char *nptr)
{
	return (int) strtol(nptr, (char **)NULL, 10);
}

static inline long atol(const char *nptr)
{
	return strtol(nptr, (char **)NULL, 10);
}

static inline long long atoll(const char *nptr)
{
	return strtoll(nptr, (char **)NULL, 10);
}

/* 7.20.2 Pseudo-random sequence generation functions */

int rand(void);
void srand(unsigned int seed);

/* 7.20.3 Memory management functions */

void *malloc(size_t);
void free(void *);
void *calloc(size_t, size_t);
void *realloc(void *, size_t);

/* 7.20.4 Communcation with the environment */

void abort(void);
int atexit(void (*func)(void));
void exit(int status);
void _Exit(int status);
char *getenv(const char *name);
int system(const char *string);

/* 7.20.5 Searching and sortin utilities */
void *bsearch(const void *key, const void *base, size_t nmemb, size_t, int (*compar)(const void *, const void*));
void qsort(void *base, size_t nmemb, size_t, int (*compar)(const void *, const void*));

/* 7.20.6 Integer arithmetic function */

/* FIXME: (benjl) Gcc defines these, but if we aren't using gcc it probably
   won't, but how do we know? Or maybe we should compile with -fnobuiltin? */

#ifndef abs
int abs(int);
#endif
#ifndef labs
long labs(long);
#endif
#ifndef llabs
long long llabs(long long);
#endif

#if 0
static inline int
abs(int x)
{
	return x < 0 ? -x : x;
}

static inline long
labs(long x)
{
	return x < 0 ? -x : x;
}

static inline long long
llabs(long long x)
{
	return x < 0 ? -x : x;
}
#endif
/* 7.20.7 Multibyte/wide character conversion functions */
#if 0 /* We don't have wide characters */
int mblen(const char *s, size_t n);
int mbtowc(wchar_t pwc, const char *s, size_t n);
int wctomb(char *s, wchat_t wc);
#endif

/* 7.20.8 Multibyte/wide string conversion functions */
#if 0 /* We don't have wide characters */
size_t mbstowcs(wchar_t *pwcs, const char *s, size_t n);
size_t wcstombs(char *s, constwchat_t *pwcs, size_t n);
#endif

#endif				/* _STDLIB_H_ */
