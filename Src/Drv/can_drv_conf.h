#ifndef CAN_DRV_CONF_H
#define CAN_DRV_CONF_H

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/

/*******************************************************************************
**                      Global Macro Definitions                              **
*******************************************************************************/
#define CAN_DRV_NM_MSG_ID01 0x801
#define CAN_DRV_NM_MSG_ID02 0x802

#define CAN_DRV_TP_MSG_ID01 0x701
#define CAN_DRV_TP_MSG_ID02 0x702

#define CAN_DRV_IS_NM_MSG(id) (((id) == CAN_DRV_NM_MSG_ID01) || ((id) == CAN_DRV_NM_MSG_ID02))

#define CAN_DRV_IS_TP_MSG(id) (((id) == CAN_DRV_TP_MSG_ID01) || ((id) == CAN_DRV_TP_MSG_ID02))

#define CAN_DRV_RECV_BUF_SIZE 100U

#define CAN_DRV_INSTS_NUMS 1U

#endif /* CAN_DRV_CONF_H */
