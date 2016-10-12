#ifndef QEMU_COSIMU_H
#define QEMU_COSIMU_H

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu-common.h"
#include "hw/qdev.h"
#include "sysemu/char.h"

#ifdef __cplusplus
extern "C" {
#endif

// qemu_char.c
typedef int (*IOatlasCanReadHandler)(void*);
typedef int (*IOatlasFunc)(void*,uint32_t);

int qemu_cosimu_get_connect_fd(int);
void qemu_cosimu_io_add_watch_poll(int, IOatlasCanReadHandler,IOatlasFunc,void*);
uint8_t qemu_cosimu_set_watch_callback(int port,GIOCondition cond, GIOFunc fct, void* ud);
bool qemu_cosimu_uart_data_push(int port, const void* data, int size);

void qemu_cosimu_parse(const char * args);

// main_loop.c
void qemu_cosimu_main_loop_wait(int);

#ifdef __cplusplus
}
#endif

#endif /* QEMU_COSIMU_H */
