#include <delay_us.h>
#include "fsl_common.h"


void delay_us(uint32_t n)
{

    SDK_DelayAtLeastUs(n, SystemCoreClock);
}
