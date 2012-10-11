////////////////////////////////////////////////////////////////////////////////
///@file    kstdio.h
///@short   Kernel implementation of std C lib
///@author  J.Nider
///@date    01-Jun-2007 12:29:01 PM
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef _STDIO__H
#define _STDIO__H

#define __KERNEL_RETURN()              __ASM("leave\nlret\n")

#define SEGMENT_INDEX(_index, _ldt, _priv)  ((_index << 3) | (_ldt << 2) | _priv)   ///< makes a segment index

typedef long unsigned int size_t; // JKN - leave this for the stdlib (pdclib)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void k_doNothing(void);
void k_printf(const char *format, ...);
void k_memset(void*, char value, int size);
void k_memcpy(void*, const void*, int size);
void k_strcpy(char* dest, const char* src);
char* k_strncpy(char* s1, const char* s2, size_t n);
int k_strlen(const char* a);
int k_strcmp(const char* a, const char* b);
int k_strncmp(const char* s1, const char* s2, size_t n);
int k_isspace(char c);
char* k_strtok(char* s1, const char* s2);
char* k_strdup(const char* s1);
int k_atoi(const char* a);
#define k_memmove k_memcpy

int substring (const char *s1, const char *s2);

// port functions
void outb (unsigned short port, unsigned char data);
unsigned char inb(unsigned short port);
//void outs (unsigned short port, unsigned short data);
//unsigned short ins(unsigned short port);
void outl (unsigned short port, unsigned long data);
unsigned long inl(unsigned short port);

/* memory management functions */
void k_initHeap(int address, int size);
void* kmalloc(int size);
void kfree(void* ptr);
unsigned int k_freeMem(void);
void k_itoa (char *buf, int base, int d);

void pause();

// debug hex dump
void k_hexout(const char* pBuffer, unsigned int length);

void halt();

#define __ASM asm
#define _ENABLE_INTERRUPTS() __ASM("sti");
#define _DISABLE_INTERRUPTS() __ASM("cli");
#define __PACKED __attribute__((packed))
#define READ_REGISTER_PTR(_reg, _var) __ASM("movl %%" _reg ", %%eax\n movl %%eax, %0" :: "am"(_var))

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _STDIO__H */
