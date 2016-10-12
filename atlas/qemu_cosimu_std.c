#include "atlas/qemu_atlas.h"
#include "atlas/qemu_cosimu.h"
#include "atlas/qemu_cosimu_timer.h"

int qemu_cosimu_get_connect_fd(int port)
{
	// atlas_configs.fd
	return 0;
}

void qemu_cosimu_io_add_watch_poll(int id, IOatlasCanReadHandler fd_can_read,IOatlasFunc fd_read,void* user_data)
{
//     atlas_configs.fd_read = fd_read;
//     atlas_configs.fd_can_read = fd_can_read;
//     atlas_configs.ctx = user_data;
}

uint8_t qemu_cosimu_set_watch_callback(int port,GIOCondition cond, GIOFunc fct, void* ud)
{
//    atlas_configs.tty[port].w.allocated = 1;
//    atlas_configs.tty[port].w.flags	= cond;
//    atlas_configs.tty[port].w.fct	= fct;
//    atlas_configs.tty[port].w.ud 	= ud;
    return 1;
}

void qemu_cosimu_parse(const char * args)
{
//char* ptr=NULL;
//char* base_largs,*largs = NULL;
//int param_type = 0;
//int offset;
//
//	base_largs = largs = (char*)malloc(strlen(args)+2);
//	strcpy(largs,args);
//	largs[strlen(args)]=',';
//	offset = strlen(largs);
//
//	do {
//		ptr = strchr(largs,',');
//		if(ptr)
//		{
//			ptr = strndup(largs,offset-strlen(ptr));
//			switch(param_type)
//			{
//			case 0:
//				atlas_configs.mode = (unsigned char)atoi(ptr);
//				param_type++;
//				break;
//			case 1:
//				atlas_configs.port = (unsigned int)atoi(ptr);
//				param_type++;
//				break;
//			default:
//				break;
//			}
//			largs += strlen(ptr)+1;
//			offset = strlen(largs);
//			free(ptr);
//		}
//	}while(ptr);
//
//	free(base_largs);
//
//	if(param_type < 2)
//	{
//               	fprintf(stderr, "Failed to get atlas arguments input=%s\n", optarg);
//		exit(1);
//	}
}

// main_loop.c
void qemu_cosimu_main_loop_wait(int timeout)
{
//fd_set rxfds;
//struct timeval tv;
//int fd = atlas_configs.fd;
////int ret=0;
//uint8_t count=0;
//fwMsg_header h;
//
//    qemu_cosimu_clock_synch();
//
//    // Check Watch - UART0
//    if(atlas_configs.tty[0].w.allocated && (atlas_configs.tty[0].w.flags & G_IO_OUT))
//    {
//	atlas_configs.tty[0].w.allocated = 0;
//	atlas_configs.tty[0].w.fct(NULL,atlas_configs.tty[0].w.flags,atlas_configs.tty[0].w.ud);
//    }
//
//    do
//    {
//      tv.tv_sec 	= timeout / 1000;
//      tv.tv_usec 	= (timeout % 1000) * 1000;
//
//      FD_CLR(fd,&rxfds);
//      FD_ZERO(&rxfds);
//      FD_SET(fd,&rxfds);
//
//      qemu_mutex_unlock_iothread();
//      //ret = select(FD_SETSIZE, &rxfds, NULL, NULL, &tv);
//      select(FD_SETSIZE, &rxfds, NULL, NULL, &tv);
//      qemu_mutex_lock_iothread();
//
//      if(FD_ISSET(fd, &rxfds))
//      {
//	uint32_t len,datalen=0;
//
//	if(!(atlas_configs.DataToRead))
//	{
//		// Get Framework message header
//		qemu_com_socket_receive(fd,sizeof(fwMsg_header),(uint8_t*)&h);
//		switch(h.subProcess)
//		{
//		case ENTITY_SYNC: // tester Synch message
//			if (h.msgId == E_EXEC_STDIO_READ_REQ)
//			{
//				datalen = h.len - sizeof(fwMsg_header);
//				atlas_configs.DataToRead = datalen;
//			}
//			break;
//		case ENTITY_SUT_MODEM:
//			qemu_cosimu_modem_at_msghdl(h);
//			break;
//		case ENTITY_SUT_QMI:
//			qemu_cosimu_qmi_msghdl(h);
//			break;
//		case ENTITY_SUT_SIM1:
//			qemu_cosimu_sim_msghdl(&h);
//			break;
//		default:
//			break;
//   		}
//	}
//	else datalen = atlas_configs.DataToRead;
//
//	// Dispatch according to the src
//	atlas_configs.fd_can_read(atlas_configs.ctx);
//	len = atlas_configs.fd_read(atlas_configs.ctx,datalen);
//	if(len > 0) atlas_configs.DataToRead -= len;
//      }
//      else count = 0;
//    }while(count || (atlas_configs.DataToRead));

}

bool qemu_cosimu_uart_data_push(int port, const void* data, int size)
{
//uint8_t* body=NULL;
//uint32_t len_out=0;
//
//    body = qemu_cosimu_stdio_serialize(data,size,&len_out);
//
//    if(body)
//    {
//	qemu_com_socket_send(atlas_configs.fd,body,len_out);
//   	free(body);
//	return true;
//    }
//    else return false;
	return true;
}

int64_t qemu_cosimu_clock_val(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000000LL + (tv.tv_usec * 1000);
}
