/*
    ---------------------------------------------------
    QEMU SMD backend wrapper prototypes and definitions
    ---------------------------------------------------
*/

#ifndef _QEMU_SMD_H_
#define _QEMU_SMD_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*fcPTR) (void*,int,uint8_t*,int);

typedef void (*smd_open_ptr) (int ch,fcPTR fct,void* ctx);
typedef int (*smd_send_ptr) (int ch,uint8_t* in,int size);
typedef void (*smd_close_ptr) (int ch);
typedef void (*smd_set_atchannel_ptr) (int ch);

void qemu_smd_register(smd_open_ptr,smd_send_ptr,smd_close_ptr,smd_set_atchannel_ptr);

void qemu_smd_backend_open(int ch,fcPTR fct,void* ctx);

int qemu_smd_backend_send(int ch,uint8_t* in,int size);

void qemu_smd_backend_close(int ch);

void qemu_smd_backend_set_atchannel(int ch);

#ifdef __cplusplus
}
#endif

#endif /* _QEMU_SMD_H_ */
