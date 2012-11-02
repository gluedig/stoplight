/**
  ******************************************************************************
  * @file    Examples/GPIOToggle/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    09/13/2010
  * @brief   Main program body.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "STM32vldiscovery.h"
#include "usart.h"
#include "i2c.h"


/** @addtogroup Examples
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;

/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);
void INTinit(void);


/* Private functions ---------------------------------------------------------*/

void EXTI0_IRQHandler (void)
{
	if ( EXTI_GetFlagStatus(EXTI_Line0) ) {
		Usart1Put(0x21);
		EXTI_ClearFlag(EXTI_Line0);
	}

}
void EXTI2_IRQHandler (void)
{
	if ( EXTI_GetFlagStatus(EXTI_Line2) ) {
		uint8_t acc_int =0;
		Usart1Put(0x23);
		STM32vldiscovery_LEDToggle(LED3);
		if( (acc_int = I2C_ReadShort(0xc, 0x1d))) {
			Usart1WriteAsHex8(acc_int);

			if (acc_int & 0x10) {
				uint8_t pl_status;
				pl_status = I2C_ReadShort(0x10, 0x1d);
				Usart1WriteAsHex8(pl_status);

				STM32vldiscovery_LEDToggle(LED0);
			}
			if (acc_int & 0x20) {
				uint8_t pl_status;
				pl_status = I2C_ReadShort(0x1e, 0x1d);
				Usart1WriteAsHex8(pl_status);
			}

		}
		Usart1WriteNLCR();

		EXTI_ClearFlag(EXTI_Line2);
	}
}

void INTinit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the BUTTON Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

  /* Configure Button pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOB, &GPIO_InitStructure);


    /* Connect Button EXTI Line to Button GPIO Pin */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource2);

    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;

    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;

    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0A;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);

}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  uint8_t count = 1;
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     
       
  /* Configure all unused GPIO port pins in Analog Input mode (floating input
     trigger OFF), this will reduce the power consumption and increase the device
     immunity against EMI/EMC *************************************************/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE, DISABLE);  

  /* Initialize Leds LD3 and LD4 mounted on STM32VLDISCOVERY board */
  STM32vldiscovery_LEDInit(LED3);
  STM32vldiscovery_LEDInit(LED4);
  STM32vldiscovery_LEDInit(LED0);

  //STM32vldiscovery_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
  Usart1Init();
  I2C_Setup();

  Usart1Put(0xa);
  Usart1Put(0xd);
  Delay(0xAFFFF);

  /* set ACTIVE mode */
  I2C_WriteShort(0x2a, 0x1d, 0x0);

  //int active high
  I2C_WriteShort(0x2c, 0x1d, 0x22);

  //P/L detection
  I2C_WriteShort(0x12, 0x1d, 0xff);
  I2C_WriteShort(0x11, 0x1d, 0x40);

  //transient detection
  I2C_WriteShort(0x1d, 0x1d, 0x02);
  I2C_WriteShort(0x1f, 0x1d, 0x5);
  I2C_WriteShort(0x20, 0x1d, 0x5);


  //int enable
  I2C_WriteShort(0x2d, 0x1d, 0x30);

  /* set ACTIVE mode */
  I2C_WriteShort(0x2a, 0x1d, 0x1);


  STM32vldiscovery_LEDOn(LED0);
  INTinit();


//  EXTI_GenerateSWInterrupt(EXTI_Line0);
//  EXTI_GenerateSWInterrupt(EXTI_Line3);
  while (1)
  {
	uint16_t addr = 0;
	uint8_t read_b = 0;
	uint8_t read_chars[2];
	uint8_t read_c = 0;

    /* Turn on LD2 and LD3 */
    STM32vldiscovery_LEDOn(LED4);    

    /* Insert delay */

    Usart1Put(0x30+(count++));
    Usart1Put(0x3e);
    Usart1Put(0x20);
    Delay(0xFF);

    while (read_c < 2) {
    	uint8_t read_ch = 0;
    	read_ch = Usart1Get();
    	Usart1Put(read_ch);
    	Delay(0xFF);

    	if ((read_ch >= 0x30) & (read_ch <= 0x39)) {
    		read_chars[read_c] = read_ch - 0x30;
			read_c++;
    	} else if ((read_ch >= 0x61) & (read_ch <= 0x66)) {
    		read_chars[read_c] = (read_ch - 0x61) + 0xa;
    		read_c++;
    	} else {
    		Usart1Put(0x8);
	    	Delay(0xFF);
    	}
    }
    addr = (read_chars[0] << 4) | read_chars[1];
    Usart1WriteNLCR();

   	read_b = I2C_ReadShort(addr, 0x1D);
   	Usart1WriteAsHex8(read_b);
    Usart1WriteNLCR();

    /* Turn off LD3 and LD4 */
    STM32vldiscovery_LEDOff(LED4);

    /* Insert delay */
    Delay(0xAFFFF);
    if (count > 9)
    	count = 0;

  }

}




/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
