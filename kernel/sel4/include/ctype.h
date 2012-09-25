/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

/*
 Authors: Cristan Szmadja, Ben Leslie
 Description: 
  Implements character handling as per 7.4
 Status: Incomplete
  Ignores locale
*/

#ifndef _CTYPE_H_
#define	_CTYPE_H_

static inline int
iscntrl(int c)
{
	return (0 <= c && c <= '\037') || c == '\177';
}

static inline int
isgraph(int c)
{
	return ' ' < c && c < '\177';
}

static inline int
isprint(int c)
{
	return ' ' <= c && c < '\177';
}

static inline int
isdigit(int c)
{
	return '0' <= c && c <= '9';
}

static inline int
islower(int c)
{
	return 'a' <= c && c <= 'z';
}

static inline int
isspace(int c)
{
	return c == ' ' || ('\011' <= c && c <= '\015');
}

static inline int
isupper(int c)
{
	return 'A' <= c && c <= 'Z';
}

static inline int
isalpha(int c)
{
	return isupper(c) || islower(c);
}

static inline int
isalnum(int c)
{
	return isalpha(c) || isdigit(c);
}

static inline int
ispunct(int c)
{
	return isgraph(c) && !isalnum(c);
}

static inline int
isxdigit(int c)
{
	return isdigit(c) || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f');
}

static inline int
tolower(int c)
{
	return isupper(c) ? c + ('a' - 'A') : c;
}

static inline int
toupper(int c)
{
	return islower(c) ? c - ('a' - 'A') : c;
}

static inline int
isascii(int c)
{
	return 0 <= c && c <= '\177';
}

static inline int
toascii(int c)
{
	return c & 0177;
}

static inline int
digittoint(int c)
{
	return isdigit(c) ? c - '0' : 'A' <= c
	    && c <= 'Z' ? c - 'A' : 'a' <= c && c <= 'z' ? c - 'a' : 99;
}

#endif				/* !_CTYPE_H_ */

/* XXX: Extra things needed by POSIX... need to find a correct #ifndef */

/*

int isascii(int);
int toascii(int);

#define _toupper(x)
#define _tolower(x)


*/



