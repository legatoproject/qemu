/*
    --------------------------------
	QEMU <-> ATLAS Adaptation Layer
    --------------------------------
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <netinet/tcp.h>

#include "qemu/osdep.h"
#include "qemu-common.h"
#include "qemu/main-loop.h"

#include "atlas/qemu_atlas.h"
#include "atlas/qemu_com.h"
#include "atlas/qemu_smd.h"

#include "fwAALmain.h"
#include "fwmsg.h"
#include "fwNet.h"
#include "fwTraceDef.h"
#include "clkclt.h"
#include "fwModemDef.h"
#include "fwQmiDef.h"

int qemuAtlas_get_connect_fd(int port);
void qemuAtlas_io_add_watch_poll(int id, IOatlasCanReadHandler fd_can_read,IOatlasFunc fd_read,void* user_data);
uint8_t qemuAtlas_set_watch_callback(int port,GIOCondition cond, GIOFunc fct, void* ud);
void qemuAtlas_parse_args(const char * args);
void qemuAtlas_main_loop_wait(int timeout);
int64_t qemuAtlas_clock_val(void);
bool qemuAtlas_uart_data_push(int port, const void* data, int size);

QAtlasBackend qemuAtlas_backend =
{
    &qemuAtlas_main_atlas,
    &qemuAtlas_main_loop_wait,
    &qemuAtlas_uart_data_push,
    &qemuAtlas_io_add_watch_poll,
    &qemuAtlas_set_watch_callback,
    &qemuAtlas_get_connect_fd
};

struct sSockWatch
{
   void*        ud;
   GIOFunc     fct;
   int 		fd;
   GIOCondition flags;
   uint8_t 	allocated;
};

struct sMemBlock
{
   uint8_t* buf;
   uint32_t len;
};

struct sCtx_tty
{
   struct sSockWatch w;
   struct sMemBlock tx;
};

struct atlas_config {
   unsigned char mode;
   unsigned int port;
   int fd;
   int sock_clock;
   uint8_t IsConnected;
   IOatlasCanReadHandler fd_can_read;
   IOatlasFunc fd_read;
   uint32_t DataToRead;
   struct sCtx_tty tty[2];
   void* ctx;
   int at_smd_ch;
};

static struct atlas_config atlas_configs={0};
static int64_t tick=0;

struct qemu_smd_ch
{
    uint32_t state;
    fcPTR    notify;
    void*    ctx;
};

#define SMD_CHANNELS             64

#define SMD_SS_CLOSED            0x00
#define SMD_SS_OPENING           0x01
#define SMD_SS_OPENED            0x02
#define SMD_SS_FLUSHING          0x03
#define SMD_SS_CLOSING           0x04
#define SMD_SS_RESET             0x05
#define SMD_SS_RESET_OPENING     0x06

static struct qemu_smd_ch qemu_smd_list[SMD_CHANNELS];
static int sim_ti;

static uint8_t* qemuAtlas_qmi_serialize(const uint8_t*in, int len, uint32_t* len_out, int ch);

/**/
/**/
static uint8_t* qemuAtlas_stdio_serialize(const uint8_t*in, int len, uint32_t* len_out)
{
fwMsg msg;
fwMsg 	*s;
uint8_t *body;

    msg.header.len 	= len+sizeof(fwMsg_header);
    msg.header.sender 	= PROCESS_CORE1;
    msg.header.process	= PROCESS_SCENARIO;
    msg.header.subProcess = ENTITY_SLINK1;
    msg.header.msgId 	= E_EXEC_STDIO_WRITE_REQ;
    msg.header.msgType 	= 1;
    msg.header.Ti 	= 1;

    body = (uint8_t*)malloc(msg.header.len);
    s = (fwMsg*)(body);
    s->header.len        = msg.header.len;
    s->header.sender     = msg.header.sender;
    s->header.process    = msg.header.process;
    s->header.subProcess = msg.header.subProcess;
    s->header.msgId      = msg.header.msgId;
    s->header.msgType    = msg.header.msgType;
    s->header.Ti	 = msg.header.Ti;

    memcpy((body)+sizeof(s->header),in,len);
    *len_out = msg.header.len;

return body;
}

/**/
/**/
static uint8_t* qemuAtlas_modem_at_serialize(const uint8_t*in, int len, int* len_out)
{
fwMsg msg;
fwMsg 	*s;
uint8_t *body;

    msg.header.len 	= len+sizeof(fwMsg_header);
    msg.header.sender 	= PROCESS_CORE1;
    msg.header.process	= PROCESS_SCENARIO;
    msg.header.subProcess = ENTITY_MODEMVC;
    msg.header.msgId 	= E_MODEM_CMD;
    msg.header.msgType 	= MSG_TYPE_EVT;
    msg.header.Ti 	= 0;

    body = (uint8_t*)malloc(msg.header.len);
    s = (fwMsg*)(body);
    s->header.len        = msg.header.len;
    s->header.sender     = msg.header.sender;
    s->header.process    = msg.header.process;
    s->header.subProcess = msg.header.subProcess;
    s->header.msgId      = msg.header.msgId;
    s->header.msgType    = msg.header.msgType;
    s->header.Ti	 = msg.header.Ti;

    memcpy((body)+sizeof(s->header),in,len);
    *len_out = msg.header.len;

return body;
}

/**/
/**/
static void qemuAtlas_memblock_alloc(struct sMemBlock* mem,uint32_t size)
{
    if(!mem->buf)
    {
	mem->buf = (uint8_t*)malloc(size);
    	mem->len = 0;
    }
    else
	mem->buf = (uint8_t*)realloc(mem->buf,mem->len+size+1);
}

/**/
/* REMOVE NOT USED*/
/*static void qemuAtlas_memblock_free(struct sMemBlock* mem)
{
    free(mem->buf); mem->buf = NULL;
    mem->len = 0;
}*/

/**/
/**/
static void qemuAtlas_update_tester_socket(int tester_soc)
{
    atlas_configs.fd = tester_soc;
    atlas_configs.IsConnected = 1;
}

/**/
/**/
static void qemuAtlas_smd_set_atchannel(int ch)
{
    atlas_configs.at_smd_ch = ch;
}

/**/
/**/
static void qemuAtlas_smd_open(int ch,fcPTR fct,void* ctx)
{
    qemu_smd_list[ch].state = SMD_SS_OPENED;
    qemu_smd_list[ch].notify = fct;
    qemu_smd_list[ch].ctx = ctx;
}

/**/
/**/
static void qemuAtlas_smd_close(int ch)
{
    qemu_smd_list[ch].state = SMD_SS_CLOSED;
    qemu_smd_list[ch].notify = NULL;
}

/**/
/**/
static uint8_t qemuAtlas_is_smd_open(int ch)
{
    if(qemu_smd_list[ch].state == SMD_SS_OPENED) return 1;

    return 0;
}

/**/
/**/
static int qemuAtlas_smd_at_send(int ch,uint8_t* buf,int size)
{
    if( qemuAtlas_is_smd_open(ch) )
    {
		uint8_t* body;
		int len_out=0;
		body = qemuAtlas_modem_at_serialize(buf,size,&len_out);
    	qemu_com_socket_send(atlas_configs.fd,body,len_out);
		free(body);
		return size;
    }
    else return -1;
}

/**/
/**/
static int qemuAtlas_smd_qmi_send(int ch,uint8_t* in,int size)
{
    if( qemuAtlas_is_smd_open(ch) )
    {
		int i=0;
		uint8_t* body;
		uint32_t len_out=0;

		fprintf(stderr,"RX QMI Frame . Len %d. Channel SMD %d\n",size,ch);
		for(i=0;i<size;i++)
			fprintf(stderr,"%02X ",in[i]);
		fprintf(stderr,"\n");
		fflush(stderr);

		// Send to ATLAS Tester
		body = qemuAtlas_qmi_serialize(in,size,&len_out,ch);
 	   	qemu_com_socket_send(atlas_configs.fd,body,len_out);
		free(body);
		return size;
    }
    else
    {
		fprintf(stderr,"RX QMI Frame . Len %d. Channel SMD %d. But channel not openned !!!!!!!!!!!!\n",size,ch);
		return -1;
    }
}

/**/
/**/
static int qemuAtlas_smd_send(int ch,uint8_t* buf,int size)
{
	if (ch == atlas_configs.at_smd_ch) {
		return qemuAtlas_smd_at_send (ch,buf,size);
	} else {
		return qemuAtlas_smd_qmi_send (ch,buf,size);
	}
}

/**/
/**/
static int64_t qemuAtlas_clock_set_val(int64_t val)
{
     tick = val;
     return 1;
}

/* REMOVE NOT USED by qemuAtlas_clock_synch*/
/*typedef enum
{
	MyNONE,
	myACK,
	mySYNCH,
};*/

/* REMOVE NOT USED by qemuAtlas_clock_synch*/
/*typedef struct fwclock
{
	int64_t val;
	uint32_t type;
}fwclock;*/

/**/
/**/
static void qemuAtlas_clock_synch(void)
{
/*
fwclock c;

	qemuAtlas_socket_receive(atlas_configs.sock_clock,sizeof(fwclock),&c);
	if(c.type != mySYNCH)
	{
		fprintf(stderr,"Error: Bad synck msg");
		exit(1);
	}
	tick = c.val;

	c.type = myACK;
	qemuAtlas_socket_send(atlas_configs.sock_clock,&c,sizeof(fwclock));
*/

struct timeval tv;
int64_t val;

    gettimeofday(&tv, NULL);
    val = (tv.tv_sec) * 1000000000LL + (tv.tv_usec * 1000);
    qemuAtlas_clock_set_val(val);
}

/**/
/**/
static void qemuAtlas_send_SimRelay(E_SUBPROCESS sub,E_TYPE type, fwMsgType msgType,int Ti, void *data, int len )
{
//char name[255]  = {0};
uint8_t * body;
fwMsg  *s;
fwMsg* msgSend = fwMsgNew();

	msgSend->header.msgId 	   = msgType;
	msgSend->header.process    = PROCESS_SCENARIO;
	msgSend->header.sender     = PROCESS_CORE1;
	msgSend->header.subProcess = sub;
	msgSend->header.msgType    = type;
	msgSend->header.Ti         = Ti;
	msgSend->header.len        = len + sizeof(fwMsg_header);

	body = (uint8_t*)malloc(msgSend->header.len);
	if(NULL == body)
	{
	   	PrintTRC("\r\n\t%s:: memory allocation error\r\n",SUT_PROCESS_NAME);
	   	fwMsgDelete(msgSend);
	   	return;
	}

	s = (fwMsg*)(body);
	s->header.len        = msgSend->header.len;
	s->header.sender     = msgSend->header.sender;
	s->header.process    = msgSend->header.process;
	s->header.subProcess = msgSend->header.subProcess;
	s->header.msgId      = msgSend->header.msgId;
	s->header.msgType    = msgSend->header.msgType;
	s->header.Ti	     = msgSend->header.Ti;
	if ( NULL != data )
		memcpy((body)+sizeof(s->header),data,len);

	fwNet_SocketSend(atlas_configs.fd,body,msgSend->header.len);
	fwMsgDelete(msgSend);
	free(body);
}
/**/
/**/
static void qemuAtlas_sim_msghdl(fwMsg_header* h)
{
uint32_t datalen = h->len - sizeof(fwMsg_header);
uint8_t* data = (uint8_t*)malloc(datalen);
uint8_t* cmdId;

    qemu_com_socket_receive(atlas_configs.fd,datalen,data);
    cmdId = (uint8_t*)(data);

    if( (cmdId[0] == SIM_PROACTIVE_CMD) && !sim_ti)
    {
	h->msgId = E_MODEM_PROACTIVE;
	qemuAtlas_send_SimRelay(ENTITY_MODEMVC,MSG_TYPE_EVT,(fwMsgType)h->msgId,sim_ti,data,datalen);
    }
    else
    {
	qemuAtlas_send_SimRelay(ENTITY_MODEMVC,MSG_TYPE_RSP,(fwMsgType)h->msgId,sim_ti,data,datalen);
    }
    sim_ti = 0;
}

static void qemuAtlas_slink1_msghdl(fwMsg_header* h)
{
uint32_t datalen = h->len - sizeof(fwMsg_header);
uint8_t* data = (uint8_t*)malloc(datalen);

    qemu_com_socket_receive(atlas_configs.fd,datalen,data);
	if(data) free(data);
	// TODO : To be implemented
}

/**/
/**/
static void qemuAtlas_modem_at_msghdl(fwMsg_header h)
{
uint32_t datalen = h.len - sizeof(fwMsg_header);
uint8_t* data = (datalen)?(uint8_t*)malloc(datalen):NULL;
int ch = atlas_configs.at_smd_ch;

    memset(data,0,datalen);
    if(datalen) qemu_com_socket_receive(atlas_configs.fd,datalen,data);

    switch(h.msgId)
    {
    case E_MODEM_RSP :
    	if( qemuAtlas_is_smd_open(ch) )
    	{
		if(data) qemu_smd_list[ch].notify(qemu_smd_list[ch].ctx,ch,data,datalen);
		if(data) fprintf(stderr,"data :: %s %d\n",data,datalen);
   	}
	break;
    case E_MODEM_SIM_EXCH_REQ:
		/* Send message to sim */
		qemuAtlas_send_SimRelay(ENTITY_SIM1VC,MSG_TYPE_EVT,(fwMsgType)E_SIM_EXCHANGE,h.Ti,data,datalen);
		/* Send message to modem to unblock response que */
		qemuAtlas_send_SimRelay(ENTITY_MODEMVC,MSG_TYPE_RSP,(fwMsgType)h.msgId,h.Ti,data,datalen);
		break;
    case E_MODEM_SIM_RSTART_REQ:
		qemuAtlas_send_SimRelay(ENTITY_SIM1VC,MSG_TYPE_EVT,(fwMsgType)E_SIM_RESTART,h.Ti,data,datalen);
		/* Send message to modem to unblock response que */
		qemuAtlas_send_SimRelay(ENTITY_MODEMVC,MSG_TYPE_RSP,(fwMsgType)h.msgId,h.Ti,data,datalen);
		break;
    case E_SIM_RESET:
    case E_SIM_EXCHANGE:
    case E_SIM_GET_PARENT:
		/* Send message to sim */
		qemuAtlas_send_SimRelay(ENTITY_SIM1VC,MSG_TYPE_EVT,(fwMsgType)h.msgId,h.Ti,data,datalen);
		sim_ti = h.Ti;
		break;
    default:
	break;
    }
    if(data) free(data);
}

/**/
/**/
static void qemuAtlas_qmi_msghdl(fwMsg_header h)
{
uint32_t datalen = h.len - sizeof(fwMsg_header);
uint8_t* data = (datalen)?(uint8_t*)malloc(datalen):NULL;
int ch = h.Ti;
int i;

    memset(data,0,datalen);
    if(datalen) qemu_com_socket_receive(atlas_configs.fd,datalen,data);

    switch(h.msgId)
    {
    case E_QMI_RSP :
    	if( qemuAtlas_is_smd_open(ch) )
    	{
		if(data) qemu_smd_list[ch].notify(qemu_smd_list[ch].ctx,ch,data,datalen);
		fprintf(stderr,"\n\tTX QMI Frame . Len %d. Channel SMD %d\n",datalen,ch);
		for(i=0;i<datalen;i++)
			fprintf(stderr,"%02X ",data[i]);
		fprintf(stderr,"\n\n");
		fflush(stderr);
   	}
	break;
    default:
	break;
    }
    if(data) free(data);
}

/**/
/**/
static uint8_t* qemuAtlas_qmi_serialize(const uint8_t*in, int len, uint32_t* len_out, int ch)
{
fwMsg msg;
fwMsg 	*s;
uint8_t *body;

    msg.header.len 	= len+sizeof(fwMsg_header);
    msg.header.sender 	= PROCESS_CORE1;
    msg.header.process	= PROCESS_SCENARIO;
    msg.header.subProcess = ENTITY_QMIVC;
    msg.header.msgId 	= E_QMI_CMD;
    msg.header.msgType 	= MSG_TYPE_EVT;
    msg.header.Ti 	= ch;

    body = (uint8_t*)malloc(msg.header.len);
    s = (fwMsg*)(body);
    s->header.len        = msg.header.len;
    s->header.sender     = msg.header.sender;
    s->header.process    = msg.header.process;
    s->header.subProcess = msg.header.subProcess;
    s->header.msgId      = msg.header.msgId;
    s->header.msgType    = msg.header.msgType;
    s->header.Ti	 = msg.header.Ti;

    memcpy((body)+sizeof(s->header),in,len);
    *len_out = msg.header.len;

return body;
}

/**/
/**/
void qemuAtlas_init(void)
{
    memset(&atlas_configs,0,sizeof(struct atlas_config));

    qemu_smd_register(qemuAtlas_smd_open,qemuAtlas_smd_send,qemuAtlas_smd_close,qemuAtlas_smd_set_atchannel);

    /*Init UART0 TX memory*/
    qemuAtlas_memblock_alloc(&atlas_configs.tty[0].tx,1);

    qemuAtlas_update_tester_socket(fwAAL_GetCoreSocket());
    qemuAtlas_send_capabilities(); //to send capability message.
}

/**/
/**/
void qemuAtlas_parse_args(const char * args)
{
char* ptr=NULL;
char* base_largs,*largs = NULL;
int param_type = 0;
int offset;

	base_largs = largs = (char*)malloc(strlen(args)+2);
	strcpy(largs,args);
	largs[strlen(args)]=',';
	offset = strlen(largs);

	do {
		ptr = strchr(largs,',');
		if(ptr)
		{
			ptr = strndup(largs,offset-strlen(ptr));
			switch(param_type)
			{
			case 0:
				atlas_configs.mode = (unsigned char)atoi(ptr);
				param_type++;
				break;
			case 1:
				atlas_configs.port = (unsigned int)atoi(ptr);
				param_type++;
				break;
			default:
				break;
			}
			largs += strlen(ptr)+1;
			offset = strlen(largs);
			free(ptr);
		}
	}while(ptr);

	free(base_largs);

	if(param_type < 2)
	{
               	fprintf(stderr, "Failed to get atlas arguments input=%s\n", optarg);
		exit(1);
	}
}

/**/
/**/
uint8_t qemuAtlas_set_watch_callback(int port,GIOCondition cond, GIOFunc fct, void* ud)
{
    atlas_configs.tty[port].w.allocated = 1;
    atlas_configs.tty[port].w.flags	= cond;
    atlas_configs.tty[port].w.fct	= fct;
    atlas_configs.tty[port].w.ud 	= ud;

return 1;
}

/**/
/**/
bool qemuAtlas_uart_data_push(int port, const void* data, int size)
{
uint8_t* body=NULL;
uint32_t len_out=0;

    body = qemuAtlas_stdio_serialize((const uint8_t*)data,size,&len_out);

    if(body)
    {
	qemu_com_socket_send(atlas_configs.fd,body,len_out);
   	free(body);
	return true;
    }
    else return false;
}

/**/
/* REMOVE NOT USED */
/*void qemuAtlas_uart_flushTx(int port)
{
uint8_t* body;
uint32_t len_out=0;
uint32_t len = atlas_configs.tty[port].tx.len;

    if(len)
    {
	uint8_t* src = atlas_configs.tty[port].tx.buf;

	body = qemuAtlas_stdio_serialize(src,len,&len_out);
	qemuAtlas_memblock_free(&atlas_configs.tty[port].tx);

    	qemuAtlas_socket_send(atlas_configs.fd,body,len_out);
	free(body);

    	qemuAtlas_memblock_alloc(&atlas_configs.tty[port].tx,1);
    }
}*/

/**/
/* REMOVE NOT USED */
/*void qemuAtlas_connect_clockSrv(int port)
{
int socket_connect = 0;
struct sockaddr_in st;

  	socket_connect = socket(AF_INET, SOCK_STREAM, 0);

	bzero((char*)&st, sizeof(st));
 	st.sin_family      = AF_INET;
	st.sin_addr.s_addr = INADDR_ANY;
	st.sin_port        = htons(port);
	while (-1 == connect(socket_connect,(struct sockaddr*) &st,sizeof(struct sockaddr))) sleep (1);
 	atlas_configs.sock_clock = socket_connect;
}*/

/**/
/**/
int qemuAtlas_get_connect_fd(int port)
{
   return atlas_configs.fd;
}

/**/
/**/
void qemuAtlas_main_loop_wait(int timeout)
{
fd_set rxfds;
struct timeval tv;
int fd = atlas_configs.fd;
//int ret=0;
uint8_t count=0;
fwMsg_header h;

    qemuAtlas_clock_synch();

    // Check Watch - UART0
    if(atlas_configs.tty[0].w.allocated && (atlas_configs.tty[0].w.flags & G_IO_OUT))
    {
	atlas_configs.tty[0].w.allocated = 0;
	atlas_configs.tty[0].w.fct(NULL,atlas_configs.tty[0].w.flags,atlas_configs.tty[0].w.ud);
    }

    do
    {
      tv.tv_sec 	= timeout / 1000;
      tv.tv_usec 	= (timeout % 1000) * 1000;

      FD_CLR(fd,&rxfds);
      FD_ZERO(&rxfds);
      FD_SET(fd,&rxfds);

      qemu_mutex_unlock_iothread();
      select(FD_SETSIZE, &rxfds, NULL, NULL, &tv);
      qemu_mutex_lock_iothread();

      if(FD_ISSET(fd, &rxfds))
      {
	      	int len=0;
	      	uint32_t datalen=0;

			if(!(atlas_configs.DataToRead))
			{
			// Get Framework message header
			qemu_com_socket_receive(fd,sizeof(fwMsg_header),(uint8_t*)&h);
			switch(h.subProcess)
			{
			case ENTITY_SYNC: // tester Synch message
			if (h.msgId == E_EXEC_STDIO_READ_REQ)
			{
				datalen = h.len - sizeof(fwMsg_header);
				atlas_configs.DataToRead = datalen;
			}
			break;
			case ENTITY_SUT_MODEM:
			qemuAtlas_modem_at_msghdl(h);
			break;
			case ENTITY_SUT_QMI:
			qemuAtlas_qmi_msghdl(h);
			break;
			case ENTITY_SUT_SIM1:
			qemuAtlas_sim_msghdl(&h);
			break;
			case ENTITY_SUT_SLINK1:
			qemuAtlas_slink1_msghdl(&h);
			break;
			default:
			break;
   			}
		}
		else datalen = atlas_configs.DataToRead;

		// Dispatch according to the src
		atlas_configs.fd_can_read(atlas_configs.ctx);
		len = atlas_configs.fd_read(atlas_configs.ctx,datalen);
		if(len > 0) atlas_configs.DataToRead -= len;
    	}
      	else count = 0;
    }while(count || (atlas_configs.DataToRead));
}

/**/
/**/
void qemuAtlas_io_add_watch_poll(int id, IOatlasCanReadHandler fd_can_read,IOatlasFunc fd_read,void* user_data)
{
     atlas_configs.fd_read = fd_read;
     atlas_configs.fd_can_read = fd_can_read;
     atlas_configs.ctx = user_data;
}

/**/
/**/
int64_t qemuAtlas_clock_val(void)
{
     return tick;
}
