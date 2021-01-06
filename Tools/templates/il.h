#ifndef {{ inst.can_name.upper() }}_IL_H
#define {{ inst.can_name.upper() }}_IL_H
#include "types.h"
#include "can_drv.h"
#include "{{ inst.can_name.lower() }}.h"

{% macro msg_symbol(msg) -%}{{ inst.can_name.lower() }}_{{ msg.snake_name }}{%- endmacro %}{% macro signal_symbol(msg, signal) -%}{{ inst.can_name.lower() }}_{{ msg.snake_name }}_{{ signal.snake_name }}{%- endmacro %}/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/

/*******************************************************************************
**                      Global Macro Definitions                              **
*******************************************************************************/
#define CAN_APP_{{ inst.can_name.upper() }}_INSTANCE {{ inst.can_instance }}
#define CAN_APP_{{ inst.can_name.upper() }}_SEND_MSG_NUMS {{ inst.send_msg_nums }}
#define CAN_APP_{{ inst.can_name.upper() }}_RECV_MSG_NUMS {{ inst.recv_msg_nums }}

{% for msg in db.recv_msgs %}{% for signal in msg.signals %}#define {{ signal_symbol(msg, signal) }}_changed() ({{ msg_symbol(msg) }}.{{ signal.snake_name}} != {{ msg_symbol(msg) }}_bak.{{ signal.snake_name}})
{% endfor %}
{% endfor %}
/*******************************************************************************
**                      Global Type Definitions                               **
*******************************************************************************/

/*******************************************************************************
**                      Global Variable Declarations                          **
*******************************************************************************/
/*Global receive message structure*/
{% for msg in db.recv_msgs %}extern struct {{ msg_symbol(msg) }}_t {{ msg_symbol(msg) }};
extern struct {{ msg_symbol(msg) }}_t {{ msg_symbol(msg) }}_bak;

{% endfor %}
/*Global send message structure*/
{% for msg in db.send_msgs %}extern struct {{ msg_symbol(msg) }}_t {{ msg_symbol(msg) }};
{% endfor %}
/*******************************************************************************
**                      Global Function Declarations                          **
*******************************************************************************/
void can_app_{{ inst.can_name.lower() }}_msg_process(can_drv_msg *msg);

{% for msgs in db.category_send_msgs.items() %}void can_app_{{ inst.can_name.lower() }}_{{ msgs[0] }}ms_msg_set_sender(uint8 cycle);
{% endfor %}
{% for msg in db.send_msgs %}
void {{ msg_symbol(msg) }}_pack_msg();
void {{ msg_symbol(msg) }}_send_msg();
{% endfor %}
#endif /* {{ inst.can_name.upper() }}_IL_H */
