/*
 *  QEMU <-> ATLAS backend prototypes de definitions
 *
 *  Copyright (C) Sierra Wireless Inc.
 *
 */

#ifndef QEMU_ATLAS_H
#define QEMU_ATLAS_H

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu-common.h"
#include "hw/qdev.h"
#include "sysemu/char.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef int (*IOatlasCanReadHandler)(void*);
    typedef int (*IOatlasFunc)(void*,uint32_t);

    typedef bool (*qemuAtlas_io_push_ptr) (int port, const void* data, int size);
    typedef void (*qemuAtlas_io_add_watch_ptr)(int id, IOatlasCanReadHandler fd_can_read,IOatlasFunc fd_read,void* user_data);
    typedef uint8_t (*qemuAtlas_set_watch_cb_ptr) (int port,GIOCondition cond, GIOFunc fct, void* ud);
    typedef int (*qemuAtlas_get_fd_ptr) (int port);
    typedef void (*qemuAtlas_main_loop_wait_ptr) (int timeout);
    typedef int (*qemuAtlas_main_ptr) (int argc, char **argv);

    int main_qemu(int argc, char **argv, char **envp);
	int qemuAtlas_main_atlas(int argc,char** argv);
    void qemuAtlas_init(void);
    void qemuAtlas_send_capabilities(void);

#ifdef __cplusplus
}
#endif

typedef struct
{
    qemuAtlas_main_ptr main;
    qemuAtlas_main_loop_wait_ptr main_loop_wait_ptr;
    qemuAtlas_io_push_ptr io_push_data;
    qemuAtlas_io_add_watch_ptr io_add_watch;
    qemuAtlas_set_watch_cb_ptr set_watch_cb;
    qemuAtlas_get_fd_ptr get_fd;
}QAtlasBackend;

extern QAtlasBackend* qemu_atlas_backend;

#endif /* QEMU_ATLAS_H */
