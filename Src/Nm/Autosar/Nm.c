/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include "types.h"
#include "tmr.h"
#include "Nm_Cfg.h"
#include "Nm.h"

/*******************************************************************************
**                      Private Macro Definitions                             **
*******************************************************************************/
#define NM_GET_MODE() (NmMode)
#define NM_SET_MODE(m) (NmMode = (m))

#define NM_GET_STATE() (NmState)
#define NM_SET_STATE(m)                            \
    do                                             \
    {                                              \
        if (NM_GET_MODE() == NM_MODE_NETWORK_MODE) \
        {                                          \
            NmState = (m);                         \
        }                                          \
    } while (0)

/*******************************************************************************
**                      Private Type Definitions                              **
*******************************************************************************/
typedef enum
{
    NM_MODE_BUS_SLEEP_MODE = 1U,
    NM_MODE_PREPARE_BUS_SLEEP_MODE,
    NM_MODE_NETWORK_MODE,
} NM_MODE;

typedef enum
{
    NM_STATE_REPEAT_MESSAGE_STATE = 1U,
    NM_STATE_NORMAL_OPERATION_STATE,
    NM_STATE_READY_SLEEP_STATE,
} NM_STATE;

typedef enum
{
    NM_TMR_Msg_Cycle_Time = 0,
    NM_TMR_NM_Timeout_Timer,
    NM_TMR_Wait_Bus_Sleep_Timer,
    NM_TMR_Repeat_Message_Timer,
    NM_TMR_Tx_Timeout_Timer,
    NM_TMR_ID_MAX
} NM_TMR_ID;
/*******************************************************************************
**                      Private Function Declarations                         **
*******************************************************************************/
static void Nm_TimerTask(void);
static void Nm_RestartTimer(NM_TMR_ID Id, uint32 StartTimeOutVal, uint32 CycleTimeOutVal);
static void Nm_KillTimer(NM_TMR_ID Id);
static boolean Nm_TimerIsAlive(NM_TMR_ID Id);

static void Nm_MsgCycleTimeCallback(void);
static void Nm_NMTimeoutTimerCallback(void);
static void Nm_WaitBusSleepTimerCallback(void);
static void Nm_RepeatMessageTimerCallback(void);
static void Nm_TxTimeoutTimerCallback(void);

/*******************************************************************************
**                      Private Variable Definitions                          **
*******************************************************************************/
static NM_PDU NmRxPdu;
static NM_PDU NmTxPdu;
static NM_MODE NmMode = 0;
static NM_STATE NmState = 0;
static uint8 NmInitState = FALSE;
static uint8 NmNetworkRequested = FALSE;

static TMR NmTmr[NM_TMR_ID_MAX] = {
    {TMR_STATE_INACTIVE, 0, 0, Nm_MsgCycleTimeCallback},
    {TMR_STATE_INACTIVE, 0, 0, Nm_NMTimeoutTimerCallback},
    {TMR_STATE_INACTIVE, 0, 0, Nm_WaitBusSleepTimerCallback},
    {TMR_STATE_INACTIVE, 0, 0, Nm_RepeatMessageTimerCallback},
    {TMR_STATE_INACTIVE, 0, 0, Nm_TxTimeoutTimerCallback},

};

static const TMGR Nm_Tmgr = {.pTmrs = NmTmr, .SchedCycle = 10U, .TmrNum = NM_TMR_ID_MAX};

/*******************************************************************************
**                      Private Function Definitions                         **
*******************************************************************************/
static void Nm_TimerTask(void)
{
    TimerTask(&Nm_Tmgr);
}

static void Nm_RestartTimer(NM_TMR_ID Id, uint32 StartTimeOutVal, uint32 CycleTimeOutVal)
{
    RestartTimer(&Nm_Tmgr, Id, StartTimeOutVal, CycleTimeOutVal);
}

static void Nm_KillTimer(NM_TMR_ID Id)
{
    KillTimer(&Nm_Tmgr, Id);
}

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
static boolean Nm_TimerIsAlive(NM_TMR_ID Id)
{
    return TimerIsAlive(&Nm_Tmgr, Id);
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

static void Nm_MsgCycleTimeCallback(void)
{
    CanNm_Transmit(&NmTxPdu);
}

static void Nm_NMTimeoutTimerCallback(void)
{
    if (NM_GET_MODE() == NM_MODE_NETWORK_MODE)
    {
        if ((NM_GET_STATE() == NM_STATE_REPEAT_MESSAGE_STATE) || (NM_GET_STATE() == NM_STATE_NORMAL_OPERATION_STATE))
        {
            Nm_RestartTimer(NM_TMR_NM_Timeout_Timer, CanNmTimeoutTime, CanNmTimeoutTime);
        }

        if (NM_GET_STATE() == NM_STATE_READY_SLEEP_STATE)
        {
            Nm_RestartTimer(NM_TMR_Wait_Bus_Sleep_Timer, CanNmWaitBusSleepTime, 0);
            NM_SET_MODE(NM_MODE_PREPARE_BUS_SLEEP_MODE);
            Nm_PrepareBusSleepMode();
        }
    }
}

static void Nm_WaitBusSleepTimerCallback(void)
{
    if (NM_GET_MODE() == NM_MODE_PREPARE_BUS_SLEEP_MODE)
    {
        NM_SET_MODE(NM_MODE_BUS_SLEEP_MODE);
        Nm_BusSleepMode();
    }
}

static void Nm_RepeatMessageTimerCallback(void)
{
    if (NmNetworkRequested == TRUE)
    {
        NM_SET_MODE(NM_STATE_NORMAL_OPERATION_STATE);
    }
    else
    {
        NM_SET_MODE(NM_STATE_READY_SLEEP_STATE);
    }
}

static void Nm_TxTimeoutTimerCallback(void)
{
    if (NM_GET_MODE() == NM_MODE_NETWORK_MODE)
    {
        if ((NM_GET_STATE() == NM_STATE_REPEAT_MESSAGE_STATE) || (NM_GET_STATE() == NM_STATE_NORMAL_OPERATION_STATE))
        {
            Nm_TxTimeoutException();
        }
    }
}

/*******************************************************************************
**                      Global Variable Definitions                           **
*******************************************************************************/

/*******************************************************************************
**                      Global Function Definitions                           **
*******************************************************************************/
void CanNm_Init(void)
{
    if (NmInitState == FALSE)
    {
        NM_SET_MODE(NM_MODE_BUS_SLEEP_MODE);
    }
}

void CanNm_DeInit(void)
{
    if ((NmInitState == TRUE) && (NM_GET_MODE() == NM_MODE_BUS_SLEEP_MODE))
    {
        uint8 TmrId = 0;
        for (TmrId = 0; TmrId < NM_TMR_ID_MAX; TmrId++)
        {
            Nm_KillTimer(TmrId);
        }
    }
}

void CanNm_PassiveStartup(void)
{
    if ((NM_GET_MODE() == NM_MODE_BUS_SLEEP_MODE) || (NM_GET_MODE() == NM_MODE_PREPARE_BUS_SLEEP_MODE))
    {
        Nm_RestartTimer(NM_TMR_NM_Timeout_Timer, CanNmTimeoutTime, CanNmTimeoutTime);
        Nm_RestartTimer(NM_TMR_Repeat_Message_Timer, CanNmRepeatMessageTime, CanNmRepeatMessageTime);
        NM_SET_MODE(NM_MODE_NETWORK_MODE);
        NM_SET_STATE(NM_STATE_REPEAT_MESSAGE_STATE);
        Nm_NetworkMode();
    }
}

void CanNm_NetworkRequest(void)
{
    if ((NM_GET_MODE() == NM_MODE_BUS_SLEEP_MODE) || (NM_GET_MODE() == NM_MODE_PREPARE_BUS_SLEEP_MODE))
    {
        Nm_RestartTimer(NM_TMR_NM_Timeout_Timer, CanNmTimeoutTime, CanNmTimeoutTime);
        Nm_RestartTimer(NM_TMR_Repeat_Message_Timer, CanNmRepeatMessageTime, CanNmRepeatMessageTime);
        NM_SET_MODE(NM_MODE_NETWORK_MODE);
        NM_SET_STATE(NM_STATE_REPEAT_MESSAGE_STATE);
        Nm_NetworkMode();
    }
    else if (NM_GET_MODE() == NM_MODE_NETWORK_MODE)
    {
        if (NM_GET_STATE() == NM_STATE_REPEAT_MESSAGE_STATE)
        {
            Nm_RestartTimer(NM_TMR_Repeat_Message_Timer, CanNmRepeatMessageTime, CanNmRepeatMessageTime);
        }
        else if (NM_GET_STATE() == NM_STATE_READY_SLEEP_STATE)
        {
            NM_SET_STATE(NM_STATE_NORMAL_OPERATION_STATE);
        }
    }
}

void CanNm_NetworkRelease(void)
{
    if (NM_GET_MODE() == NM_MODE_NETWORK_MODE)
    {
        if (NM_GET_STATE() == NM_STATE_NORMAL_OPERATION_STATE)
        {
            NM_SET_STATE(NM_STATE_READY_SLEEP_STATE);
        }
    }
}

void CanNm_Transmit(NM_PDU *pdu)
{
    CanNm_TxConfirmation();
}

void CanNm_TxConfirmation(void)
{
    Nm_RestartTimer(NM_TMR_NM_Timeout_Timer, CanNmTimeoutTime, CanNmTimeoutTime);
}

void CanNm_RxIndication(NM_PDU *pdu)
{
    NmRxPdu = *pdu;
    if (NM_GET_MODE() == NM_MODE_BUS_SLEEP_MODE)
    {
        Nm_NetworkStartIndication();
    }
    else if (NM_GET_MODE() == NM_MODE_PREPARE_BUS_SLEEP_MODE)
    {
        Nm_RestartTimer(NM_TMR_NM_Timeout_Timer, CanNmTimeoutTime, CanNmTimeoutTime);
        Nm_RestartTimer(NM_TMR_Repeat_Message_Timer, CanNmRepeatMessageTime, CanNmRepeatMessageTime);
        NM_SET_MODE(NM_MODE_NETWORK_MODE);
        NM_SET_STATE(NM_STATE_REPEAT_MESSAGE_STATE);
        Nm_NetworkMode();
    }
    else if (NM_GET_MODE() == NM_MODE_NETWORK_MODE)
    {
        Nm_RestartTimer(NM_TMR_NM_Timeout_Timer, CanNmTimeoutTime, CanNmTimeoutTime);
    }
}

void CanNm_MainFunction(void)
{
    Nm_TimerTask();
}

void Nm_NetworkStartIndication(void)
{
}

void Nm_BusSleepMode(void)
{
}

void Nm_PrepareBusSleepMode(void)
{
}

void Nm_NetworkMode(void)
{
}

void Nm_TxTimeoutException(void)
{
}
