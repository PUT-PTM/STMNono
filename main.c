#include "stm32f4xx_conf.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"

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

    TIM_BaseStruct.TIM_Prescaler = 0;
    TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_BaseStruct.TIM_Period = 1049; /* 80kHz PWM 1049 PERIOD */
    TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_BaseStruct.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM4, &TIM_BaseStruct);

    /* W£¥CZENIE TIMERA 4 */

    TIM_Cmd(TIM4, ENABLE);
}
void TM_PWM_Set(int kanal, float value)
{
	/* PRZELICZNIK PROPORCJONALNY DLA ZAKRESU PRACY SILNIKÓW */
	value = 3.30 + (value*0.08);

    TIM_OCInitTypeDef TIM_OCStruct;

    /* OGÓLNE USTAWIENIA */

    TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;

    TIM_OCStruct.TIM_Pulse = (8400 * value)/100 - 1; // value% CYKLU PRACY KANA£U

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

int main(void)
{
	/*
	 *
	 *
	 * KANAL 1 - PB6 ( KOLO 1 - NAJPIERW SYGNAL PWM, POZNIEJ MASA )
	 * KANAL 2 - PB7 ( KOLO 2 - NAJPIERW SYGNAL PWM, POZNIEJ MASA )
	 * KANAL 3 - PB8 ( KOLO 1 - NAJPIERW MASA, POZNIEJ SYGNAL PWM )
	 * KANAL 4 - PB9 ( KOLO 2 - NAJPIERW MASA, POZNIEJ SYGNAL PWM )
	 *
	 * GDY CHCEMY ROZKRECIC KOLA W DANYM KIERUNKU:
	 * 		PIERW PODLACZAMY DO MASY PIN SILNIKA NA KTORY NIE POWEDRUJE PWM
	 * 		NASTEPNIE KONFIGURUJEMY PIN SILNIKA NA KTORY POJDZIE SYGNAL PWM
	 * 		OSTATECZNIE USTAWIAMY WARTOSC PROCENTOWA WYPELNIENIA KANALU DLA PWM
	 *
	 */

	SystemInit();
	TM_TIMER_Init(); // <-- URUCHOMIENIE TIMERA DLA PWM

	/*POD£¥CZENIE DO MASY KANA£ÓW TIMERA SYGNA£ÓW STERUJ¥CYCH KIERUNKIEM OBROTU JEDNEGO I DRUGIEGO KO£A */
	TM_KANAL_Ground(3);
	TM_KANAL_Ground(4);

	/*KONFIGURUJEMY KANA£ 1 I 2 DLA MO¯LIWOCI WYPROWADZANIA SYGNA£U AF - PWM*/
	TM_KANAL_Init(1);
	TM_KANAL_Init(2);

	/*NA KANA£Y 1 I 2 PUSZCZAMY SYGNA£ PWM W SKALI 0-100*/
	TM_PWM_Set(1,100);
	TM_PWM_Set(2,100);

	/*PRZED ZMIAN¥ KIERUNKU OBROTU KÓ£ ODCZEKUJEMY JAKIS CZAS*/
	int i = 0;
	for(;i < 50000000; i++) {}

	/*KANALY 1 I 2 NA KTORE DO TEJ PORY SKONFIGUROWANY BYL PWM PODLACZAMY DO MASY*/
	TM_KANAL_Ground(1);
	TM_KANAL_Ground(2);

	/*KONFIGURUJEMY KANA£ 3 I 4 DLA MO¯LIWOCI WYPROWADZANIA SYGNA£U AF - PWM*/
	TM_KANAL_Init(3);
	TM_KANAL_Init(4);

	/*NA KANA£Y 3 I 4 PUSZCZAMY SYGNA£ PWM W SKALI 0-100*/
	TM_PWM_Set(3,100);
	TM_PWM_Set(4,100);

	while(1)
	{
	}

}
