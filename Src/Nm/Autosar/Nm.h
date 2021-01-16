#ifndef NM_H
#define NM_H

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/

/*******************************************************************************
**                      Global Macro Definitions                              **
*******************************************************************************/

/*******************************************************************************
**                      Global Type Definitions                               **
*******************************************************************************/
typedef struct
{
    uint8 SourceNodeId;
    uint8 RepeatMessageRequest : 1;
    uint8 PNShutdownRequestBit : 1;
    uint8 Reserved00 : 1;
    uint8 NMCoordinatorSleepReadyBit : 1;
    uint8 ActiveWakeupBit : 1;
    uint8 PartialNetworkLearningBit : 1;
    uint8 PartialNetworkInformationBit : 1;
    uint8 Reserved01 : 1;
    uint8 UserData0;
    uint8 UserData1;
    uint8 UserData2;
    uint8 UserData3;
    uint8 UserData4;
    uint8 UserData5;
} NM_PDU;
/*******************************************************************************
**                      Global Variable Declarations                          **
*******************************************************************************/

/*******************************************************************************
**                      Global Function Declarations                          **
*******************************************************************************/
void CanNm_Init(void);
void CanNm_DeInit(void);
void CanNm_PassiveStartup(void);
void CanNm_NetworkRequest(void);
void CanNm_NetworkRelease(void);

void CanNm_Transmit(NM_PDU *pdu);
void CanNm_TxConfirmation(void);
void CanNm_RxIndication(NM_PDU *pdu);

void CanNm_MainFunction(void);

void Nm_NetworkStartIndication(void);
void Nm_BusSleepMode(void);
void Nm_PrepareBusSleepMode(void);
void Nm_NetworkMode(void);
void Nm_TxTimeoutException(void);

#endif /* NM_H */
