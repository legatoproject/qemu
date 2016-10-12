#include "qemu_atlas.h"
#include "qemu_smd.h"
#include "qemu_cosimu.h"

/**/
/**/
static void qemu_std_open(int ch,fcPTR fct,void* ctx)
{
}

/**/
/**/
static int qemu_std_send(int ch,uint8_t* in,int size)
{
	return 0;
}

/**/
/**/
static void qemu_std_close(int ch)
{
}

/**/
/**/
void qemu_atlas_init(void)
{
	qemu_smd_register(qemu_std_open,qemu_std_send,qemu_std_close);
}

int atlas_main(int argc,char** argv)
{
	printf("Using cosimu standalone, do not use \"-atlas\" option!\n");
	return main_qemu(argc,argv,NULL);
}
