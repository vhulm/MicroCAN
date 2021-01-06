{% macro msg_symbol(msg) -%}{{ inst.can_name.lower() }}_{{ msg.snake_name }}{%- endmacro %}{% macro signal_symbol(msg, signal) -%}{{ inst.can_name.lower() }}_{{ msg.snake_name }}_{{ signal.snake_name }}{%- endmacro %}/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include "types.h"
#include "{{ inst.can_name.lower() }}_il.h"
#include "can_app.h"

/*******************************************************************************
**                      Private Macro Definitions                             **
*******************************************************************************/

/*******************************************************************************
**                      Private Type Definitions                              **
*******************************************************************************/

/*******************************************************************************
**                      Private Function Declarations                         **
*******************************************************************************/
{% for msg in db.recv_msgs %}static void {{ msg_symbol(msg) }}_process(const uint8_t *src_p);
{% endfor %}
/*******************************************************************************
**                      Private Variable Definitions                          **
*******************************************************************************/
{% for msg in db.send_msgs %}static can_drv_msg {{ msg_symbol(msg) }}_drv_msg = {{ '{' }}{{ "0x%X" % msg.frame_id }}{{ '}' }};
{% endfor %}

static can_app_msg_id_ind_cb_mapping {{ inst.can_name.lower() }}_msg_id_ind_cb_map[CAN_APP_{{ inst.can_name.upper() }}_RECV_MSG_NUMS] =
{{ '{' }}{% for msg in db.recv_msgs %}
    {{ '{' }}{{ "0x%X" % msg.frame_id }}, {{ msg_symbol(msg) }}_process{{ '}' }},{% endfor %}
{{ '}' }};
/*******************************************************************************
**                      Global Variable Definitions                           **
*******************************************************************************/
{% for msg in db.recv_msgs %}struct {{ msg_symbol(msg) }}_t {{ msg_symbol(msg) }} = {0};
struct {{ msg_symbol(msg) }}_t {{ msg_symbol(msg) }}_bak = {0};
{% endfor %}

{% for msg in db.send_msgs %}struct {{ msg_symbol(msg) }}_t {{ msg_symbol(msg) }} = {0};
{% endfor %}

/*******************************************************************************
**                      Private Function Definitions                         **
*******************************************************************************/
{% for msg in db.recv_msgs %}{% for signal in msg.signals %}extern void {{ signal_symbol(msg, signal) }}_ind(void);
{% endfor %}
{% endfor %}
{% for msg in db.recv_msgs %}
static void {{ msg_symbol(msg) }}_process(const uint8_t *src_p)
{
    {{ msg_symbol(msg) }}_unpack(&{{ msg_symbol(msg) }}, src_p, 8);
    {% for signal in msg.signals %}
    {{ signal_symbol(msg, signal) }}_ind();{% endfor %}

    {{ msg_symbol(msg) }}_bak = {{ msg_symbol(msg) }};
};
{% endfor %}
/*******************************************************************************
**                      Global Function Definitions                           **
*******************************************************************************/
void can_app_{{ inst.can_name.lower() }}_msg_process(can_drv_msg *msg)
{
    int start = 0;
    int end = CAN_APP_{{ inst.can_name.upper() }}_RECV_MSG_NUMS;
    int mid;
    while (start <= end)
    {
        mid = start + (end - start) / 2;
        if ({{ inst.can_name.lower() }}_msg_id_ind_cb_map[mid].msg_id < msg->msg_id)
        {
            start = mid + 1;
        }
        else if ({{ inst.can_name.lower() }}_msg_id_ind_cb_map[mid].msg_id > msg->msg_id)
        {
            end = mid - 1;
        }
        else
        {
            {{ inst.can_name.lower() }}_msg_id_ind_cb_map[mid].msg_ind_cb(&msg->data[0]);
            break;
        }
    }
}
{% for msgs in db.category_send_msgs.items() %}
void can_app_{{ inst.can_name.lower() }}_{{ msgs[0] }}ms_msg_set_sender(uint8 cycle)
{
    static uint16 cnt = 0;
    cnt += cycle;
    if (cnt > {{ msgs[0] }})
    {{ '{' }}
        cnt = 0;
    {% for msg in msgs[1] %}
        {{ msg_symbol(msg) }}_send_msg();{% endfor %}
    {{ '}' }}
}
{% endfor %}
{% for msg in db.send_msgs %}
void {{ msg_symbol(msg) }}_pack_msg()
{
    {{ msg_symbol(msg) }}_pack({{ msg_symbol(msg) }}_drv_msg.data, &{{ msg_symbol(msg) }}, 8);
}
void {{ msg_symbol(msg) }}_send_msg()
{
    can_drv_send_msg({{ inst.can_instance }}, &{{ msg_symbol(msg) }}_drv_msg);
}
{% endfor %}