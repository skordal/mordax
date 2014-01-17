@ The Mordax System Call Library
@ (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/mordax/issues>
.syntax unified
.arm

#include <private/syscalls.h>

.section .text

@ System call wrapper macro, calls the specified system call and returns.
.macro syscall_wrapper syscall_name, syscall_num
.global \syscall_name
.type \syscall_name, %function
\syscall_name:
	svc \syscall_num
	bx lr
.endm

syscall_wrapper mordax_system, #MORDAX_SYSCALL_SYSTEM

syscall_wrapper mordax_thread_exit, #MORDAX_SYSCALL_THREAD_EXIT
syscall_wrapper mordax_thread_create, #MORDAX_SYSCALL_THREAD_CREATE
syscall_wrapper mordax_thread_join, #MORDAX_SYSCALL_THREAD_JOIN
syscall_wrapper mordax_thread_yield, #MORDAX_SYSCALL_THREAD_YIELD
syscall_wrapper mordax_thread_info, #MORDAX_SYSCALL_THREAD_INFO

syscall_wrapper mordax_process_create, #MORDAX_SYSCALL_PROCESS_CREATE

syscall_wrapper mordax_memory_map, #MORDAX_SYSCALL_MAP
syscall_wrapper mordax_memory_map_alloc, #MORDAX_SYSCALL_MAP_ALLOC
syscall_wrapper mordax_memory_unmap, #MORDAX_SYSCALL_UNMAP

syscall_wrapper mordax_service_create, #MORDAX_SYSCALL_SERVICE_CREATE
syscall_wrapper mordax_service_listen, #MORDAX_SYSCALL_SERVICE_LISTEN
syscall_wrapper mordax_service_connect, #MORDAX_SYSCALL_SERVICE_CONNECT

syscall_wrapper mordax_socket_send, #MORDAX_SYSCALL_SOCKET_SEND
syscall_wrapper mordax_socket_receive, #MORDAX_SYSCALL_SOCKET_RECEIVE

syscall_wrapper mordax_resource_destroy, #MORDAX_SYSCALL_RESOURCE_DESTROY

