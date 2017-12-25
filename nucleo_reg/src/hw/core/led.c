/*
 *  led.c
 *
 *  Created on: 2016. 7. 13.
 *      Author: Baram
 */
#include "hw.h"
#include "led.h"



typedef struct
{
  GPIO_TypeDef *port;
  uint16_t      pin_number;
  bool          on_state;
  bool          off_state;
} led_port_t;


static led_port_t led_port_tbl[LED_CH_MAX] =
{
    {GPIOB, 3, false, true}
};



int ledCmdif(int argc, char **argv);




void ledInit(void)
{
  uint32_t i;


  for (i=0; i<LED_CH_MAX; i++)
  {
    // General purpose output mode
    //
    led_port_tbl[i].port->MODER &= ~(3<<(led_port_tbl[i].pin_number*2));
    led_port_tbl[i].port->MODER |=  (1<<(led_port_tbl[i].pin_number*2));

    // Output push-pull (reset state)
    //
    led_port_tbl[i].port->OTYPER &= ~(1<<led_port_tbl[i].pin_number);

    ledOff(i);
  }


  //cmdifAdd("led", ledCmdif);
}

void ledOn(uint8_t ch)
{
  if (ch >= LED_CH_MAX) return;

  if (led_port_tbl[ch].on_state == true)
  {
    led_port_tbl[ch].port->BSRR = (1<<led_port_tbl[ch].pin_number);
  }
  else
  {
    led_port_tbl[ch].port->BRR  = (1<<led_port_tbl[ch].pin_number);
  }
}

void ledOff(uint8_t ch)
{
  if (ch >= LED_CH_MAX) return;

  if (led_port_tbl[ch].off_state == true)
  {
    led_port_tbl[ch].port->BSRR = (1<<led_port_tbl[ch].pin_number);
  }
  else
  {
    led_port_tbl[ch].port->BRR  = (1<<led_port_tbl[ch].pin_number);
  }
}

void ledToggle(uint8_t ch)
{
  if (ch >= LED_CH_MAX) return;

  led_port_tbl[ch].port->ODR ^= (1<<led_port_tbl[ch].pin_number);
}


#if 0
//-- ledCmdif
//
int ledCmdif(int argc, char **argv)
{
  bool ret = true;
  uint8_t number;


  if (argc == 3)
  {
    number = (uint8_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);

    if(strcmp("on", argv[1]) == 0)
    {
      ledOn(number);
    }
    else if(strcmp("off", argv[1])==0)
    {
      ledOff(number);
    }
    else if(strcmp("toggle", argv[1])==0)
    {
      ledToggle(number);
    }
    else if(strcmp("demo", argv[1])==0)
    {
      while(cmdifRxAvailable() == 0)
      {
        ledToggle(number);
        delay(200);
        cmdifPrintf("led toggle %d\n", number);
      }
    }
    else
    {
      ret = false;
    }
  }
  else
  {
    ret = false;
  }

  if (ret == false)
  {
    cmdifPrintf( "led on/off/toggle/demo number ...\n");
  }

  return 0;
}
#endif
