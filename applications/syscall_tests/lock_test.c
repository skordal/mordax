// The Mordax Microkernel OS Lock Test Programme
// (c) Kristian Klomsten Skordal <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <mordax.h>

static uint32_t test_thread_stack_1[256];
static uint32_t test_thread_stack_2[256];

mordax_resource_t test_lock;

static void test_thread(void)
{
	mordax_system(MORDAX_SYSTEM_DEBUG, "Attempting to aquire lock...");

	mordax_lock_aquire(test_lock);
	mordax_system(MORDAX_SYSTEM_DEBUG, "Lock aquired!");

	mordax_thread_yield();

	mordax_lock_release(test_lock);
	mordax_system(MORDAX_SYSTEM_DEBUG, "Lock released!");

	mordax_thread_exit(0);
}

int main(void)
{
	mordax_system(MORDAX_SYSTEM_DEBUG, "Mordax Lock Test Application");

	// Create a lock:
	mordax_system(MORDAX_SYSTEM_DEBUG, "Creating test lock...");
	test_lock = mordax_lock_create();

	// Aquire the test lock:
	mordax_system(MORDAX_SYSTEM_DEBUG, "Attempting to aquire test lock...");
	mordax_lock_aquire(test_lock);

	asm volatile("dsb\n\t");

	// Create the test threads:
	mordax_system(MORDAX_SYSTEM_DEBUG, "Creating test threads...");
	tid_t thread_1 = mordax_thread_create(test_thread, test_thread_stack_1);
	tid_t thread_2 = mordax_thread_create(test_thread, test_thread_stack_2);

	// Release the lock, starting one of the threads:
	mordax_system(MORDAX_SYSTEM_DEBUG, "Releasing test lock...");
	mordax_lock_release(test_lock);

	// Join with the test threads:
	mordax_system(MORDAX_SYSTEM_DEBUG, "Joining test threads...");
	mordax_thread_join(thread_1);
	mordax_thread_join(thread_2);

	// Free the lock:
	mordax_system(MORDAX_SYSTEM_DEBUG, "Destroying lock...");
	mordax_resource_destroy(test_lock);
}

