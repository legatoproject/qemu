/*
    -------------------------------------------------------
	QEMU <-> ATLAS Sockets APIs prototypes de definitions
    -------------------------------------------------------
*/

#ifndef QEMU_COM_H
#define QEMU_COM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int qemu_com_socket_receive(int, uint32_t, uint8_t*);
void qemu_com_socket_send(int socket, const void* Msg, int size);

#ifdef __cplusplus
}
#endif

#endif /* QEMU_COM_H */
