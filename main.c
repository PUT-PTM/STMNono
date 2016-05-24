#include "stm32f4xx_conf.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_exti.h"
#include "misc.h"

int usTime = 0;

/* STEROWANIE SILNIKAMI */
void TM_KANAL_Init(int kanal)
{
	/*
	 *
	 *  TIM4 CH1 - PB6 / PD12
	 *  TIM4 CH2 - PB7 / PD13
	 *  TIM4 CH3 - PB8 / PD14
	 *  TIM4 CH4 - PB9 / PD15
	 *
	 * */

     GPIO_InitTypeDef GPIO_InitStruct;

     /* POD£¥CZENIE ZEGARA DO GPIOB I GPIOD */

     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

     /* USTAWIENIE PINU JAKO WYJSCIA ANALOGOWEGO DLA PWM */

     switch(kanal)
     {
      case 1: GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4); break;
      case 2: GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4); break;
      case 3: GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM4); break;
      case 4: GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_TIM4); break;
      default:break;
     }

     /* USTAWIENIE PINU */

     switch(kanal)
	 {
	  case 1: GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6; break;
	  case 2: GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7; break;
	  case 3: GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8; break;
	  case 4: GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9; break;
	  default:break;
	 }

     GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
     GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
     GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
     GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
     GPIO_Init(GPIOB, &GPIO_InitStruct);
}
void TM_TIMER_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_BaseStruct;

    /* POD£¥CZENIE ZEGARA DO TIMERA 4 */

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    /* WYPE£NIENIE STRUKTURY INICJALIZACYJNEJ TIMERA 4 */

    TIM_BaseStruct.TIM_Prescaler = (84000000/100000)-1;
    TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_BaseStruct.TIM_Period = (100000/1000)-1; /* 80kHz PWM 1049 PERIOD */
    TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_BaseStruct.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM4, &TIM_BaseStruct);

    /* W£¥CZENIE TIMERA 4 */

    TIM_Cmd(TIM4, ENABLE);
}
void TM_PWM_Set(int kanal, char value)
{
	/* PRZELICZNIK PROPORCJONALNY DLA ZAKRESU PRACY SILNIKÓW */

    TIM_OCInitTypeDef TIM_OCStruct;

    /* OGÓLNE USTAWIENIA */

    TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;

    TIM_OCStruct.TIM_Pulse = (8400 * (float)value/255) - 1; // value% CYKLU PRACY KANA£U

    /* USTAWIENIA WYPE£NIENIA KANA£U TIMERA */

    switch(kanal)
    {
    case 1:
    	TIM_OC1Init(TIM4, &TIM_OCStruct);
    	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    	break;
    case 2:
    	TIM_OC2Init(TIM4, &TIM_OCStruct);
    	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
    	break;
    case 3:
    	TIM_OC3Init(TIM4, &TIM_OCStruct);
    	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
    	break;
    case 4:
    	TIM_OC4Init(TIM4, &TIM_OCStruct);
    	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
    	break;
    default:
    	break;
    }
}
void TM_KANAL_Ground(int kanal)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	switch(kanal)
	{
	 case 1: GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; break;
	 case 2: GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; break;
	 case 3: GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; break;
	 case 4: GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; break;
	 default:break;
	}

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	switch(kanal)
	{
	 case 1: GPIO_ResetBits(GPIOB, GPIO_Pin_6); break;
	 case 2: GPIO_ResetBits(GPIOB, GPIO_Pin_7); break;
	 case 3: GPIO_ResetBits(GPIOB, GPIO_Pin_8); break;
	 case 4: GPIO_ResetBits(GPIOB, GPIO_Pin_9); break;
	 default:break;
	}
}


/* OPOZNIENIE */
void DELAY_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_BaseStruct;

    /* POD£¥CZENIE ZEGARA DO TIMERA 3 */

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /* WYPE£NIENIE STRUKTURY INICJALIZACYJNEJ TIMERA 3 */

    TIM_BaseStruct.TIM_Prescaler = 0;
    TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_BaseStruct.TIM_Period = 83; /* 1MHz - 1us */
    TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_BaseStruct.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM3, &TIM_BaseStruct);

    /* W£¥CZENIE TIMERA 3 */

    TIM_Cmd(TIM3, ENABLE);

    /* WLACZAMY PRZERWANIA DLA TIMERA 3 */

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
}
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		++usTime;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}
void delay(int us)
{
	usTime = 0;
	while(usTime < us);
}


/* OBS£UGA BLUETOOTH */
void USRT_Init()
{
	// WLACZENIE TAKTOWANIA WYBRANEGO PORTU
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	// WLACZENIE TAKTOWANIA WYBRANEGO UKLADU USART
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	// KONFIGURACJA LINII TX
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// KONFIGURACJA LINII RX
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// KONFIGURACJA USART
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART3, &USART_InitStructure);

	// WLACZENIE UKLADU USART
	USART_Cmd(USART3, ENABLE);

	// KONFIGURACJA PRZERWAN WYWOLYWANYCH W MOMENCIE ODEBRANIA PLIKU PRZEZ BLUETOOTH
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
	NVIC_EnableIRQ(USART3_IRQn);
}
void USART3_IRQHandler(void)
{
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == RESET)
		return;

	wykonaj(USART3->DR);
	NVIC_ClearPendingIRQ(USART3_IRQn);
}

/* OBS£UGA DIOD  I PINU SYGNALU CZUJNIKA ODLEGLOSCIOWEGO PRZEDNIEGO */
void GPIOD_Init()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef DIODY;
	DIODY.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	DIODY.GPIO_Mode = GPIO_Mode_OUT;
	DIODY.GPIO_PuPd = GPIO_PuPd_NOPULL;
	DIODY.GPIO_OType = GPIO_OType_PP;
	DIODY.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &DIODY);

	GPIO_InitTypeDef TRIG;
	TRIG.GPIO_Pin = GPIO_Pin_4;
	TRIG.GPIO_Mode = GPIO_Mode_OUT;
	TRIG.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &TRIG);

	GPIO_InitTypeDef ECHO;
	ECHO.GPIO_Pin = GPIO_Pin_3;
	ECHO.GPIO_Mode = GPIO_Mode_IN;
	ECHO.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &ECHO);
}

/* ODEBRANIE SYGNA£U STEROWANIA */
void wykonaj(char znak)
{
	if(znak != 'r') wykonaj('r');
	switch(znak)
	{
	case 'w':
		GPIO_SetBits(GPIOD, GPIO_Pin_15);
		GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14);
/*
		int czas = 0;
		GPIO_SetBits(GPIOD, GPIO_Pin_4); delay(10);
		GPIO_ResetBits(GPIOD, GPIO_Pin_4);

		while(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_3) == 1) // <-- PODPIÊTE DO ECHO CZUJNIKA DAJE 1 BEZ WCZENIEJSZEGO WYS£ANIA SYGNA£U
		{
			delay(1);
			GPIO_ToggleBits(GPIOD, GPIO_Pin_13); delay(1000);
			czas++;
		}
		if(czas > 0)
		{
			float cm = (float)czas/58; czas = 0;
			if(cm < 4)
			{
				...
			}
			break;
		}
*/
		TM_KANAL_Init(2);
		TM_KANAL_Init(4);

		TM_PWM_Set(2, 255);
		TM_PWM_Set(4, 255);

		break;
	case 's':
		GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
		GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_14|GPIO_Pin_15);

		TM_KANAL_Init(1);
		TM_KANAL_Init(3);

		TM_PWM_Set(1, 255);
		TM_PWM_Set(3, 255);

		break;
	case 'a':
		GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
		GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15);

		TM_KANAL_Init(1);
		TM_KANAL_Init(4);

		TM_PWM_Set(1, 255);
		TM_PWM_Set(4, 255);

		break;
	case 'd':
		GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
		GPIO_ResetBits(GPIOD, GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);

		TM_KANAL_Init(2);
		TM_KANAL_Init(3);

		TM_PWM_Set(2, 255);
		TM_PWM_Set(3, 255);

		break;
	default:
		TM_KANAL_Ground(1);
		TM_KANAL_Ground(2);
		TM_KANAL_Ground(3);
		TM_KANAL_Ground(4);

		break;
	}
	/////////////////////////////////////////////////////////////////////////////////////////
}



int main(void)
{
	SystemInit();
	TM_TIMER_Init();
	USRT_Init();
	GPIOD_Init();
	DELAY_Init();

	wykonaj('r');

	/*
	 * [STEROWANIE SILNIKAMI]
	 *
	 * KANAL 1 - PB6 ( KOLO 1 )
	 * KANAL 2 - PB7 ( KOLO 1 )
	 * KANAL 3 - PB8 ( KOLO 2 )
	 * KANAL 4 - PB9 ( KOLO 2 )
	 *
	 * GDY CHCEMY ROZKRECIC KOLA W DANYM KIERUNKU:
	 * 		PIERW PODLACZAMY DO MASY PIN SILNIKA NA KTORY NIE POWEDRUJE PWM
	 * 		NASTEPNIE KONFIGURUJEMY PIN SILNIKA NA KTORY POJDZIE SYGNAL PWM
	 * 		OSTATECZNIE USTAWIAMY WARTOSC PROCENTOWA WYPELNIENIA KANALU DLA PWM
	 *
	 * [OBS£UGA BLUETOOTH]
	 *
	 * STM USART3 TX PC10 - POD£¥CZONE DO RX HC-05
	 * STM USART3 RX PC11 - POD£¥CZONE DO TX HC-05
	 *
	 */

	TM_KANAL_Ground(1);
	TM_KANAL_Ground(2);
	TM_KANAL_Ground(3);
	TM_KANAL_Ground(4);




	while(1)
	{

	}
}
