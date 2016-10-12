#include "qemu_smd.h"

struct qemu_smd
{
		smd_open_ptr smd_open;
		smd_send_ptr smd_send;
		smd_close_ptr smd_close;
};

static struct qemu_smd smd = {0};

void qemu_smd_register(smd_open_ptr open_fct,smd_send_ptr send_fct,smd_close_ptr close_fct)
{
	smd.smd_open = open_fct;
	smd.smd_send = send_fct;
	smd.smd_close = close_fct;
}

void qemu_smd_backend_open(int ch,fcPTR fct,void* ctx)
{
	if (smd.smd_open) {
		smd.smd_open(ch,fct,ctx);
	}
}

int qemu_smd_backend_send(int ch,uint8_t* in,int size)
{
	if (smd.smd_send) {
		return smd.smd_send(ch,in,size);
	}
	return -1;
}

void qemu_smd_backend_close(int ch)
{
	if (smd.smd_close) {
		smd.smd_close(ch);
	}
}
