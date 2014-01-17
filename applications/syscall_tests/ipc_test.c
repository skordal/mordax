// The Mordax Microkernel OS Memory Mapping Test Programme
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <stdint.h>

#include <mordax.h>
#include <mordax-ipc.h>

// Size of the application image:
extern void * image_size;

static uint32_t test_thread_stack[256];

void test_thread(void)
{
	mordax_resource_t socket = mordax_service_connect("/test-service", 13);
	while(socket < 0)
	{
		mordax_system(MORDAX_SYSTEM_DEBUG, "Error connecting, trying again...");
		socket = mordax_service_connect("/test-service", 13);
	}

	mordax_system(MORDAX_SYSTEM_DEBUG, "Sending test message...");
	mordax_socket_send(socket, "TEST", 4);

	mordax_system(MORDAX_SYSTEM_DEBUG, "Receiving test reply...");
	char reply_buffer[4];
	mordax_socket_receive(socket, reply_buffer, 4);
	if(reply_buffer[0] == 'T' && reply_buffer[1] == 'S' && reply_buffer[2] == 'E' && reply_buffer[3] == 'T')
		mordax_system(MORDAX_SYSTEM_DEBUG, "Reply is correct");
	else
		mordax_system(MORDAX_SYSTEM_DEBUG, "Incorrect reply received");

	mordax_system(MORDAX_SYSTEM_DEBUG, "Closing socket and exiting client thread...");
	mordax_resource_destroy(socket);
	mordax_thread_exit(0);
}

int main(void)
{
	mordax_system(MORDAX_SYSTEM_DEBUG, "Mordax IPC Test Application");

	mordax_system(MORDAX_SYSTEM_DEBUG, "Creating service /test-service...");
	mordax_resource_t test_service = mordax_service_create("/test-service", 13);

	// Spawn client thread:
	mordax_thread_create(test_thread, test_thread_stack + 252);

	// Listen on the service:
	mordax_system(MORDAX_SYSTEM_DEBUG, "Listening on service...");
	mordax_resource_t socket = mordax_service_listen(test_service);

	// Receive the test message:
	mordax_system(MORDAX_SYSTEM_DEBUG, "Receiving test message...");
	char test_buffer[4];
	mordax_socket_receive(socket, test_buffer, 4);

	if(test_buffer[0] == 'T' && test_buffer[1] == 'E' && test_buffer[2] == 'S' && test_buffer[3] == 'T')
		mordax_system(MORDAX_SYSTEM_DEBUG, "Test message is correct");
	else
		mordax_system(MORDAX_SYSTEM_DEBUG, "Incorrect test message received");

	// Send test reply:
	mordax_system(MORDAX_SYSTEM_DEBUG, "Sending test reply...");
	mordax_socket_send(socket, "TSET", 4);

	// Close socket:
	mordax_system(MORDAX_SYSTEM_DEBUG, "Closing socket...");
	mordax_resource_destroy(socket);

	// Destroy the service:
	mordax_system(MORDAX_SYSTEM_DEBUG, "Destroying service...");
	mordax_resource_destroy(test_service);

	mordax_system(MORDAX_SYSTEM_DEBUG, "Finished.");
	return 0;
}


