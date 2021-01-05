/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include <string.h>
#include "types.h"
#include "can_drv_conf.h"
#include "can_drv.h"

/*******************************************************************************
**                      Private Macro Definitions                             **
*******************************************************************************/

/*******************************************************************************
**                      Private Type Definitions                              **
*******************************************************************************/
typedef struct
{
	uint16 used;
	uint16 free;
	can_drv_msg buf[CAN_DRV_RECV_BUF_SIZE];
} can_drv_fifo;

/*******************************************************************************
**                      Private Function Declarations                         **
*******************************************************************************/
static void fifo_init(can_drv_fifo *this);
static uint8 fifo_push(can_drv_fifo *this, can_drv_msg *msg);
static uint8 fifo_pop(can_drv_fifo *this, can_drv_msg *msg);
static uint8 fifo_is_empty(const can_drv_fifo *this);
static uint8 fifo_is_full(const can_drv_fifo *this);

/*******************************************************************************
**                      Private Variable Definitions                          **
*******************************************************************************/
static can_drv_fifo can_drv_recv_fifo[CAN_DRV_INSTS_NUMS];
/*******************************************************************************
**                      Private Function Definitions                         **
*******************************************************************************/
static void fifo_init(can_drv_fifo *this)
{
	this->used = 0U;
	this->free = 0U;
}

static uint8 fifo_push(can_drv_fifo *this, can_drv_msg *msg)
{
	can_drv_msg *buf = this->buf;
	uint16 free = this->free;

	if (msg == NULL)
	{
		return FALSE;
	}

	if (fifo_is_full(this) == TRUE)
	{
		return FALSE;
	}

	memcpy(buf + free, msg, sizeof(can_drv_msg));
	this->free++;
	this->free %= CAN_DRV_RECV_BUF_SIZE;
	return TRUE;
}

static uint8 fifo_pop(can_drv_fifo *this, can_drv_msg *msg)
{
	can_drv_msg *buf = this->buf;
	uint16 used = this->used;

	if (msg == NULL)
	{
		return FALSE;
	}

	if (fifo_is_empty(this) == TRUE)
	{
		return FALSE;
	}

	memcpy(msg, buf + used, sizeof(can_drv_msg));
	this->used++;
	this->used %= CAN_DRV_RECV_BUF_SIZE;
	return TRUE;
}

static uint8 fifo_is_empty(const can_drv_fifo *this)
{
	if (this->used == this->free)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static uint8 fifo_is_full(const can_drv_fifo *this)
{
	uint16 used = this->used;
	uint16 free = this->free;

	if ((free + 1) % CAN_DRV_RECV_BUF_SIZE == used)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*******************************************************************************
**                      Global Variable Definitions                           **
*******************************************************************************/

/*******************************************************************************
**                      Global Function Definitions                           **
*******************************************************************************/
void can_drv_init(void)
{
	uint8 instance = 0;
	for (instance = 0; instance < CAN_DRV_INSTS_NUMS; instance++)
	{
		fifo_init(&can_drv_recv_fifo[instance]);
	}
}

void can_drv_task(void)
{
	uint8 instance = 0;
	can_drv_msg msg = {0};

	for (instance = 0; instance < CAN_DRV_INSTS_NUMS; instance++)
	{
		while (1)
		{
			if (fifo_pop(&can_drv_recv_fifo[instance], &msg) == TRUE)
			{
				if (CAN_DRV_IS_NM_MSG(msg.msg_id))
				{
					//can_nm_msg_process(instance, &msg);
				}
				else if (CAN_DRV_IS_TP_MSG(msg.msg_id))
				{
					//can_tp_msg_process(instance, &msg);
				}
				else
				{
					//can_app_msg_process(instance, &msg);
				}
			}
			else
			{
				break;
			}
		}
	}
}

void can_drv_send_msg(uint8 instance, can_drv_msg *msg)
{
}

void can_drv_receive_msg(uint8 instance, can_drv_msg *msg)
{
	if (instance < CAN_DRV_INSTS_NUMS)
	{
		fifo_push(&can_drv_recv_fifo[instance], msg);
	}
}
