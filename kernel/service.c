// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "context.h"
#include "debug.h"
#include "mm.h"
#include "rbtree.h"
#include "scheduler.h"
#include "service.h"
#include "utils.h"

#include "api/errno.h"

// Table of all registered services:
static struct rbtree * service_table;

// Frees a service structure:
static void service_free(struct service * svc);

void services_initialize(void)
{
	service_table = rbtree_new((rbtree_key_compare_func) strcmp, mm_free,
		(rbtree_key_duplicate_func) strdup, (rbtree_data_free_func) service_free);
}

struct service * service_lookup(const char * name)
{
	return rbtree_get_value(service_table, name);
}

struct service * service_create(const char * name, struct process * owner)
{
	if(rbtree_key_exists(service_table, name))
		return 0;

	struct service * retval = mm_allocate(sizeof(struct service), MM_DEFAULT_ALIGNMENT,
		MM_MEM_NORMAL);
	retval->name = mm_allocate(strlen(name) + 1, MM_DEFAULT_ALIGNMENT,
		MM_MEM_NORMAL);
	memcpy(retval->name, name, strlen(name) + 1);
	retval->owner = owner;
	retval->listening_thread = 0;
	retval->backlog = queue_new();

	rbtree_insert(service_table, name, retval);
	return retval;
}

void service_destroy(struct service * svc)
{
	// Causes service_free to be called on the service if found:
	rbtree_delete(service_table, svc->name);
}

int service_listen(struct service * svc, struct thread * listener, 
	struct socket ** server_socket, bool * blocking)
{
	*blocking = false;
	*server_socket = 0;

	if(svc->listening_thread != 0)
		return -EBUSY;

	if(svc->backlog->elements > 0)
	{
		struct thread * client_thread = 0;
		if(!queue_remove_front(svc->backlog, (void **) &client_thread))
			return -EINTERNAL;
		struct socket * client_socket = socket_create(client_thread);

		*server_socket = socket_create(listener);
		socket_connect(client_socket, *server_socket);

		mordax_resource_t client_retval = process_add_resource(client_thread->parent,
			PROCESS_RESOURCE_SOCKET, client_socket);
		context_set_syscall_retval(client_thread->context, (void *) client_retval);
		scheduler_move_thread_to_running(client_thread);

		return 0;
	} else {
		svc->listening_thread = listener;
		*blocking = true;
		return 0;
	}
}

int service_connect(struct service * svc, struct thread * connecting_thread,
	struct socket ** client_socket, bool * blocking)
{
	*blocking = false;
	*client_socket = 0;

	if(svc->listening_thread != 0)
	{
		struct socket * server_socket  = socket_create(svc->listening_thread);
		*client_socket = socket_create(connecting_thread);

		socket_connect(*client_socket, server_socket);

		// Wake up the listening thread and return the new socket:
		mordax_resource_t server_retval = process_add_resource(svc->listening_thread->parent,
			PROCESS_RESOURCE_SOCKET, server_socket);
		context_set_syscall_retval(svc->listening_thread->context, (void *) server_retval);
		scheduler_move_thread_to_running(svc->listening_thread);
		svc->listening_thread = 0;

		return 0;
	} else {
		queue_add_back(svc->backlog, connecting_thread);
		*blocking = true;
		return 0;
	}
}

static void service_free(struct service * svc)
{
	// If a thread is currently listening on this socket, release it with an error:
	if(svc->listening_thread != 0)
	{
		context_set_syscall_retval(svc->listening_thread->context, (void *) -ECANCELED);
		scheduler_move_thread_to_running(svc->listening_thread);
	}

	mm_free(svc->name);
	mm_free(svc);
}

