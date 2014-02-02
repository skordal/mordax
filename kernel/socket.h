// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_SOCKET_H
#define MORDAX_SOCKET_H

#include <stdbool.h>

#include "queue.h"
#include "thread.h"

/**
 * @defgroup socket IPC Socket Support
 * @{
 */

#ifndef CONFIG_IPC_BUFFER_LENGTH
#error "IPC socket buffer size is not set, define CONFIG_IPC_MSG_BUFLEN with the proper value"
#endif

/**
 * IPC socket structure.
 */
struct socket
{
	struct socket * endpoint;
	struct thread * owner;

	struct thread * blocking_receiver;
	struct thread * blocking_sender;
	struct thread * blocking_waiter;

	struct {
		void * buffer;
		size_t length;
	} blocking_details;
};

/**
 * Creates a new, unconnected socket.
 */
struct socket * socket_create(struct thread * owner) __attribute((malloc));

/**
 * Closes and destroys a socket.
 * @param sock the socket to close and destroy.
 */
void socket_destroy(struct socket * sock);

/**
 * Connects two sockets together.
 * @param a socket A.
 * @param b socket B.
 * @return `true` if successful, `false` otherwise.
 */
bool socket_connect(struct socket * a, struct socket * b);

/**
 * Waits for a message to arrive and returns the size of it.
 * This does not remove the message from the message queue and socket_receive
 * must be called to receive the message.
 * @param sock the socket to wait on.
 * @param waiting_thread the thread doing the waiting.
 * @param block a pointer to a variable that is set to `true` if the sending thread
 *              should be moved to the blocking queue.
 * @return length of the received message or < 0 on error.
 */
int socket_wait(struct socket * sock, struct thread * waiting_thread,
	bool * block);

/**
 * Receives a message from a socket's endpoint. If the specified length does
 * not encompass the entire message, the next call to this function will
 * retrieve the next part of the message.
 * @param sock the socket to receive from.
 * @param receiving_thread the thread doing the receiving.
 * @param buffer a buffer to store the received message into.
 * @param length length of the receive buffer.
 * @param block a pointer to a variable that is set to `true` if the sending
 *              thread should be moved to the blocking queue.
 * @return length of the received message or < 0 on error.
 */
int socket_receive(struct socket * sock, struct thread * receiving_thread,
	void * buffer, size_t length, bool * block);

/**
 * Sends a message to a socket's endpoint.
 * @param sock the socket to send on.
 * @param sending_thread thread that is doing the sending.
 * @param buffer a buffer containing the message to send.
 * @param length length of the send buffer.
 * @param block a pointer to a variable that is set to `true` if the sending
 *              thread should be moved to the blocking queue.
 * @return number of bytes sent.
 */
int socket_send(struct socket * sock, struct thread * sending_thread,
	const void * buffer, size_t length, bool * block);

/** @} */

#endif

