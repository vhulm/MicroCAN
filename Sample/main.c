#include <unistd.h>
#include <stdio.h>
#include "types.h"
#include "can_drv.h"
#include "m_can_il.h"
#include "m_can_impl.h"
#include "m_can.h"
#include "can_app.h"

int main()
{
    printf("Test start!\n");
    can_drv_init();
    can_app_init();

    while (1)
    {
        can_drv_task();
        can_app_task();

        usleep(1000 * 10);
    }

    return 0;
}