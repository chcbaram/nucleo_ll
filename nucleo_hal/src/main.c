/*
 * main.c
 *
 *  Created on: 2017. 3. 19.
 *      Author: baram
 */
#include "main.h"



void mainInit(void);



void led_isr(void *arg)
{
  ledToggle(0);
}



int main(void)
{
  mainInit();


  apMain();


  return 0;
}

void mainInit(void)
{
  bspInit();
  hwInit();
  apInit();
}
