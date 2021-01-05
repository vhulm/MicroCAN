#include <unistd.h>
#include <stdio.h>
#include "types.h"
#include "can_drv.h"

int main()
{
    can_drv_msg msg;
    printf("Test start!\n");

    while (1)
    {
        can_drv_task();
        usleep(1000 * 10);
        can_drv_send_msg(0, &msg);
    }

    return 0;
}