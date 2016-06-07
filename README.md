# STMNono

### Overview
> Projekt dotyczyć będzie zdalnie sterowanego pojazdu. Pojazd ma być sterowany za pomocą smartfona.

### Description
> Podstawka użyta w pojeździe zostanie wyciągnięta ze starego zepsutego zdalnie sterowanego samochodzika. Sterowanie odbywać się będzie za pomocą smartfona, a konkretniej zainstalowanej w nim aplikacji wykorzystującej wbudowany w telefon moduł bluetooth. Po stronie pojazdu wykorzystany zostanie dwukanałowy moduł sterowania silnikami DRV8835 , Moduł Bluetooth Master/Slave HC05 , oraz zarządzająca nimi płytka STM32F407VG. Komunikaty sterowania pojazdem przesyłane będą w sposób szeregowy ze smartfona wprost do modułu bluetooth, a odczytywane przez STM32F407VG dane będą interpretowane  i w odpowiedni sposób przekierowywane do modułu sterowania silnikami.

### Tools
> W projekcie zostanie użyte środowisko Visual Studio , Coocox oraz języki programowania C, Java.

### How to run
> ...

### How to compile
> Pliki potrzebne do załączenia:
-tm_stm32f4_delay.h
-tm_stm32f4_delay.c
-tm_stm32f4_disco.h
-tm_stm32f4_disco.c
-tm_stm32f4_hcsr04.h
-tm_stm32f4_hcsr04.c

### How to connect
>	  [STEROWANIE SILNIKAMI]
	 
	  KANAL 1 - PB6 ( KOLO 1 )
	  KANAL 2 - PB7 ( KOLO 1 )
	  KANAL 3 - PB8 ( KOLO 2 )
	  KANAL 4 - PB9 ( KOLO 2 )
	 
	  GDY CHCEMY ROZKRECIC KOLA W DANYM KIERUNKU:
	  PIERW PODLACZAMY DO MASY PIN SILNIKA NA KTORY NIE POWEDRUJE PWM
	  NASTEPNIE KONFIGURUJEMY PIN SILNIKA NA KTORY POJDZIE SYGNAL PWM
	  OSTATECZNIE USTAWIAMY WARTOSC PROCENTOWA WYPELNIENIA KANALU DLA PWM
	 
	  [OBSŁUGA BLUETOOTH]
	 
	  STM USART3 TX PC10 - PODŁĄCZONE DO RX HC-05
	  STM USART3 RX PC11 - PODŁĄCZONE DO TX HC-05
	  VCC - PODŁĄCZANE POD 5V
	  GND - PODŁĄCZANE POD GND
	 
	  [CZUJNIKI ODLEGLOSCIOWE]
	 
	  PRZOD (SENSOR_W): ECHO -> GPIOD GPIO_Pin_6,  TRIG -> GPIOD GPIO_Pin_5 ( ZASILANIE Z PŁYTKI: VCC - 5V, GND - GND )
	  TYL   (SENSOR_S): ECHO -> GPIOD GPIO_Pin_14, TRIG -> GPIOD GPIO_Pin_13
	  LEWO	 (SENSOR_A): ECHO -> GPIOD GPIO_Pin_12, TRIG -> GPIOD GPIO_Pin_11
	  PRAWO (SENSOR_D): ECHO -> GPIOD GPIO_Pin_10, TRIG -> GPIOD GPIO_Pin_9
	 

### Future improvements
> ...

### Attributions
> ...

### License
> ...

### Credits
> The project was conducted during the Microprocessor Lab course held by the Institute of Control and Information Engineering, Poznan University of Technology.
Supervisor: Mateusz Korolow/Daniel Hildebrand
