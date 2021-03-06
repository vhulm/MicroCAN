#ifndef CAN_APP_H
#define CAN_APP_H
#include "types.h"
#include "can_drv.h"

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/

/*******************************************************************************
**                      Global Macro Definitions                              **
*******************************************************************************/

/*******************************************************************************
**                      Global Type Definitions                               **
*******************************************************************************/
typedef void (*can_app_msg_ind_cb)(const uint8_t *);

typedef struct
{
  uint32 msg_id;
  can_app_msg_ind_cb msg_ind_cb;
} can_app_msg_id_ind_cb_mapping;

/*******************************************************************************
**                      Global Variable Declarations                          **
*******************************************************************************/

/*******************************************************************************
**                      Global Function Declarations                          **
*******************************************************************************/
void can_app_init(void);
void can_app_task(void);
void can_app_msg_process(uint8 instance, can_drv_msg *msg);

#endif /* CAN_APP_H */
