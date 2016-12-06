/*****************************************************************************\
*                                                                             *
* Copyright (C) CEVA Inc. All rights reserved.                                *
*                                                                             *
* Project: Debugger                                                           *
*                                                                             *
* File Name: GUIDEFS.H                                                        *
*                                                                             *
* Written by: Menachem Stern                                                  *
* Date:       11/3/2001                                                       *
*                                                                             *
* Purpose: Common GUI related type definitions use throughout the Debugger's  *
*          modules.                                                           *
*                                                                             *
\*****************************************************************************/

#ifndef COMMON_GUI_TYPE_DEFINITIONS_H_
#define COMMON_GUI_TYPE_DEFINITIONS_H_


// Working indication type enumerations (for toggling cursors):
//-----------------------------------------------------------------------------
typedef enum T_WorkingIndication_tag {
    WorkingIndication_E_Idle,
    WorkingIndication_E_Running,
    WorkingIndication_E_Busy
} T_WorkingIndication;


// Command Window text types enumerations:
//-----------------------------------------------------------------------------
typedef enum CmdWnd_T_CommandTypes_tag {
    CmdWnd_E_Command,
    CmdWnd_E_Comment, // like command
    CmdWnd_E_Response,
    CmdWnd_E_ResponseInfo
} CmdWnd_T_CommandTypes;


// Message display type enumerations:
//-----------------------------------------------------------------------------
typedef enum T_MessageDisplayTypes_tag {
    MsgDisp_E_Assert,
    MsgDisp_E_Fatal,
    MsgDisp_E_Error,
    MsgDisp_E_Warning,
    MsgDisp_E_WarningDontHalt,
    MsgDisp_E_Debug,
    MsgDisp_E_Info,
    MsgDisp_E_DlgBox,
    MsgDisp_E_RtlBug,
    MsgDisp_E__Size
} T_MessageDisplayTypes;


#endif /* COMMON_GUI_TYPE_DEFINITIONS_H_ */
/*================================== E N D ==================================*/
