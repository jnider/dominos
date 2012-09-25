/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

/*
 Author: Ben Leslie
 Created: Fri Sep 24 2004 
 Description: time.h as per ISOC99
*/

#ifndef _TIME_H_
#define _TIME_H_

#include <stddef.h> /* For NULL, size_t */

#define CLOCKS_PER_SEC 1 /* Arbitrary, 1s resolution */

typedef long clock_t;
typedef long time_t;

struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};


/* 7.23.2 Time manipulation functions */
clock_t clock(void);
double difftime(time_t time1, time_t time2);
time_t mktime(struct tm *timeptr);
time_t time(time_t *timer);

/* 7.23.3 Time conversion functions */
char *asctime(const struct tm *timeptr);
struct tm *gmtime(const time_t *timer);
struct tm *localtime(const time_t *timer);

/* As per spec pg. 341 */
static inline
char *ctime(const time_t *timer)
{
	return asctime(localtime(timer));
}

size_t strftime(char *s, size_t maxsize, const char *format, 
		const struct tm *timeptr);

#endif /* _TIME_H_ */
