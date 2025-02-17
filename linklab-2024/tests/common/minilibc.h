// As if this is the C standard library headers.
#pragma once
#include <stdarg.h>
#include <stddef.h>
#include <sys/syscall.h>

#ifndef __MINILIBC_H__
#define __MINILIBC_H__

long syscall(int num, ...);
size_t strlen(const char* s);
char* strcpy(char* d, const char* s);
char* strchr(const char* s, int c);
void print(const char* s, ...);
int sprintf(char* buf, const char* fmt, ...);
int vsprintf(char* buf, const char* fmt, va_list ap);
int printf(const char* fmt, ...);

#endif // __MINILIBC_H__
