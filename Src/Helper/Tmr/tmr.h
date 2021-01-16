#ifndef TMR_H
#define TMR_H
#include "types.h"

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/

/*******************************************************************************
**                      Global Macro Definitions                              **
*******************************************************************************/

/*******************************************************************************
**                      Global Type Definitions                               **
*******************************************************************************/
typedef enum
{
    TMR_STATE_ACTIVE,
    TMR_STATE_INACTIVE,
} TMR_STATE;

typedef void (*TMR_CALL_BACK_FUNC)(void);

typedef struct
{
    TMR_STATE State;
    uint32 StartTimeOutVal;
    uint32 CycleTimeOutVal;
    TMR_CALL_BACK_FUNC CallBack;
} TMR;

typedef struct
{
    TMR *pTmrs;
    uint32 SchedCycle;
    uint32 TmrNum;
} TMGR;
/*******************************************************************************
**                      Global Variable Declarations                          **
*******************************************************************************/

/*******************************************************************************
**                      Global Function Declarations                          **
*******************************************************************************/
void TimerTask(const TMGR *Tmgr);
void RestartTimer(const TMGR *Tmgr, uint32 TmrID, uint32 StartTimeOutVal, uint32 CycleTimeOutVal);
void KillTimer(const TMGR *Tmgr, uint32 TmrID);
boolean TimerIsAlive(const TMGR *Tmgr, uint32 TmrID);

#endif //TMR_H
