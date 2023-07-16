#include "stm32f4xx_hal.h"

void GPIO_Config(void);
void ADC_Config(void);
void Systick_Config(void);

ADC_HandleTypeDef myADCtypeDef;
uint32_t myAdcValue;

int main(void)
{
	HAL_Init();
	GPIO_Config();
	ADC_Config();
	Systick_Config();
	
	while(1)
	{
		//Start ADC
		ADC1->CR2 |= ADC_CR2_ADON;
		ADC1->CR2 |= (1<<30);
		//Stabilize the ADC Clock
		uint32_t counter = (ADC_STAB_DELAY_US * (SystemCoreClock/1000000U));
		while(counter != 0U)
		{
			counter--;
		}
		
		//Poll for the end of conversion
		uint32_t tickstart = 0U;
		tickstart = HAL_GetTick();
		while((ADC1->SR&0x2) != 0x2)
		{
			if((HAL_GetTick() - tickstart ) > 10)
			{
				break;
			}
		}
	  //Read ADC Value
		myAdcValue = ADC1->DR;
		//Stop the ADC
		ADC1->CR2 &= ~ADC_CR2_ADON;
		//Clear all flags
		ADC1->SR = 0x00;
		//Delay
		HAL_Delay(100);
	}
}

void GPIO_Config(void)
{
	RCC->AHB1ENR |=0x01;

	GPIOA->MODER |= 0x03;
	GPIOA->PUPDR &= ~(0x00000003);

// ******************************************Configuration with HAL**************************************
	//	__HAL_RCC_GPIOA_CLK_ENABLE();
//	
//	GPIO_InitTypeDef myADCpin;
//	myADCpin.Pin = GPIO_PIN_0;
//	myADCpin.Mode = GPIO_MODE_ANALOG;
//	myADCpin.Pull = GPIO_NOPULL;
//	
//	HAL_GPIO_Init(GPIOA, &myADCpin);
}

void ADC_Config(void)
{
	//Enable EDC Clock
	__HAL_RCC_ADC1_CLK_ENABLE();
	
	//ADC Basic Configuration
	ADC1->CR1 = (0x2 << 24); //Set the reslution to 8 bits
	ADC1->CR2 = (0X1 << 10); // EOC Flag at the emd of regular channel conversion
	ADC1->SMPR2 = 1; //Sample time to 15 clock cycles
	ADC1->SQR1 = (0x0 << 20); //Number of Conversion = 1
	ADC1->SQR3 = 0x00; //Sequencier to convert channel 0 first
	
	//***********************************Code with HAL*******************************************
	
//	__HAL_RCC_ADC1_CLK_ENABLE();
//	
//	myADCtypeDef.Instance = ADC1;
//	myADCtypeDef.Init.Resolution = ADC_RESOLUTION8b;
//	myADCtypeDef.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
//	myADCtypeDef.Init.DataAlign = ADC_DATAALIGN_RIGHT;
//	
//	HAL_ADC_Init(&myADCtypeDef);
//	
//	ADC_ChannelConfTypeDef myADCchannelTypeDef;
//	myADCchannelTypeDef.Channel = ADC_CHANNEL_0;
//	myADCchannelTypeDef.Rank = 1;
//	myADCchannelTypeDef.SamplingTime = ADC_SAMPLETIME_15CYCLES;
//	
//	HAL_ADC_ConfigChannel(&myADCtypeDef, &myADCchannelTypeDef);
}

void Systick_Config(void)
{
	//Set Source Clock and Speed
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	//Systick Interrupt priority and enable
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(SysTick_IRQn);
}

void SysTick_Handler(void)
{
	HAL_IncTick();
}
	