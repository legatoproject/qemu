/*****************************************************************************\
*                                                                             *
* Copyright (C) CEVA Inc. All rights reserved.                                *
*                                                                             *
* Project: Debugger's User I/O DLL                                            *
*                                                                             *
* File Name: USERIO.H                                                         *
*                                                                             *
* Purpose: The UserIO DLL purpose is to enable the user to simulate the whole *
*          system.                                                            *
*          Since, the Debugger's build-in Simulator, simulates only the core  *
*          and the core is only a part of the whole system, the user can      *
*          implement the externals of the core by this DLL.                   *
*          The UserIO DLL contains set of functions that are called by the    *
*          Simulator before and after every instruction execution and when    *
*          the Simulator accesses externals (e.g: port, external register).   *
*          In addition, the UserIO DLL has the ability to call some of the    *
*          Debugger & the Simulator functions in order to control the way the *
*          application is running.                                            *
*                                                                             *
*          This File contains the skeleton structure of the UserIO DLL.       *
*          In order to build a user specific DLL, the user has to add his     *
*          specific code to the following empty functions.                    *
*                                                                             *
\*****************************************************************************/
#ifndef USER_IO_H_
#define USER_IO_H_

//-----------------------------------------------------------------------------
typedef enum UserIO_T_States_tag {
    UserIO_E_NoAccess,
    UserIO_E_Read,
    UserIO_E_Write
} UserIO_T_States;

//-----------------------------------------------------------------------------
// lbu
typedef WORD  T_DataMemoryCell;      // 16 Bits
typedef WORD  T_CodeMemoryCell;      // 16 Bits
typedef WORD  T_DataMemoryAddress;   // 16 Bits
typedef DWORD T_CodeMemoryAddress;   // 18 Bits

#define T_CodeMemoryWord T_CodeMemoryCell
#define T_DataMemoryWord T_DataMemoryCell
void SIMU_GenerateInterrupt(u16 );

//#define AUDIO_TX_FIFO_INT   (1<<0)
//#define AUDIO_RX_FIFO_INT   (1<<1)
//#define SPEECH_ENCODE_INT	(1<<2)
//#define SPEECH_DECODE_INT	(1<<3)
#define TX_PROCESS_INT		(2)
#define RX_PROCESS_INT		(3)
#define CPU_INT			    (4)
#define USER_INT			(5)
//#define EDGE_DETECTOR_INT	(1<<8)
//#define EDGE_VITERBI_INT	(1<<9)
//#define EDGE_PREFILTER_INT	(1<<10)
//#define DSMA_INT			(1<<11)
//#define SW_INT		        (1<<12)

#define RX_PORT_BASE           0xC000
#define RX_PORT_SIZE           2048

#define TX_PORT_BASE           0xC800
#define TX_PORT_SIZE           384
#define ADD_RxPort          RX_PORT_BASE
#define ADD_TxPort          TX_PORT_BASE

#define SIZE_RxPort         RX_PORT_SIZE
#define SIZE_TxPort         TX_PORT_SIZE

#define DATA_MEM_PAGE2	1			// modem

extern u16 OldMemPage;	// Old value for OU1 and OU0 bits in MOD0
extern u16 NewMemPage;	// New value for OU1 and OU0 bits in MOD0

extern u16 RxPortBuffer[SIZE_RxPort];
extern u16 TxPortBuffer[SIZE_TxPort];

#if 0
//-----------------------
#if (DSP_TYPE == OAK) // stands for TeakLite too !
//-----------------------

typedef enum {
    UserIO_E_Reset  = 0x0001,
    UserIO_E_Nmi    = 0x0002,
    UserIO_E_Trap   = 0x0004,
    UserIO_E_Int0   = 0x0008,
    UserIO_E_Int1   = 0x0010,
    UserIO_E_Int2   = 0x0020,
    UserIO_E_IU0    = 0x0040,
    UserIO_E_IU1    = 0x0080,
    UserIO_E_FloatD = 0x0100,
    UserIO_E_FloatP = 0x0200,
    UserIO_E_Boot   = 0x0400
} UserIO_T_InSigs;

typedef enum {
    UserIO_E_OU0      = 0x0001,
    UserIO_E_OU1      = 0x0002,
    UserIO_E_DVMatch  = 0x0004,
    UserIO_E_SoftTrap = 0x0008,
    UserIO_E_TrapA    = 0x0010
} UserIO_T_OutSigs;

typedef WORD T_DataMemoryCell;
typedef WORD T_CodeMemoryCell;
typedef WORD T_DataMemoryAddress;
typedef WORD T_CodeMemoryAddress;

typedef struct {
    UserIO_T_States     idState;
    T_DataMemoryAddress address;
    T_DataMemoryCell    data;
} UserIO_T_DataTransactionInfo;


//-----------------------
#ifdef  DSP_TKL2
#ifndef DataMemorySegmentStateStruct
#define DataMemorySegmentStateStruct
typedef struct T_DataMemorySegmentState_tag {
    WORD           xpage;
    WORD           ypage;
    WORD           zpage;
} T_DataMemorySegmentState;
#endif
#endif

//-----------------------
#elif (DSP_TYPE == PALM)
//-----------------------

typedef enum {
    UserIO_E_Reset = 0x0001,
    UserIO_E_Bpi   = 0x0002,
    UserIO_E_Nmi   = 0x0004,
    UserIO_E_Int0  = 0x0008,
    UserIO_E_Int1  = 0x0010,
    UserIO_E_Int2  = 0x0020,
    UserIO_E_VInt  = 0x0040,
    UserIO_E_VIntC = 0x0080,
    UserIO_E_IU0   = 0x0100,
    UserIO_E_IU1   = 0x0200,
    UserIO_E_Boot  = 0x0400
} UserIO_T_InSigs;

typedef enum {
    UserIO_E_OU0          = 0x0001,
    UserIO_E_OU1          = 0x0002,
    UserIO_E_DVMatch      = 0x0004,
    UserIO_E_UserSoftTrap = 0x0008,
    UserIO_E_EmulSoftTrap = 0x0010,
    UserIO_E_TrapA        = 0x0020,
    UserIO_E_OU2          = 0x0040, // 10/7/2002 - PalmDSPCore new OU[2-4] flags, RTL >= 2.06
    UserIO_E_OU3          = 0x0080,
    UserIO_E_OU4          = 0x0100

} UserIO_T_OutSigs;

typedef enum {
    PostModifyMode_E_0,
    PostModifyMode_E_Plus1,
    PostModifyMode_E_Minus1,
    PostModifyMode_E_PStep0
} UserIO_T_PostModifyMode;

typedef enum {
    CoProInstType_E_CPEX,
    CoProInstType_E_CPRD,
    CoProInstType_E_CPRDDW,
    CoProInstType_E_CPWR,
    CoProInstType_E_CPWRDW
} UserIO_T_CoProInstType;

typedef DWORD T_DataMemoryCell;      // 16/20/24 Bits
typedef WORD  T_CodeMemoryCell;      // 16 Bits
typedef WORD  T_DataMemoryAddress;   // 16 Bits
typedef DWORD T_CodeMemoryAddress;   // 20 Bits

#define UserIO_K_SizeDataTransactionInfo 8

typedef struct {
    UserIO_T_States     idState[UserIO_K_SizeDataTransactionInfo];
    T_DataMemoryAddress address[UserIO_K_SizeDataTransactionInfo];
    T_DataMemoryCell    data[UserIO_K_SizeDataTransactionInfo];
} UserIO_T_DataTransactionInfo;

//-----------------------
#elif (DSP_TYPE == TEAK)
//-----------------------

typedef enum {
    UserIO_E_Reset  = 0x0001,
    UserIO_E_Nmi    = 0x0002,
    UserIO_E_Trap   = 0x0004,
    UserIO_E_Int0   = 0x0008,
    UserIO_E_Int1   = 0x0010,
    UserIO_E_Int2   = 0x0020,
    UserIO_E_VInt   = 0x0040,
    UserIO_E_VIntC  = 0x0080,
    UserIO_E_IU0    = 0x0100,
    UserIO_E_IU1    = 0x0200,
    UserIO_E_FloatD = 0x0400,
    UserIO_E_FloatP = 0x0800,
    UserIO_E_Boot   = 0x1000
} UserIO_T_InSigs;

typedef enum {
    UserIO_E_OU0      = 0x0001,
    UserIO_E_OU1      = 0x0002,
    UserIO_E_DVMatch  = 0x0004,
    UserIO_E_SoftTrap = 0x0008,
    UserIO_E_TrapA    = 0x0010,
    UserIO_E_OU2      = 0x0020,     // 13/6/2002 - Teak new OU[2-4] flags RTL >= 2.1
    UserIO_E_OU3      = 0x0040,
    UserIO_E_OU4      = 0x0080
} UserIO_T_OutSigs;

#if ! defined (MAINWIN)
//typedef DWORD T_Reg;
//typedef WORD  T_ExtReg;
#endif
typedef WORD  T_DataMemoryCell;      // 16 Bits
typedef WORD  T_CodeMemoryCell;      // 16 Bits
typedef WORD  T_DataMemoryAddress;   // 16 Bits
typedef DWORD T_CodeMemoryAddress;   // 18 Bits

//#define UserIO_K_SizeDataTransactionInfo 8
#define UserIO_K_SizeDataTransactionInfo 512		// 512 comes from FFT
#define kNumberOfExtRegs 4							// imported from userio.c

/*typedef struct {
    UserIO_T_States     idState[UserIO_K_SizeDataTransactionInfo];
    T_DataMemoryAddress address[UserIO_K_SizeDataTransactionInfo];
    T_DataMemoryWord    data[UserIO_K_SizeDataTransactionInfo];
} UserIO_T_DataTransactionInfo;*/
typedef struct {						// bma structure
	DWORD				clock;
        UserIO_T_States     idState[UserIO_K_SizeDataTransactionInfo];
        T_DataMemoryAddress address[UserIO_K_SizeDataTransactionInfo];
    T_DataMemoryCell    data[UserIO_K_SizeDataTransactionInfo];
} UserIO_T_DataTransactionInfo[kNumberOfExtRegs];

typedef enum {
    PostModifyMode_E_0,
    PostModifyMode_E_Plus1,
    PostModifyMode_E_Minus1,
    PostModifyMode_E_PStep0
} UserIO_T_PostModifyMode;




//-----------------------
#endif
//-----------------------
#define T_CodeMemoryWord T_CodeMemoryCell
#define T_DataMemoryWord T_DataMemoryCell

#define UserIO_K_nMajorVersion   9
#define UserIO_K_nMinorVersion   11



//=============================================================================
// Imported Functions from the Debugger:
//=============================================================================

//-----------------------------------------------------------------------------
//  Following are pointers to functions of the Debugger that are callable by
//  this module
//  The usage of these pointers (that are initialized by the ConnectDbgFuncs
//  function) can be done as any other regular function call,
//  e.g:  Gui_DisplayMessageBox( "message", "title", MsgDisp_E_Info );
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Purpose: Display message box.
// Input:   pMsg    - pointer to string with free text message to display.
//          pTitle  - pointer to string with free text title of the displayed
//                    message box.
//          msgType - one of the T_MessageDisplayTypes type enumerations
//                    values.
// Output:  void
//----------------------------------------------------------------------------
typedef void (* T_PFunc_Gui_DisplayMessageBox) ( char                  *pMsg,
                                                 char                  *pTitle,
                                                 T_MessageDisplayTypes  msgType );

//----------------------------------------------------------------------------
// Purpose: Display message box and get return value.
// Input:   pMsg   - pointer to string with free text message to display.
//          pTitle - pointer to string with free text title of the displayed
//                   message box.
// Output:  The user botton choice (OK/Cancel).
//----------------------------------------------------------------------------
typedef BOOL (* T_PFunc_Gui_DisplayMessageBoxAndGet) ( char *pMsg,
                                                       char *pTitle );

//----------------------------------------------------------------------------
// Purpose: Enter a CLI command into the Debugger's CLI queue.
// Input:   pCliCmd          - pointer to string to fill with the CLI command.
//          enableMessageBox - indication if this function should cause
//                             a message box to pop up or not.
// Output:  TRUE  ==> SUCCESS
//          FALSE ==> FAILURE (CLI queue is full).
// Note:    Regular commands will be processed, by the Debugger, in it's turn.
//          The user can post a high priority commands, that will be put in
//          the head of the CLI queue, by prefixing the command with a
//          caret ('^').
//----------------------------------------------------------------------------
typedef BOOL (* T_PFunc_Dbg_PutCliCommand) ( char *pCliCmd, BOOL enableMessageBox );

//----------------------------------------------------------------------------
// Purpose: Get an internal register value.
// Input:   regID     - register ID (T_ShortRegID enumeration).
//          pRegValue - pointer to T_Reg to fill with the register's value.
// Output:  TRUE  ==> SUCCESS
//          FALSE ==> FAILURE (illegal regID or calling from unsafed function).
//----------------------------------------------------------------------------
typedef BOOL (* T_PFunc_Dbg_GetRegister) ( T_ShortRegID regID, T_Reg *pRegValue );

//----------------------------------------------------------------------------
// Purpose: Set an internal register value.
// Input:   regID    - Register ID (T_ShortRegID enumeration)..
//          regValue - value to fill the register with.
// Output:  TRUE  ==> SUCCESS
//          FALSE ==> FAILURE (illegal regID or calling from unsafed function).
//----------------------------------------------------------------------------
typedef BOOL (* T_PFunc_Dbg_SetRegister) ( T_ShortRegID regID, T_Reg regValue );

//----------------------------------------------------------------------------
// Purpose: Get the clock count.
// Input:   pClockValue - pointer to DWORD to fill with the current clock
//                        counter value.
// Output:  TRUE  ==> SUCCESS
//          FALSE ==> FAILURE (calling from unsafed function).
//----------------------------------------------------------------------------
typedef BOOL (* T_PFunc_Dbg_GetClockCount) ( DWORD *pClockValue );

//----------------------------------------------------------------------------
// Purpose: Set the clock to a given value.
// Input:   clockValue - new value to set the clock with.
// Output:  TRUE  ==> SUCCESS
//          FALSE ==> FAILURE (calling from unsafed function).
//----------------------------------------------------------------------------
typedef BOOL (* T_PFunc_Dbg_SetClockCount) ( DWORD clockValue );

//----------------------------------------------------------------------------
// Purpose: Get a code memory word.
// Input:   address   - address in the code memory.
//          pCodeWord - pointer to T_CodeMemoryCell to fill with the memory
//                      address content.
// Output:  TRUE  ==> SUCCESS
//          FALSE ==> FAILURE (unmapped memory address or calling from unsafed
//                             function).
//----------------------------------------------------------------------------
typedef BOOL (* T_PFunc_Dbg_GetCodeMemoryWord) ( T_CodeMemoryAddress  address,
                                                 T_CodeMemoryCell    *pCodeWord );

//----------------------------------------------------------------------------
// Purpose: Set a code memory word.
// Input:   address  - address in the code memory.
//          codeWord - code word to fill the memory address with.
// Output:  TRUE  ==> SUCCESS
//          FALSE ==> FAILURE (unmapped memory address or calling from unsafed
//                             function).
// Note:    Note that the Debugger will relate the modified address as an
//          invalidated address (as after the MOVD instruction was executed).
//          When the simulator fetch an invalidated address, the whole code
//          memory will be validated (i.e: decoded and restrictions checked).
//----------------------------------------------------------------------------
typedef BOOL (* T_PFunc_Dbg_SetCodeMemoryWord) ( T_CodeMemoryAddress address,
                                                 T_CodeMemoryCell    codeWord );

//----------------------------------------------------------------------------
// Purpose: Get a data memory word.
// Input:   address   - address in the data memory.
//          pDataWord - pointer to T_DataMemoryWord to fill with the memory
//                      address content.
// Output:  TRUE  ==> SUCCESS
//          FALSE ==> FAILURE (unmapped memory address or calling from unsafed
//                             function).
//----------------------------------------------------------------------------
typedef BOOL (* T_PFunc_Dbg_GetDataMemoryWord) ( T_DataMemoryAddress  address,
                                                 T_DataMemoryCell    *pDataWord );

//----------------------------------------------------------------------------
// Purpose: Set a data memory word.
// Input:   address  - address in the data memory.
//          dataWord - data word to fill the memory address with.
// Output:  TRUE  ==> SUCCESS
//          FALSE ==> FAILURE (unmapped memory address or calling from unsafed
//                             function).
//----------------------------------------------------------------------------
typedef BOOL (* T_PFunc_Dbg_SetDataMemoryWord) ( T_DataMemoryAddress address,
                                                 T_DataMemoryCell    dataValue );

//----------------------------------------------------------------------------
// Purpose: Get the current working code memory segment number.
// Input:   pCodeSegmentNumber - pointer to WORD to fill with the current code
//                               memory segment number.
// Output:  TRUE  ==> SUCCESS
//          FALSE ==> FAILURE (calling from unsafed function).
//----------------------------------------------------------------------------
typedef BOOL (* T_PFunc_Dbg_GetCurrentCodeMemorySegmentNumber) ( WORD *pCodeSegmentNumber );

//----------------------------------------------------------------------------
// Purpose: Set the current working code memory segment number.
// Input:   codeSegmentNumber - new code memory segment number to set
//                              (0, 1, 2, 3, ...).
// Output:  TRUE  ==> SUCCESS
//          FALSE ==> FAILURE (out of range segment number or calling from
//                             unsafed function).
//----------------------------------------------------------------------------
typedef BOOL (* T_PFunc_Dbg_SetCurrentCodeMemorySegmentNumber) ( WORD codeSegmentNumber );

//----------------------------------------------------------------------------
// Purpose: Get the current working data memory segment number.
// Input:   pDataSegmentNumber - pointer to WORD to fill with the current data
//                               memory segment number. In tkl2 this is a pointer
//                               to a struct that contains the three values of the
//                               three segments of tkl2.
// Output:  TRUE  ==> SUCCESS
//          FALSE ==> FAILURE (calling from unsafed function).
//----------------------------------------------------------------------------
typedef BOOL (* T_PFunc_Dbg_GetCurrentDataMemorySegmentNumber) ( WORD *pDataSegmentNumber );

//----------------------------------------------------------------------------
// Purpose: Set the current working data memory segment number.
// Input:   dataSegmentNumber - new data memory segment number to set
//                              (0, 1, 2, 3, ...).In tkl2 this is a struct that
//                              contains the three values of the three segments
//                              of tkl2.
// Output:  TRUE  ==> SUCCESS
//          FALSE ==> FAILURE (out of range segment number or calling from
//                             unsafed function).
//----------------------------------------------------------------------------
typedef BOOL (* T_PFunc_Dbg_SetCurrentDataMemorySegmentNumber) ( WORD dataSegmentNumber );


// Connection struct of functions pointers of the Debugger:
typedef struct T_DbgUserIOFunctions_tag {
    T_PFunc_Gui_DisplayMessageBox                 Gui_DisplayMessageBox;
    T_PFunc_Gui_DisplayMessageBoxAndGet           Gui_DisplayMessageBoxAndGet;
    T_PFunc_Dbg_PutCliCommand                     Debugger_PutCliCommand;
    T_PFunc_Dbg_GetRegister                       Debugger_GetRegister;
    T_PFunc_Dbg_SetRegister                       Debugger_SetRegister;
    T_PFunc_Dbg_GetClockCount                     Debugger_GetClockCount;
    T_PFunc_Dbg_SetClockCount                     Debugger_SetClockCount;
    T_PFunc_Dbg_GetCodeMemoryWord                 Debugger_GetCodeMemoryWord;
    T_PFunc_Dbg_SetCodeMemoryWord                 Debugger_SetCodeMemoryWord;
    T_PFunc_Dbg_GetDataMemoryWord                 Debugger_GetDataMemoryWord;
    T_PFunc_Dbg_SetDataMemoryWord                 Debugger_SetDataMemoryWord;
    T_PFunc_Dbg_GetCurrentCodeMemorySegmentNumber Debugger_GetCurrentCodeMemorySegmentNumber;
    T_PFunc_Dbg_SetCurrentCodeMemorySegmentNumber Debugger_SetCurrentCodeMemorySegmentNumber;
    T_PFunc_Dbg_GetCurrentDataMemorySegmentNumber Debugger_GetCurrentDataMemorySegmentNumber;
    T_PFunc_Dbg_SetCurrentDataMemorySegmentNumber Debugger_SetCurrentDataMemorySegmentNumber;
} T_DbgUserIOFunctions;


//=============================================================================
// Exported functions:
//=============================================================================

//-----------------------------------------------------------------------------

#undef DLLFUNC
#define DLLFUNC

//-----------------------------------------------------------------------------

#undef EXTERNC

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

//-----------------------------------------------------------------------------

#ifdef NON_UNIX

#undef DECLSPEC
#ifdef INSIDE_USERIO_DLL_
#define DECLSPEC EXTERNC __declspec(dllexport)  // The DLL exports the following functions.
#else
#define DECLSPEC EXTERNC __declspec(dllimport)  // The DLL users import the following functions.
#endif /* INSIDE_USERIO_DLL_ */

#else /* UNIX */

#undef DECLSPEC
#define DECLSPEC EXTERNC

#endif /* UNIX */

//-----------------------------------------------------------------------------
typedef struct T_UserIOEnabledFunctions_tag
{
    BOOL UserIO_InExtCodeMemory;
    BOOL UserIO_InExtDataMemory;
    BOOL UserIO_InPort;
    BOOL UserIO_OutExtCodeMemory;
    BOOL UserIO_OutExtDataMemory;
    BOOL UserIO_OutPort;
    BOOL UserIO_PostExec;
    BOOL UserIO_PreFetch;
    BOOL UserIO_SetUser;
    BOOL UserIO_StopSimul;
#if ((DSP_TYPE == OAK) || (DSP_TYPE == TEAK) || (DSP_TYPE == PALM) || (DSP_TYPE == TKL3))
    BOOL UserIO_InExtReg;
    BOOL UserIO_OutExtReg;
#endif
#if ((DSP_TYPE == TEAK) || (DSP_TYPE == PALM))
    BOOL UserIO_ExternalPostModification;
#endif
#if (DSP_TYPE == PALM)
    BOOL UserIO_CoProcessor;
    BOOL UserIO_SetDataMemoryWordSize;
    BOOL UserIO_SetBusLockSignal;
#endif
#if ((DSP_TYPE == CEVAX) || (DSP_TYPE == TKL3))
    BOOL UserIO_InIOPort;
    BOOL UserIO_OutIOPort;
    BOOL UserIO_InExtIOMemory;
    BOOL UserIO_OutExtIOMemory;
#endif
} T_UserIOEnabledFunctions;

DECLSPEC DWORD DLLFUNC UserIO_Connect (
    WORD    Arg_R_nMajorVersion,
    WORD    Arg_R_nMinorVersion
);

DECLSPEC void DLLFUNC UserIO_ConnectDbgFuncs (
    T_DbgUserIOFunctions *pDbgUserIOFuncs
);

DECLSPEC void DLLFUNC UserIO_GetEnabledFuncs (
    T_UserIOEnabledFunctions *pEnabledFuncs
);

DECLSPEC void DLLFUNC UserIO_Disconnect (
    void
);

// For Oak, Teak, TeakLite. Palm
//-----------------------------------------------------------------
#if ((DSP_TYPE == OAK) || (DSP_TYPE == TEAK) || (DSP_TYPE == PALM) || (DSP_TYPE == TKL3))
//-----------------------------------------------------------------
DECLSPEC T_ExtReg DLLFUNC UserIO_InExtReg (
    int     Arg_R_iReg
);
//-----------------------------------------------------------------
#endif // ((DSP_TYPE == OAK) || (DSP_TYPE == TEAK) || (DSP_TYPE == PALM))
//-----------------------------------------------------------------

DECLSPEC BOOL DLLFUNC UserIO_InExtCodeMemory (
    T_CodeMemoryAddress  Arg_R_Address,
    T_CodeMemoryCell    *Arg_R_Data,
    BOOL                 isCodeFetch,
    T_CodeMemoryAddress *Arg_R_Address2,
    T_CodeMemoryCell    *Arg_R_Data2
);

DECLSPEC BOOL DLLFUNC UserIO_InExtDataMemory (
    T_DataMemoryAddress  Arg_R_Address,
    T_DataMemoryCell    *Arg_R_pData,
    BYTE                 Arg_R_pNumOfCells,
    BOOL                 Arg_R_isFromSim
);

DECLSPEC BOOL DLLFUNC UserIO_InPort (
    T_DataMemoryAddress  Arg_R_Address,
    T_DataMemoryCell    *Arg_R_pData,
    BYTE                 Arg_R_numOfCells,
    BOOL                peekAddress,
    BOOL                isFromSim
);

// For Oak, Teak, TeakLite. Palm
//-----------------------------------------------------------------
#if ((DSP_TYPE == OAK) || (DSP_TYPE == TEAK) || (DSP_TYPE == PALM) || (DSP_TYPE == TKL3))
//-----------------------------------------------------------------
DECLSPEC void DLLFUNC UserIO_OutExtReg (
    int      Arg_R_iReg,
    T_ExtReg Arg_R_Data
);
//-----------------------------------------------------------------
#endif // ((DSP_TYPE == OAK) || (DSP_TYPE == TEAK) || (DSP_TYPE == PALM))
//-----------------------------------------------------------------

DECLSPEC void DLLFUNC UserIO_OutExtCodeMemory (
    T_CodeMemoryAddress Arg_R_Address,
    T_CodeMemoryCell    Arg_R_Data
);

DECLSPEC void DLLFUNC UserIO_OutExtDataMemory (
    T_DataMemoryAddress  Arg_R_Address,
    T_DataMemoryCell    *Arg_R_pData,
    BYTE                 Arg_R_pNumOfCells,
    BOOL                 Arg_R_isFromSim
);

DECLSPEC void DLLFUNC UserIO_OutPort (
    T_DataMemoryAddress  Arg_R_Address,
    T_DataMemoryCell    *Arg_R_pData,
    BYTE                 Arg_R_numOfCells,
    BOOL                 Arg_R_isFromSim
);

#if ((DSP_TYPE == OAK) || (DSP_TYPE == PALM) || (DSP_TYPE == TEAK))
DECLSPEC const char * DLLFUNC UserIO_PostExec (
    UserIO_T_DataTransactionInfo *Arg_R_pDataTransactionInfo,
    WORD                          Arg_R_wOutSigsMask,
    UserIO_T_InSigs               Arg_R_idInterruptAck,
    WORD                         *Arg_R_lpwInSigsMask
#if ( (DSP_TYPE == PALM) || (DSP_TYPE == TEAK) )
   ,T_CodeMemoryAddress          *Arg_R_pVectorAddress
#endif // Palm / Teak
);

#elif ((DSP_TYPE == CEVAX) || (DSP_TYPE == TKL3))
DECLSPEC const char * DLLFUNC UserIO_PostExec (
    DWORD                        Arg_R_dwClock,
    T_CodeMemoryAddress          Arg_R_PC,
    const T_CoreOutputPinsArray *Arg_R_pOutputPins
);
#endif // dsp_type

#if ((DSP_TYPE == OAK) || (DSP_TYPE == PALM) || (DSP_TYPE == TEAK))
DECLSPEC int DLLFUNC UserIO_PreFetch (
    T_CodeMemoryAddress Arg_R_PC,
    DWORD               Arg_R_dwClock
);
#endif

DECLSPEC const char * DLLFUNC UserIO_SetUser (
    WORD    Arg_R_wFunc,
    DWORD   Arg_R_dwData
);

DECLSPEC BOOL DLLFUNC UserIO_StopSimul (
    void
);


//--------------------------------------------
#if ( (DSP_TYPE == PALM) || (DSP_TYPE == TEAK) )
//--------------------------------------------

DECLSPEC WORD DLLFUNC UserIO_ExternalPostModification (
    UserIO_T_PostModifyMode Arg_R_Mode,
    BOOL                    Arg_R_IsRi
);

//--------------------
#endif // PALM || TEAK
//--------------------

//--------------------
#if (DSP_TYPE == PALM)
//--------------------

DECLSPEC void DLLFUNC UserIO_CoProcessor (
    UserIO_T_CoProInstType Arg_R_Type,
    int                    Arg_R_ExtRegId1,
    int                    Arg_R_ExtRegId2,
    WORD                   Arg_R_4BitCoProCmd
);

DECLSPEC void DLLFUNC UserIO_SetDataMemoryWordSize (
    WORD Arg_R_DataWordSize
);

DECLSPEC void DLLFUNC UserIO_SetBusLockSignal (
    WORD Arg_R_NumCycles
);

//--------------------
#endif // PALM
//--------------------
#endif // 0

#endif /* USER_IO_H_ */
/*================================== E N D ==================================*/
