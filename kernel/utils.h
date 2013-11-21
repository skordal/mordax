// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_UTILS_H
#define MORDAX_UTILS_H

#include <stdbool.h>
#include "api/types.h"

/**
 * Gets the base-2 logarithm of an unsigned integer.
 * @param x the number to get the logarithm of.
 * @return the base-2 logarithm of the input number.
 */
unsigned int log2(unsigned int x) __attribute((weak));

/**
 * Gets the length of a null terminated string.
 * @param s the string to measure.
 * @return the length of the string in bytes.
 */
size_t strlen(const char * s) __attribute((weak));

/**
 * Copies a string from src to dest including the terminating null byte.
 * @param dest the destination string.
 * @param src the source string.
 */
void strcpy(char * restrict dest, const char * restrict src)
	__attribute((weak));

/**
 * Finds the first token in the input string, where tokens are delimited by
 * the delimiter character. The delimiter is replaced by 0 and the input string
 * pointer is set to point to the next token.
 * @param string the string to search in. This parameter is updated to point
 *               to the next token after the first occurrence of the delimiter or
 *               set to 0 if the end of the string is reached.
 * @param delim the delimiting character.
 * @return the token consisting of the characters from the first function argument
 *         and up to the delimiter.
 */
char * strsep(char ** string, char delim) __attribute((weak));

/**
 * Compares two strings for equality.
 * @param a the first string to compare.
 * @param b the second string to compare.
 * @return `true` if the strings are equal and `false` otherwise.
 */
bool str_equals(const char * a, const char * b)
	__attribute((weak));

/**
 * Clears the specified memory area.
 * @param memory the area of memory to clear.
 * @param length length of the memory area to clear.
 */
void memclr(void * restrict memory, size_t length)
	__attribute((weak));

/**
 * Copies data from memory area A to memory area B.
 * @param dest the memory area to copy to.
 * @param src the memory area to copy from.
 * @param length length of the memory area to copy.
 */
void memcpy(void * dest, void * src, size_t length)
	__attribute((weak));

/**
 * Converts a 32-bit big endian integer to a 32-bit little endinan integer.
 * @param input the big endian integer to convert.
 * @return the little endian variant of the input.
 */
le32 be2le32(be32 input) __attribute((weak));

/**
 * Converts a 64-bit big endian integer to a 64 bit little endian integer.
 * @param input the big endian integer to convert.
 * @return the little endian variant of the input.
 */
le64 be2le64(be64 input) __attribute((weak));

#endif

