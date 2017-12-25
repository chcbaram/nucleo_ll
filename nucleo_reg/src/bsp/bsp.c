/*
 * bsp.c
 *
 *  Created on: 2017. 3. 10.
 *      Author: Baram
 */


#include "bsp.h"
#include "core/millis.h"



void SystemClock_Config(void);
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


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 40
  *            PLL_R                          = 2
  *            PLL_P                          = 7
  *            PLL_Q                          = 4
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
#if 0
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* MSI is enabled after System reset, activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
#endif
}


