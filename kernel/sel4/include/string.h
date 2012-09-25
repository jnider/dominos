/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

#ifndef	_STRING_H_
#define	_STRING_H_

#include <stddef.h>

/* 7.21.2 Copying functions */
void *memcpy(void *s1, const void *s2, size_t n);
void *memmove(void *s1, const void *s2, size_t n);
char *strcpy(char *s1, const char *s2);
char *strncpy(char *s1, const char *s2, size_t n);

/* 7.21.3 Concatenation functions */
char *strcat(char *s1, const char *s2);
char *strncat(char *s1, const char *s2, size_t n);

/* 7.21.4 Comparison functions */
int memcmp(const void *s1, const void *s2, size_t n);
int strcmp(const char *s1, const char *s2);
int strcoll(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
size_t strxfrm(char *s1, const char *s2, size_t n);

/* 7.21.5 Search functions */
void *memchr(const void *s, int c, size_t n);
char *strchr(const char *s, int c);
size_t strcspn(const char *s1, const char *s2);
char *strpbrk(const char *s1, const char *s2);
char *strrchr(const char *s, int c);
size_t strspn(const char *s1, const char *s2);
char *strstr(const char *s1, const char *s2);
char *strtok(char *s1, const char *s2);

/* 7.21.6 Miscellaneous functions */
void *memset(void *, int, size_t);
char *strerror(int errnum);
size_t strlen(const char *);

/* Extra POSIX defined thigns that aren't part of the C standard */
#ifdef _USE_XOPEN
char *strdup(const char *);
#endif

#endif				/* _STRING_H_ */
