/*
 *  millis.c
 *
 *  Created on: 2016. 7. 13.
 *      Author: Baram
 */

#include "hw.h"
#include "millis.h"











void millisInit()
{
}

uint32_t millis()
{
  return hwGetTick();
}
