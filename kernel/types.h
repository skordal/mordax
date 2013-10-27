// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_TYPES_H
#define MORDAX_TYPES_H

#include <stdbool.h>
#include <stdint.h>

/** Object size type. */
typedef uint32_t size_t;

/** Physical pointer type. */
typedef void * physical_ptr;

/** Process identitifer type. */
typedef int32_t pid_t;
/** Thread identifier type. */
typedef int32_t tid_t;

/** 64-bit generic big endian type. */
typedef uint64_t be64;
/** 32-bit generic big endian type. */
typedef uint32_t be32;

/** 64-bit generic little endian type. */
typedef uint64_t le64;
/** 32-bit generic little endian type. */
typedef uint32_t le32;

#endif

