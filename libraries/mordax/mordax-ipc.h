// The Mordax System Call Library
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef __MORDAX_IPC_H__
#define __MORDAX_IPC_H__

#include <mordax/types.h>

/**
 * Creates a new service.
 * @param name name of the service to create.
 * @param name_len length of the name of the service, excluding the terminating
 *                 null character.
 * @return a handle to the service object.
 */
mordax_resource_t mordax_service_create(const char * name, size_t name_len);

/**
 * Listens on a service.
 * @param service handle to the service to listen to.
 * @return handle to the socket of a connecting client.
 */
mordax_resource_t mordax_service_listen(mordax_resource_t service);

/**
 * Connects to a service.
 * @param name name of the service to connect to.
 * @param name_len length of the name of the service, excluding the terminating
 *                 null character.
 * @return a handle to the connected socket.
 */
mordax_resource_t mordax_service_connect(const char * name, size_t name_len);

/**
 * Sends a message on a socket.
 * @param socket the socket to send on.
 * @param buffer buffer containing the message to send.
 * @param length length of the message.
 * @return the number of bytes sent or a negative error number.
 */
int mordax_socket_send(mordax_resource_t socket, const void * buffer, size_t length);

/**
 * Receives a message from a socket.
 * @param socket the socket to receive from.
 * @param buffer destination buffer.
 * @param length length of the destination buffer.
 * @return the number of bytes received or a negative error number.
 */
int mordax_socket_receive(mordax_resource_t socket, void * buffer, size_t length);

/**
 * Frees a resource.
 * @param identifier the resource identifier.
 */
void mordax_resource_destroy(mordax_resource_t identifier);

#endif

