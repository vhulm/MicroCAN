/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include "tmr.h"

/*******************************************************************************
**                      Private Macro Definitions                             **
*******************************************************************************/

/*******************************************************************************
**                      Private Type Definitions                              **
*******************************************************************************/

/*******************************************************************************
**                      Private Function Declarations                         **
*******************************************************************************/

/*******************************************************************************
**                      Private Variable Definitions                          **
*******************************************************************************/

/*******************************************************************************
**                      Private Function Definitions                         **
*******************************************************************************/

/*******************************************************************************
**                      Global Variable Definitions                           **
*******************************************************************************/

/*******************************************************************************
**                      Global Function Definitions                           **
*******************************************************************************/
void TimerTask(const TMGR *Tmgr)
{
    uint8 Id = 0;
    TMR *pTmr = Tmgr->pTmrs;

    for (Id = 0; Id < Tmgr->TmrNum; Id++)
    {
        if (pTmr->State == TMR_STATE_ACTIVE)
        {
            if (pTmr->StartTimeOutVal == 0)
            {
                if (pTmr->CycleTimeOutVal == 0)
                {
                    pTmr->State = TMR_STATE_INACTIVE;
                }
                else
                {
                    pTmr->StartTimeOutVal = pTmr->CycleTimeOutVal;
                }

                pTmr->CallBack();
            }
            else
            {
                if (pTmr->StartTimeOutVal > Tmgr->SchedCycle)
                {
                    pTmr->StartTimeOutVal -= Tmgr->SchedCycle;
                }
                else
                {
                    pTmr->StartTimeOutVal = 0;
                }
            }
        }

        pTmr++;
    }
}

void RestartTimer(const TMGR *Tmgr, uint32 TmrID, uint32 StartTimeOutVal, uint32 CycleTimeOutVal)
{
    if (TmrID < Tmgr->TmrNum)
    {
        Tmgr->pTmrs[TmrID].State = TMR_STATE_ACTIVE;
        Tmgr->pTmrs[TmrID].StartTimeOutVal = StartTimeOutVal;
        Tmgr->pTmrs[TmrID].CycleTimeOutVal = CycleTimeOutVal;
    }
}

void KillTimer(const TMGR *Tmgr, uint32 TmrID)
{
    if (TmrID < Tmgr->TmrNum)
    {
        Tmgr->pTmrs[TmrID].State = TMR_STATE_INACTIVE;
    }
}

boolean TimerIsAlive(const TMGR *Tmgr, uint32 TmrID)
{
    if (TmrID < Tmgr->TmrNum)
    {
        if (Tmgr->pTmrs[TmrID].State == TMR_STATE_ACTIVE)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}
