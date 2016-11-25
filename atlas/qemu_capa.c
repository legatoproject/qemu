/*
    ------------------------------------------
	QEMU <-> ATLAS capabilities message routing
    ------------------------------------------
*/

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

/**/
/**/
void qemuAtlas_send_capabilities( void )
{
fwCoreCaps sSendData;
fwMsg msg;
fwMsg   *s;
uint8_t *body;
char* prod = getenv("PRODUCT_VARIANT");

    // Build Capability message
    memset(&sSendData,0,sizeof(fwCoreCaps));
    sSendData.uartc.identity    = 0x01;
    sSendData.usbc.identity     = 0x00;
    sSendData.gpioc.identity    = 0x00;
    sSendData.simc.identity     = 0x01;
    sSendData.netc.identity     = 0x00;
    sSendData.dpramc.identity   = 0x00;
    sSendData.audc.identity     = 0x00;
    sSendData.cns.identity      = 0x00;
    sSendData.modem_at.identity = 0x01;
    sSendData.stdio.identity    = 0x01; // Possible values: 1 or 0
    sSendData.pid               = PROCESS_CORE1;
    sSendData.qmi.identity 	= 0x01;

    memcpy(sSendData.name,"QEMU",4);
   	if(prod) memcpy(sSendData.prodName,prod,strlen(prod));
    else memcpy(sSendData.prodName,"none",4);

    /* constructing a reply message to scenario */
    msg.header.len        = sizeof(fwCoreCaps) + sizeof(fwMsg_header);
    msg.header.sender     = PROCESS_CORE1;
    msg.header.process    = PROCESS_SCENARIO;
    msg.header.subProcess = ENTITY_MNGT;
    msg.header.msgId      = E_EXEC_CAPAB;
    msg.header.msgType    = MSG_TYPE_EVT;
    msg.header.Ti         = 0;

    body = (uint8_t*)malloc(msg.header.len);
    s = (fwMsg*)(body);
    s->header.len        = msg.header.len;
    s->header.sender     = msg.header.sender;
    s->header.process    = msg.header.process;
    s->header.subProcess = msg.header.subProcess;
    s->header.msgId      = msg.header.msgId;
    s->header.msgType    = msg.header.msgType;
    s->header.Ti         = msg.header.Ti;
    memcpy((body)+sizeof(s->header),&sSendData,sizeof(fwCoreCaps));

    qemu_com_socket_send(fwAAL_GetCoreSocket(),body,msg.header.len);

    free(body);
}
