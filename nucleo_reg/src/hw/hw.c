/*
 *  hw.c
 *
 *  Created on: 2016. 7. 13.
 *      Author: Baram
 */

#include "hw.h"


extern uint32_t systic_count;




void hwInit(void)
{
  ledInit();
}


uint32_t hwGetTick(void)
{
  return systic_count;
}

