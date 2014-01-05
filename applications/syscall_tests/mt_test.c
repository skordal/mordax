// The Mordax Microkernel OS Multithreading Test Programme
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <stdint.h>
#include <mordax.h>

// Size of the application image:
extern void * image_size;

static uint32_t thread1_stack[64];
static uint32_t thread2_stack[64];
static uint32_t thread3_stack[64];

static void thread1(void)
{
	mordax_system(MORDAX_SYSTEM_DEBUG, "Hello from thread 1");
	mordax_thread_exit(1);
}

static void thread2(void)
{
	mordax_system(MORDAX_SYSTEM_DEBUG, "Hello from thread 2");
	mordax_thread_exit(2);
}

static void thread3(void)
{
	mordax_system(MORDAX_SYSTEM_DEBUG, "Hello from thread 3");
	mordax_thread_exit(3);
}

int main(void)
{
	tid_t tids[3];
	mordax_system(MORDAX_SYSTEM_DEBUG, "Mordax Multithreading Test Application");

	// Create the three threads:
	mordax_system(MORDAX_SYSTEM_DEBUG, "Creating threads...");
	tids[0] = mordax_thread_create(thread1, thread1_stack);
	tids[1] = mordax_thread_create(thread2, thread2_stack);
	tids[2] = mordax_thread_create(thread3, thread3_stack);

	// Wait for the threads:
	mordax_system(MORDAX_SYSTEM_DEBUG, "Waiting for threads to finish...");
	for(int i = 0; i < 3; ++i)
		mordax_thread_join(tids[i]);

	// Create a new instance of this process:
	struct mordax_process_info newproc = {
		.entry_point = (void *) 0x1000,
		.gid = 0,
		.uid = 0,
		.permissions = MORDAX_PROCESS_INHERIT_PERMISSIONS,
		.stack_length = MORDAX_PROCESS_INHERIT_STACK_SIZE,
		.source = (void *) 0x1000,
		.source_length = (size_t) &image_size,
		.text_length = ((size_t) &image_size + 0x1000 - 1) & -0x1000,
	};
	mordax_system(MORDAX_SYSTEM_DEBUG, "Creating new process instance...");
	volatile pid_t newpid = mordax_process_create(&newproc);
	if(newpid != -1)
		mordax_system(MORDAX_SYSTEM_DEBUG, "New process instance created successfully");
	else
		mordax_system(MORDAX_SYSTEM_DEBUG, "Process creation failed!");

	return 0;
}

