/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

/*
Author: Ben Leslie
*/

#ifndef _STDIO_H_
#define _STDIO_H_

#include <stddef.h>
#include <stdarg.h>

#ifdef THREAD_SAFE
#include <mutex/mutex.h>
#define lock_stream(s) mutex_count_lock(&(s)->mutex)
#define unlock_stream(s) mutex_count_unlock(&(s)->mutex)
#else
#define lock_stream(s)
#define unlock_stream(s)
#endif
 
#define __UNGET_SIZE 10

struct __file {
	void *handle;

	size_t (*read_fn)(void *, long int, size_t, void *);
	size_t (*write_fn)(void *, long int, size_t, void *);
	int (*close_fn)(void *);
	long int (*eof_fn)(void *);

	unsigned char buffering_mode;
	char *buffer;

	unsigned char unget_pos;
	long int current_pos;

#ifdef THREAD_SAFE
	struct mutex mutex;
#endif
	
	int eof;
	int error;

	char unget_stack[__UNGET_SIZE];
};

typedef struct __file FILE; /* This needs to be done correctly */
typedef long fpos_t; /* same */

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

#define BUFSIZ 37
#define EOF (-1)

#define FOPEN_MAX 37
#define FILENAME_MAX 37
#define L_tmpnam 37

#define SEEK_CUR 0
#define SEEK_END 1
#define SEEK_SET 2

#define TMP_MAX 37

extern FILE *stderr;
extern FILE *stdin;
extern FILE *stdout;

/* 7.19.4 Operations on files */
int remove(const char *);
int rename(const char *, const char *);
FILE *tmpfile(void);
char *tmpnam(char *);
int fclose(FILE *);
int fflush(FILE *);
FILE *fopen(const char *, const char *);
FILE *freopen(const char *, const char *, FILE *);
void setbuf(FILE *, char *);
int setvbuf(FILE *, char *, int, size_t);

/* 7.19.6 Format i/o functions */
int fprintf(FILE *, const char *, ...);
int fscanf(FILE *, const char *, ...);
int printf(const char *, ...);
int scanf(const char *, ...);
int snprintf(char *, size_t , const char *, ...);
int sprintf(char *, const char *, ...);
int sscanf(const char *, const char *, ...);
int vfprintf(FILE *, const char *, va_list);
int vfscanf(FILE *, const char *, va_list);
int vprintf(const char *, va_list);
int vscanf(const char *, va_list);
int vsnprintf(char *, size_t, const char *, va_list );
int vsprintf(char *, const char *format, va_list arg);
int vsscanf(const char *s, const char *format, va_list arg);

/* 7.19.7 Character i/o functions */
int fgetc(FILE *);
char *fgets(char *, int, FILE *);
int fputc(int, FILE *);
int fputs(const char *, FILE *);

/* getc is specified to be the same as fgetc, so it makes
   the most sense to implement as a macro here */
#define getc fgetc /*int getc(FILE *); */

int getchar(void);
char *gets(char *);

/* putc is specified to be the same as fputc, so it makes
   the most sense to implement as a macro here */
#define putc fputc /*int fetc(int, FILE *); */

int putchar(int);
int puts(const char *);
int ungetc(int, FILE *);

/* 7.19.8 Direct I/O functions */
size_t fread(void *, size_t, size_t, FILE *);
size_t fwrite(const void *, size_t, size_t, FILE *);

/* 7.19.9 File positioning functions */
int fgetpos(FILE *, fpos_t *);
int fseek(FILE *, long int, int);
int fsetpos(FILE *, const fpos_t *);
long int ftell(FILE *);
void rewind(FILE *);

/* 7.19.10 Error-handling functions */
void clearerr(FILE *);
int feof(FILE *);
int ferror(FILE *);
void perror(const char *);

#endif /* _STDIO_H_ */
