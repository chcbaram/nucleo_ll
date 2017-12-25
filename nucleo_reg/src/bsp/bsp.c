/*
 * bsp.c
 *
 *  Created on: 2017. 3. 10.
 *      Author: Baram
 */


#include "bsp.h"
#include "core/millis.h"




bool bspClockInit(void);


void bspInit(void)
{
  NVIC_SetPriorityGrouping(3);      // NVIC_PRIORITYGROUP_4
  SysTick_Config(4000000 / 1000);   // 4Mhz / 1000 = 1ms
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));


  bspClockInit();

  SysTick_Config(80000000 / 1000);   // 80Mhz / 1000 = 1ms


  //__HAL_RCC_GPIOA_CLK_ENABLE();
  //__HAL_RCC_GPIOB_CLK_ENABLE();
  //__HAL_RCC_GPIOC_CLK_ENABLE();

  SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN);
  SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN);
  SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN);
}

void bspDeinit()
{
  //HAL_RCC_DeInit();
  //HAL_DeInit();
}

bool bspClockInit(void)
{
  bool ret;
  uint32_t pre_time;


  // MSI oscillator ON
  RCC->CR |= RCC_CR_MSION;

  // Wait for MSI oscillator ready
  ret = false;
  pre_time = millis();
  while(millis()-pre_time < 50)
  {
    if (RCC->CR & RCC_CR_MSIRDY)
    {
      ret = true;
      break;
    }
  }

  MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_4WS);


  // Disable the main PLL.
  CLEAR_BIT(RCC->CR, RCC_CR_PLLON);

  ret = false;
  pre_time = millis();
  while(millis()-pre_time < 50)
  {
    if ((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
      ret = true;
      break;
    }
  }

  // VCO Input  = 4Mhz   / PLLM =   4Mhz
  // VCO Output = 4Mhz   * PLLN = 160Mhz
  // PLL Ouput  = 160Mhz / PLLR =  80Mhz
  RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_MSI       // 4Mhz
              |  ( 0<<RCC_PLLCFGR_PLLM_Pos)   // PLLM = 1
              |  (40<<RCC_PLLCFGR_PLLN_Pos)   // PLLN = 40
              |  ( 0<<RCC_PLLCFGR_PLLR_Pos)   // PLLR = 2
              |  ( 1<<RCC_PLLCFGR_PLLQ_Pos)   // PLLQ = 4
              |  ( 0<<RCC_PLLCFGR_PLLP_Pos)   // PLLP = 7
              ;

  // Enable the main PLL.
  SET_BIT(RCC->CR, RCC_CR_PLLON);

  // Enable PLL System Clock output.
  SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLREN);


  ret = false;
  pre_time = millis();
  while(millis()-pre_time < 50)
  {
    if (RCC->CR & RCC_CR_PLLRDY)
    {
      ret = true;
      break;
    }
  }

  // HCLK(Hz)   = 80000000
  // SYSCLK(Hz) = 80000000
  MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1);

  // PLL as SysClk
  MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);

  ret = false;
  pre_time = millis();
  while(millis()-pre_time < 50)
  {
    if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL)
    {
      ret = true;
      break;
    }
  }

  // APB1 Prescaler = 1
  // PCLK1(Hz) = 80000000
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV1);

  // APB2 Prescaler = 1
  // PCLK2(Hz) = 80000000
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV1);


  return ret;
}



