// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_CONTEXT_H
#define MORDAX_CONTEXT_H

/**
 * @defgroup context Thread Context Functions
 * @{
 */

/** Target-dependent thread context structure. */
struct thread_context;

/**
 * Pointer to the current user-space context. When in kernel-mode code, this
 * variable always points to the context information that is stored by the
 * interrupt handling code when entering kernel-mode.
 */
extern struct thread_context * current_context;

enum context_processor_mode { CONTEXT_USERMODE, CONTEXT_KERNELMODE };

/**
 * Creates a new thread context with default values for the registers.
 * @return a pointer to the newly allocated thread context.
 */
struct thread_context * context_new(void);

/**
 * Frees a thread context.
 * @param context the context to free.
 */
void context_free(struct thread_context * context);

/**
 * Copies context information.
 * @param dest destination.
 * @param src source.
 */
void context_copy(struct thread_context * dest, struct thread_context * src);

/**
 * Extracts a specific argument of an SVC from a thread context.
 * @param context the context to use.
 * @param num the argument number to extract, in the range 0-3.
 * @return the value of the specified argument.
 */
void * context_get_syscall_argument(struct thread_context * context, unsigned num);

/**
 * Sets the return value of an SVC in a thread context.
 * @param context the context to use.
 * @param value the value to set as the SVC return value.
 */
void context_set_syscall_retval(struct thread_context * context, void * value);

/**
 * Prints the contents of a thread context. Used for debugging.
 * @param context the context to print the contents of.
 */
void context_print(struct thread_context * context);

/**
 * Sets the processor mode field of a context.
 * @param context the context.
 * @param mode the processor mode of the context.
 */
void context_set_mode(struct thread_context * context, enum context_processor_mode mode);

/**
 * Sets the program counter register of a thread context.
 * @param context the context to set the register in.
 * @param pc the value to set the PC register to.
 */
void context_set_pc(struct thread_context * context, void * pc);

/**
 * Sets the stack pointer of a thread context.
 * @param context the context to set the register in.
 * @param sp the value to set the stack pointer register to.
 */
void context_set_sp(struct thread_context * context, void * sp);

/** @} */

#endif

