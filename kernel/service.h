// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_SERVICE_H
#define MORDAX_SERVICE_H

#include "process.h"
#include "queue.h"
#include "socket.h"
#include "thread.h"

/**
 * @defgroup service IPC Service Support
 * @{
 */

struct service
{
	char * name;
	struct process * owner;
	struct thread * listening_thread;
	struct queue * backlog;
};

/**
 * Initializes the service subsystem.
 */
void services_initialize(void);

/**
 * Looks up a service by name.
 * @param name the name of the service.
 * @return the service with the specified name, or 0 if the service does not
 *         exist.
 */
struct service * service_lookup(const char * name);

/**
 * Creates a new service.
 * @param name name of the service.
 * @param owner process that owns the service.
 * @return the new service object.
 */
struct service * service_create(const char * name, struct process * owner)
	__attribute((malloc));

/**
 * Destroys a service. This function removes the service from the list of
 * registered services and closes all related sockets.
 * @param svc the service object to destroy.
 */
void service_destroy(struct service * svc);

/**
 * Listens on a service.
 * @param svc the service to listen on.
 * @param listener the thread listening on the service.
 * @param server_socket a pointer used to return the server socket if a connection
 *                      was waiting when `service_listen` was called.
 * @param blocking a pointer to a variable that is set to `true` if the listener thread
 *                 should be moved to the blocking queue.
 * @return 0 if no error occurs, in which case `blocking` and `server_socket`
 *         should be used as return values, of less than 0 if an error occurs. In
 *         this case, the error code can be returned directly to the calling thread
 *         as a syscall return value.
 */
int service_listen(struct service * svc, struct thread * listener,
	struct socket ** server_socket, bool * blocking);

/**
 * Connects to a service.
 * @param svc the service to connect to.
 * @param connecting_thread the thread that is connecting.
 * @param client_socket a pointer used to return the client socket when the
 *                      connection is established.
 * @param blocking a pointer to a variable that is set to `true` if the connecting
 *                 thread should be moved to the blocking queue.
 * @return 0 if no error occurs, in which case `blocking` and `client_socket` is
 *         the actual return values. A value less than 0 indicates that an error
 *         has occured, and the error code can be returned directly to the calling
 *         thread as syscall return value.
 */
int service_connect(struct service * svc, struct thread * connecting_thread,
	struct socket ** client_socket, bool * blocking);

/** @} */

#endif

