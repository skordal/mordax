// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_UTILS_H
#define MORDAX_UTILS_H

#include <stdbool.h>
#include "api/types.h"

struct process;

/**
 * @defgroup utilities Utility functions
 * @{
 */

/**
 * Gets the minimum of two unsigned numbers.
 * @param a the first number to compare.
 * @param b the second number to compare.
 * @return the value of the least of the two numbers.
 */
static inline unsigned int min(unsigned int a, unsigned int b)
{
	return a < b ? a : b;
}

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
 * Duplicates a string.
 * @param s the string to duplicate.
 * @return a pointer to a newly allocated copy of the original string. This must
 *         be freed using `mm_free`.
 */
char * strdup(const char * s) __attribute((weak));

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
 * Lexically compares two strings.
 * @param a the first string to compare.
 * @param b the second string to compare.
 * @return greater than 1 if a < b, 0 if a = b and less than 0 if b < b.
 */
int strcmp(const char * restrict a, const char * restrict b)
	__attribute((weak));

/**
 * Clears the specified memory area.
 * @param memory the area of memory to clear.
 * @param length length of the memory area to clear.
 */
#define memclr(memory, length) memset(memory, 0, length)

/**
 * Fills a memory area with a specific value.
 * @param memory the memory area to fill.
 * @param value the value to fill the memory area with.
 * @param length the length of the area to fill.
 * @return the area filled.
 */
void * memset(void * restrict memory, char value, size_t length)
	__attribute((weak));

/**
 * Copies data from memory area A to memory area B.
 * @param dest the memory area to copy to.
 * @param src the memory area to copy from.
 * @param length length of the memory area to copy.
 */
void memcpy(void * dest, const void * src, size_t length)
	__attribute((weak));

/**
 * Copies data from process A to process B.
 * @param dest_addr destination address in process B.
 * @param dest_proc process B.
 * @param src_addr source address in process A.
 * @param src_proc process A.
 * @param length length of the memory area to copy.
 */
void memcpy_p(void * dest_addr, struct process * dest_proc,
	void * src_addr, struct process * src_proc, size_t length)
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

/** @} */
#endif

