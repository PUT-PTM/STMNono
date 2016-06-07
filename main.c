#include "stm32f4xx_tim.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

#include "stm32f4xx.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_hcsr04.h"



/* DEKLARACJA CZUJNIKOW ODLEGLOSCIOWYCH */
TM_HCSR04_t SENSOR_W, SENSOR_S, SENSOR_A, SENSOR_D;

/* FLAGA INFORMUJ¥CA O AKTUALNIE WYKONYWANYM MANEWRZE WYMIJANIA PRZESZKODY - NIE OBS£UGIWANE W TYM CZASIE KOM. BLUETOOTH */
/* ODLEG£OSCI PRZY JAKICH CZUJNIKI ZAOBSERWUJA PRZESZKODE PRZED/ZA SOBA I PO OBU BOKACH */
int Wymijanie = 0, cmGD = 20, cmLP = 10;

/* FLAGA MOWIACA O TYM CZY A JESLI TAK TO W KTORA STRONE KRECA SIE KOLA */
char ruch = ' ';

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

	char komunikat = (char) USART3->DR;

	/* OBS£UGUJEMY KOMUNIKAT STEROWANIA TYLKO JESLI NIE WKONUJEMY AKTUALNIE MANEWRU WYPRZEDZANIA */
	if(Wymijanie == 0 && komunikat != ruch) SYGNAL(komunikat);

	NVIC_ClearPendingIRQ(USART3_IRQn);
}

void wyminPrawa(int cm, int delay)
{
	/* USTAWIAMY FLAGÊ INFORMUJ¥CA O AKTUALNIE WYKONYWANYM MANEWRZE WYMIJANIA */
	Wymijanie = 1;

	int ile = 0, ms = 50;

	/* OBRACAMY POJAZD W PRAWO
	 *
	 * [P]
	 * ==>
	 *
	 * */
	SYGNAL('d');
	TM_HCSR04_Read(&SENSOR_A);
	while(SENSOR_A.Distance > cm) {TM_HCSR04_Read(&SENSOR_A); ms += delay; Delayms(delay);}
	Delayms(50);
	SYGNAL('r');

	/* PORUSZAMY POJAZD DO PRZODU
	 *
	 * [P]
	 *   ==>
	 *
	 * */
	SYGNAL('w');
	TM_HCSR04_Read(&SENSOR_A);
	while(SENSOR_A.Distance <= cm) {TM_HCSR04_Read(&SENSOR_A); Delayms(delay);}
	Delayms(50);
	SYGNAL('r');


	/* OBRACAMY POJAZD W LEWO
	 *
	 * [P]
	 *    ^
	 *    |
	 *
	 * */
	SYGNAL('a');
	Delayms(ms);
	SYGNAL('r');

	/* PORUSZAMY POJAZD DO PRZODU
	 *
	 *	  ^
	 * 	  |
	 * [P]
	 *
	 * */
	SYGNAL('w');
	while(1) {TM_HCSR04_Read(&SENSOR_A); if(SENSOR_A.Distance <= cm) ile = 1; else if(ile == 1) break; Delayms(delay);};
	SYGNAL('r');

	/* USTAWIAMY FLAGÊ INFORMUJ¥CA O ZAKOÑCZONYM MANEWRZE WYMIJANIA */
	Wymijanie = 0;
}

void wyminLewa(int cm, int delay)
{
	/* USTAWIAMY FLAGÊ INFORMUJ¥CA O AKTUALNIE WYKONYWANYM MANEWRZE WYMIJANIA */
	Wymijanie = 1;

	int ile = 0, ms = 50;

	/* OBRACAMY POJAZD W LEWO
	 *
	 * [P]
	 * <==
	 *
	 * */
	SYGNAL('a');
	TM_HCSR04_Read(&SENSOR_D);
	while(SENSOR_D.Distance > cm) {TM_HCSR04_Read(&SENSOR_D); ms += delay; Delayms(delay);}
	Delayms(50);
	SYGNAL('r');

	/* PORUSZAMY POJAZD DO PRZODU
	 *
	 *    [P]
	 * <==
	 *
	 * */
	SYGNAL('w');
	TM_HCSR04_Read(&SENSOR_D);
	while(SENSOR_D.Distance <= cm) {TM_HCSR04_Read(&SENSOR_D); Delayms(delay);}
	Delayms(50);
	SYGNAL('r');


	/* OBRACAMY POJAZD W PRAWO
	 *
	 *  [P]
	 * ^
	 * |
	 *
	 * */
	SYGNAL('d');
	Delayms(ms);
	SYGNAL('r');

	/* PORUSZAMY POJAZD DO PRZODU
	 *
	 * ^
	 * |
	 *  [P]
	 *
	 * */
	SYGNAL('w');
	while(1) {TM_HCSR04_Read(&SENSOR_D); if(SENSOR_D.Distance <= cm) ile = 1; else if(ile == 1) break; Delayms(delay);};
	SYGNAL('r');

	/* USTAWIAMY FLAGÊ INFORMUJ¥CA O ZAKOÑCZONYM MANEWRZE WYMIJANIA */
	Wymijanie = 0;
}

/* ODEBRANIE SYGNA£U STEROWANIA */
void SYGNAL(char znak)
{
	if(znak != 'r') SYGNAL('r');
	switch(znak)
	{
	case 'w': TM_HCSR04_Read(&SENSOR_W);

		if(SENSOR_W.Distance > cmGD)
		{
			TM_DISCO_LedOff(LED_ALL);
			TM_DISCO_LedOn(LED_BLUE);

			TM_KANAL_Init(2);
			TM_KANAL_Init(4);

			TM_PWM_Set(2, 255);
			TM_PWM_Set(4, 255);

			ruch = 'w';
		}

		break;
	case 's': TM_HCSR04_Read(&SENSOR_S);

		if(SENSOR_S.Distance > cmGD)
		{
			TM_DISCO_LedOff(LED_ALL);
			TM_DISCO_LedOn(LED_ORANGE);

			TM_KANAL_Init(1);
			TM_KANAL_Init(3);

			TM_PWM_Set(1, 255);
			TM_PWM_Set(3, 255);

			ruch = 's';
		}

		break;
	case 'a': TM_HCSR04_Read(&SENSOR_A);

		if(SENSOR_A.Distance > cmLP)
		{
			TM_KANAL_Init(2);
			TM_KANAL_Init(3);

			TM_PWM_Set(2, 255);
			TM_PWM_Set(3, 255);

			ruch = 'a';
		}

		break;
	case 'd': TM_HCSR04_Read(&SENSOR_D);

		if(SENSOR_D.Distance > cmLP)
		{
			TM_KANAL_Init(1);
			TM_KANAL_Init(4);

			TM_PWM_Set(1, 255);
			TM_PWM_Set(4, 255);

			ruch = 'd';
		}

		break;
	default:

		TM_DISCO_LedOff(LED_ALL);

		TM_KANAL_Ground(1);
		TM_KANAL_Ground(2);
		TM_KANAL_Ground(3);
		TM_KANAL_Ground(4);

		ruch = 'r';

		break;
	}
}


int main(void)
{
	SystemInit();

	/* SILNIKI */
	TM_TIMER_Init();
	SYGNAL('r');

	/* BLUETOOTH */
	USRT_Init();

	/* OPÓNIENIE */
	TM_DELAY_Init();

	/* DIODY */
	TM_DISCO_LedInit();

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
	 * VCC - POD£¥CZANE POD 5V
	 * GND - POD£¥CZANE POD GND
	 *
	 * [CZUJNIKI ODLEGLOSCIOWE]
	 *
	 * PRZOD (SENSOR_W): ECHO -> GPIOD GPIO_Pin_6,  TRIG -> GPIOD GPIO_Pin_5 ( ZASILANIE Z P£YTKI: VCC - 5V, GND - GND )
	 * TYL   (SENSOR_S): ECHO -> GPIOD GPIO_Pin_14, TRIG -> GPIOD GPIO_Pin_13
	 * LEWO	 (SENSOR_A): ECHO -> GPIOD GPIO_Pin_12, TRIG -> GPIOD GPIO_Pin_11
	 * PRAWO (SENSOR_D): ECHO -> GPIOD GPIO_Pin_10, TRIG -> GPIOD GPIO_Pin_9
	 *
	 */

	/* JESLI KTORYS Z CZUJNIKOW NIE JEST GOTOWY DO DZIALANIA WSZYSTKIE DIODY MRUGAJA */
	if(!TM_HCSR04_Init(&SENSOR_W, GPIOD, GPIO_PIN_6, GPIOD, GPIO_PIN_5)   ||
	   !TM_HCSR04_Init(&SENSOR_S, GPIOD, GPIO_PIN_14, GPIOD, GPIO_PIN_13) ||
	   !TM_HCSR04_Init(&SENSOR_A, GPIOD, GPIO_PIN_12, GPIOD, GPIO_PIN_11) ||
	   !TM_HCSR04_Init(&SENSOR_D, GPIOD, GPIO_PIN_10, GPIOD, GPIO_PIN_9)
	   )
	{
		while(1)
		{
			TM_DISCO_LedToggle(LED_ALL);
			Delayms(100);
		}
	}

	/* W CZASIE RZECZYWISTYM SPRAWDZAMY STANY CZUJNIKOW JESLI TYLKO NIE JESTESMY W TRAKCIE MANEWRU WYMIJANIA */
	while(1)
	{
		if(Wymijanie == 0)
		{
			switch(ruch)
			{
			case 'w':
				TM_HCSR04_Read(&SENSOR_W); if(SENSOR_W.Distance < cmGD)
					wyminPrawa(cmGD, 100);
				break;
			case 's':
				TM_HCSR04_Read(&SENSOR_S); if(SENSOR_S.Distance < cmGD) SYGNAL('r');
				break;
			case 'a':
				TM_HCSR04_Read(&SENSOR_A); if(SENSOR_A.Distance < cmLP) SYGNAL('r');
				break;
			case 'd':
				TM_HCSR04_Read(&SENSOR_D); if(SENSOR_D.Distance < cmLP) SYGNAL('r');
				break;
			default: break;
			}
		}
	}
}
