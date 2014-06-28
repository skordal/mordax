// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_SYSCALL_H
#define MORDAX_SYSCALL_H

#include "context.h"

/**
 * @defgroup syscalls System Calls
 * System call handlers.
 * @{
 */

/**
 * System syscall handler.
 * @param context process context information.
 */
void syscall_system(struct thread_context * context);

/**
 * Thread exit syscall handler. Takes the exit status as parameter.
 * @param context process context information.
 */
void syscall_thread_exit(struct thread_context * context);

/**
 * Thread create syscall handler. Takes two parameters,
 * the entry point of the new thread and the top of the stack
 * for the new thread.
 * @param context process context information.
 */
void syscall_thread_create(struct thread_context * context);

/**
 * Thread join syscall handler. Takes the TID of the thread
 * to join as parameter and returns the return value of the
 * thread, or -1 if the thread ID did not exist.
 * @todo This syscall returns -1 even if the thread previously existed.
 * @param context process context information.
 */
void syscall_thread_join(struct thread_context * context);

/**
 * Thread information syscall handler. Takes an integer parameter
 * specifying the information to return.
 */
void syscall_thread_info(struct thread_context * context);

/**
 * Process creation syscall handler. Takes a pointer to a filled in
 * mordax_process_info structure as parameter.
 */
void syscall_process_create(struct thread_context * context);

/**
 * Maps physical memory to virtual memory. Takes four arguments;
 * the target address, the source address, the size of the mapping
 * and a pointer to the desired attributes of the memory.
 */
void syscall_memory_map(struct thread_context * context);

/**
 * Allocates physical memory and maps it into virtual memory.
 * Takes three arguments; the target address, a pointer to the
 * size of the requested memory and a pointer to the desired
 * attributes of the memory. A pointer to the mapped memory is
 * returned from the syscall, as well as the size argument being
 * updated with the actual size allocated and mapped.
 */
void syscall_memory_map_alloc(struct thread_context * context);

/**
 * Unmaps mapped virtual memory. Takes two arguments; the address
 * to start unmapping at and the size of the area to unmap.
 */
void syscall_memory_unmap(struct thread_context * context);

/**
 * Creates a new IPC service. Takes the name of the service to
 * create and the length of the name (not including terminating
 * NULL byte) as arguments.
 */
void syscall_service_create(struct thread_context * context);

/**
 * Listens on an IPC service. Takes the service handle as argument.
 * Returns a handle to the socket of connecting clients.
 */
void syscall_service_listen(struct thread_context * context);

/**
 * Destroys an IPC service. Takes the service handle returned from
 * the service_create syscall as argument.
 */
void syscall_service_destroy(struct thread_context * context);

/**
 * Connects to an IPC service. Takes two arguments; the name of
 * the service to connect to and the length of the name (not including
 * the terminating NULL byte). Returns a handle to the connected socket.
 */
void syscall_service_connect(struct thread_context * context);

/**
 * Sends a message on a connected IPC socket. Takes three arguments, the
 * identifier of the socket, the data to send and the length of the data.
 * Returns the number of bytes sent or a negative error code.
 */
void syscall_socket_send(struct thread_context * context);

/**
 * Receives a message from a connected IPC socket. Takes three arguments,
 * the identifier of the socket, the location to store the received data and
 * the length of the buffer. Returns the number of bytes received or a negative
 * error code.
 */
void syscall_socket_receive(struct thread_context * context);

/**
 * Waits for a message on a socket and returns its size. Takes one argument,
 * which is the identifier of the socket. Returns the size of the waiting
 * message or blocks until a message is available and then returns the size.
 */
void syscall_socket_wait(struct thread_context * context);

/**
 * Creates a lock resource. Returns the resource identifier of the lock.
 */
void syscall_lock_create(struct thread_context * context);

/**
 * Attempts to aquire a lock. Takes the resource identifier of the lock as
 * parameter. If the lock is locked by another thread, the calling thread
 * is set as blocking until the lock can be aquired. On success 0 is returned,
 * otherwise a negative error code is returned.
 */
void syscall_lock_aquire(struct thread_context * context);

/**
 * Releases a lock. Takes the resource identifier of the lock as parameter.
 * Returns 0 on success and a negative error code on failure.
 */
void syscall_lock_release(struct thread_context * context);

/**
 * Gets a device tree node by its path. Takes two arguments, the path of the
 * device tree node and the length of the path. Returns a resource identifier
 * representing the device tree node on success or a negative error code.
 */
void syscall_dt_get_node_by_path(struct thread_context * context);

/**
 * Gets a device tree node by its phandle. Takes the phandle of the device
 * tree node as argument. Returns a resource identifier on success or a negative
 * error code on error.
 */
void syscall_dt_get_node_by_phandle(struct thread_context * context);

/**
 * Gets a device tree node by its compatible property. Takes two parameters,
 * a `mordax_dt_string` structure with the compatible value and an integer
 * specifying the index into the array of possible return values to retrieve.
 * Returns a resource identifier on success or a negative error code on error.
 */
void syscall_dt_get_node_by_compatible(struct thread_context * context);

/**
 * Gets an array of 32-bits integers from a device tree node attribute.
 * Takes four arguments; the resource handle of the device tree node,
 * a pointer to a mordax_dt_string structure containing the name,
 * a pointer to an array of 32-bits integers to return the value in, and
 * the length of the array to return. On success 0 is returned, otherwise
 * a negative error value is returned.
 */
void syscall_dt_get_property_array32(struct thread_context * context);

/**
 * Gets a string property from a device tree node. Takes three
 * arguments; the resource handle of the device tree node, the name of
 * the property in the form of a pointer to a mordax_dt_string structure
 * and a pointer to a mordax_dt_string structure in which the string result
 * is returned. On success 0 is returned, otherwise a negative error value
 * is returned.
 */
void syscall_dt_get_property_string(struct thread_context * context);

/**
 * Gets a phandle property from a device tree node. Takes three arguments,
 * the resource handle of the device tree node, a pointer to a
 * mordax_dt_string structure with the name of the property and a pointer to
 * where the phandle result should be stored. On success 0 is returned, otherwise
 * a negative error code is returned.
 */
void syscall_dt_get_property_phandle(struct thread_context * context);

/**
 * Creates an IRQ object. Takes one parameter, the number of the IRQ.
 * If successful, 0 is returned, otherwise a negative error code is returned.
 */
void syscall_irq_create(struct thread_context * context);

/**
 * Listens on an IRQ object. Takes one parameter, the handle of the IRQ object.
 * If the IRQ is asserted, 0 is returned. Otherwise, a negative error code is
 * returned.
 */
void syscall_irq_listen(struct thread_context * context);

/**
 * Frees and destroys a resource. Takes the identifier of the resource
 * as argument.
 */
void syscall_resource_destroy(struct thread_context * context);

/** @} */

#endif
