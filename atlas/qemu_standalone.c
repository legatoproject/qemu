/*
 *  QEMU standalone. APIs for using QEMU in standalone mode out of ATLAS framework
 *
 *  Copyright (C) Sierra Wireless Inc.
 *
 */

#include "atlas/qemu_atlas.h"
#include "atlas/qemu_smd.h"

int qemuAtlas_get_connect_fd_stub(int port);
void qemuAtlas_io_add_watch_poll_stub(int id, IOatlasCanReadHandler fd_can_read,IOatlasFunc fd_read,void* user_data);
uint8_t qemuAtlas_set_watch_callback_stub(int port,GIOCondition cond, GIOFunc fct, void* ud);
void qemuAtlas_parse_args_stub(const char * args);
void qemuAtlas_main_loop_wait_stub(int timeout);
int64_t qemuAtlas_clock_val(void);
bool qemuAtlas_uart_data_push_stub(int port, const void* data, int size);

QAtlasBackend qemuAtlas_backend =
{
    &qemuAtlas_main_atlas,
    &qemuAtlas_main_loop_wait_stub,
    &qemuAtlas_uart_data_push_stub,
    &qemuAtlas_io_add_watch_poll_stub,
    &qemuAtlas_set_watch_callback_stub,
    &qemuAtlas_get_connect_fd_stub
};

/**/
/**/
static void qemuAtlas_smd_set_atchannel(int ch)
{;}

/**/
/**/
static void qemuAtlas_smd_open(int ch,fcPTR fct,void* ctx)
{;}

/**/
/**/
static int qemuAtlas_smd_send(int ch,uint8_t* in,int size)
{
    return 0;
}

/**/
/**/
static void QemuAtlas_smd_close(int ch)
{;}

/**/
/**/
void qemuAtlas_init(void)
{
    qemu_smd_register(qemuAtlas_smd_open,qemuAtlas_smd_send,QemuAtlas_smd_close,qemuAtlas_smd_set_atchannel);
}

/**/
/**/
int qemuAtlas_main_atlas(int argc,char** argv)
{
    printf("Using standalone, do not use \"-atlas\" option!\n");
    return main_qemu(argc,argv,NULL);
}

/**/
/**/
int qemuAtlas_get_connect_fd_stub(int port)
{
    return 0;
}

/**/
/**/
void qemuAtlas_io_add_watch_poll_stub(int id, IOatlasCanReadHandler fd_can_read,IOatlasFunc fd_read,void* user_data)
{;}

/**/
/**/
uint8_t qemuAtlas_set_watch_callback_stub(int port,GIOCondition cond, GIOFunc fct, void* ud)
{
    return 1;
}

/**/
/**/
void qemuAtlas_parse_args_stub(const char * args)
{;}

/**/
/**/
void qemuAtlas_main_loop_wait_stub(int timeout)
{;}

/**/
/**/
bool qemuAtlas_uart_data_push_stub(int port, const void* data, int size)
{
    return true;
}

/**/
/**/
int64_t qemuAtlas_clock_val(void)
{
struct timeval tv;

    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000000LL + (tv.tv_usec * 1000);
}
