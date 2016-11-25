/*
    ---------------------------------------------------------------------
	QEMU <-> ATLAS AAL structure and fucntions pointer + main entry point
    ---------------------------------------------------------------------
*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "qemu/osdep.h"
#include "qemu/error-report.h"

#include "fwAALmain.h"
#include "atlas/qemu_atlas.h"

extern void exec_main_loop(bool toExecute);
extern void ConfigIns_GetValue(const char* Id, char* current, uint32_t *Len);

void qemuAtlas_main_protect(void);
void qemuAtlas_main_unprotect(void);
void qemuAtlas_main_tickHandler(void);
void qemuAtlas_main_supTask(void);
void qemuAtlas_main_idleTask(void);
void qemuAtlas_main_parse(void);
void qemuAtlas_main_init(void);

sAALmain aSut =
{
    &qemuAtlas_main_init,    		// QEMU initialization procedure. called inside the AAL main entry point
    &qemuAtlas_main_parse,   	// QEMU main loop. called inside the AAL on timer function
    &qemuAtlas_main_protect, 		// QEMU unprotect critical section API
    &qemuAtlas_main_unprotect,   	// QEMU unprotect critical section API
    &qemuAtlas_main_tickHandler,  	// QEMU RTC event handler
    &qemuAtlas_main_supTask,      	// QEMU Super Task entry point
    &qemuAtlas_main_idleTask,	    	// QEMU background task entry point
    eMAIN_KERNEL_SUP,       	// SUT kernel initialization type (start the super (root))
};

void qemuAtlas_main_protect(void)
{
    //not handled now
}

void qemuAtlas_main_unprotect(void)
{
    //not handled now
}

void qemuAtlas_main_tickHandler(void)
{
    //not handled now
}

void qemuAtlas_main_supTask(void)
{
    //not needed
}

void qemuAtlas_main_idleTask(void)
{
    //not needed
}

void qemuAtlas_main_parse(void)
{
    exec_main_loop(true);
}

static void print_command(char* args[100],int nb)
{
    int i=0;
    for(i=0;i<nb && i<100;i++)
    {
        error_report("arg[%.2d] = '%s' ,", i,args[i]);
    }
    error_report("\n");
}

static void qemuAtlas_read_config( const char* tag, const char* option, char* args[100], int *count)
{
    char buf[1024]={0};
    uint32_t size;
    int idx=*count;

    if(tag) ConfigIns_GetValue(tag,buf,&size);
	else return;

    if(size)
    {
        if (option && (strlen(option)>0))
        {
            args[idx] = strdup(option);
            idx++;
        }

        args[idx] = (char*)malloc(size);
        if(args[idx] != NULL)
        {
            memcpy(args[idx],buf,size);
        }
        idx++;
    }
    *count = idx;
}

void qemuAtlas_main_init(void)
{
char *args[100];
char buf[1024]={0};
uint32_t size;
int count=0;

    qemuAtlas_read_config("CORE1_SUT_PATH","",args,&count);
    qemuAtlas_read_config("CONFIG_QEMU_KERNEL","-kernel",args,&count);
    qemuAtlas_read_config("CONFIG_QEMU_HDA","-hda",args,&count);

    memset(buf,0,1024);
    buf[0] = '\0';
    ConfigIns_GetValue("CONFIG_QEMU_NOGRAPHIC",buf,&size);
    if(size)
    {
        if (strcmp(buf,"1")==0)
        {
            args[count] = strdup("-nographic");
            count++;
        }
    }

    qemuAtlas_read_config("CONFIG_QEMU_ECHR","-echr",args,&count);
    qemuAtlas_read_config("CONFIG_QEMU_MACHINE","-M",args,&count);
    qemuAtlas_read_config("CONFIG_QEMU_MEMORY","-m",args,&count);
    qemuAtlas_read_config("CONFIG_QEMU_DRIVE","-drive",args,&count);
    qemuAtlas_read_config("CONFIG_QEMU_NET","-net",args,&count);
    qemuAtlas_read_config("CONFIG_QEMU_NETDEV","-netdev",args,&count);
    qemuAtlas_read_config("CONFIG_QEMU_PIDFILE","-pidfile",args,&count);
    qemuAtlas_read_config("CONFIG_QEMU_APPEND","-append",args,&count);
    qemuAtlas_read_config("CONFIG_QEMU_SERIAL","-serial",args,&count);
    qemuAtlas_read_config("CONFIG_QEMU_OPTION_ROM","-option-rom",args,&count);
    qemuAtlas_read_config("CONFIG_QEMU_DTB","-dtb",args,&count);
    qemuAtlas_read_config("CONFIG_QEMU_DEBUG_GDB",NULL,args,&count);
    qemuAtlas_read_config("CONFIG_QEMU_CPU_STOP",NULL,args,&count);

    args[count] = strdup("-atlas");
	count++;

    print_command(args,count);

    qemuAtlas_init();

    main_qemu(count,(char**)&args,NULL);


    do
    {
        count--;
        free(args[count]);
    } while (count>0);
}
