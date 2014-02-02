// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "debug.h"
#include "mm.h"
#include "scheduler.h"
#include "socket.h"
#include "utils.h"

#include "api/errno.h"

struct socket * socket_create(struct thread * owner)
{
	struct socket * retval = mm_allocate(sizeof(struct socket),
		MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	memclr(retval, sizeof(struct socket));

	retval->owner = owner;

	return retval;
}

void socket_destroy(struct socket * sock)
{
	// Remove blocking threads:
	if(sock->blocking_receiver)
	{
		context_set_syscall_retval(sock->blocking_receiver->context, (void *) -ENOTCONN);
		scheduler_move_thread_to_running(sock->blocking_receiver);
	}

	if(sock->blocking_sender)
	{
		context_set_syscall_retval(sock->blocking_sender->context, (void *) -ENOTCONN);
		scheduler_move_thread_to_running(sock->blocking_sender);
	}

	if(sock->blocking_waiter)
	{
		context_set_syscall_retval(sock->blocking_waiter->context, (void *) -ENOTCONN);
		scheduler_move_thread_to_running(sock->blocking_waiter);
	}

	// Disconnect from the endpoint:
	if(sock->endpoint != 0)
		sock->endpoint->endpoint = 0;
	mm_free(sock);
}

bool socket_connect(struct socket * a, struct socket * b)
{
	a->endpoint = b;
	b->endpoint = a;
	return true;
}

int socket_wait(struct socket * sock, struct thread * waiting_thread, bool * block)
{
	*block = false;

	if(sock->endpoint == 0)
		return -ENOTCONN;
	if(sock->endpoint->blocking_receiver != 0)
		return -EBUSY;

	if(sock->endpoint->blocking_sender != 0)
		return sock->endpoint->blocking_details.length;
	else {
		sock->blocking_waiter = waiting_thread;
		*block = true;
		return 0;
	}
}

int socket_receive(struct socket * sock, struct thread * receiving_thread,
	void * buffer, size_t length, bool * block)
{
	*block = false;

	if(sock->endpoint == 0)
		return -ENOTCONN;
	if(length > CONFIG_IPC_BUFFER_LENGTH)
		return -E2BIG;
	if(sock->endpoint->blocking_receiver != 0 || sock->endpoint->blocking_waiter)
		return -EBUSY;

	if(sock->endpoint->blocking_sender != 0)
	{
		memcpy_p(buffer, receiving_thread->parent, sock->endpoint->blocking_details.buffer,
			sock->endpoint->blocking_sender->parent, min(sock->endpoint->blocking_details.length, length));
		scheduler_move_thread_to_running(sock->endpoint->blocking_sender);
		context_set_syscall_retval(sock->endpoint->blocking_sender->context,
			(void *) min(length, sock->endpoint->blocking_details.length));
		sock->endpoint->blocking_sender = 0;
		return min(length, sock->endpoint->blocking_details.length);
	} else {
		sock->blocking_receiver = receiving_thread;
		sock->blocking_details.buffer = (void *) buffer;
		sock->blocking_details.length = length;
		*block = true;
		return 0;
	}
}

int socket_send(struct socket * sock, struct thread * sending_thread,
	const void * buffer, size_t length, bool * block)
{
	*block = false;

	if(sock->endpoint == 0)
		return -ENOTCONN;
	if(length > CONFIG_IPC_BUFFER_LENGTH)
		return -E2BIG;
	if(sock->endpoint->blocking_sender != 0)
		return -EBUSY;

	if(sock->endpoint->blocking_receiver != 0)
	{
		memcpy_p(sock->endpoint->blocking_details.buffer, sock->endpoint->blocking_receiver->parent,
			(void *) buffer, sending_thread->parent, min(length, sock->endpoint->blocking_details.length));
		scheduler_move_thread_to_running(sock->endpoint->blocking_receiver);
		context_set_syscall_retval(sock->endpoint->blocking_receiver->context,
			(void *) min(length, sock->endpoint->blocking_details.length));
		sock->endpoint->blocking_receiver = 0;
		return length;
	} else {
		sock->blocking_sender = sending_thread;
		sock->blocking_details.buffer = (void *) buffer;
		sock->blocking_details.length = length;
		*block = true;

		// If a thread is waiting for a message, release it with the size of the message:
		if(sock->endpoint->blocking_waiter != 0)
		{
			scheduler_move_thread_to_running(sock->endpoint->blocking_waiter);
			context_set_syscall_retval(sock->endpoint->blocking_waiter->context, (void *) length);
			sock->endpoint->blocking_waiter = 0;
		}

		return 0;
	}
}

