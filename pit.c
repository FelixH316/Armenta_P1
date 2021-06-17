/*
 * @file    pit.c
 * @brief   Configures PIT_CH2->keypad & PIT_CH3->DAC
 * @author	Félix Armenta A - PADTS IECA 3
 * @date	Jun 13th 2021
 * @vers	v0.1
 */

#include "pit.h"

volatile bool pitCh2Flag = false;
volatile bool pitCh3Flag = false;

void PIT2_IRQHandler(void){
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_2, kPIT_TimerFlag);
	pitCh2Flag = true;
	__DSB();
}

void PIT3_IRQHandler(void){
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_3, kPIT_TimerFlag);
	pitCh3Flag = true;
	__DSB();
}

bool getPitCh2Flag(void){
	return pitCh2Flag;
}

void clearPitCh2Flag(void){
	pitCh2Flag = false;
}

bool getPitCh3Flag(void){
	return pitCh3Flag;
}

void clearPitCh3Flag(void){
	pitCh3Flag = false;
}

void initPIT(void){
	/*** PIT GLOBAL CONFIG ***/
	// PIT default config
	pit_config_t pitConfig;
	PIT_GetDefaultConfig(&pitConfig);
	PIT_Init(PIT, &pitConfig);


	/*** PIT CONFIG: KEYPAD ***/
	// Setting PIT ch2 period
	PIT_SetTimerPeriod(PIT, kPIT_Chnl_2, USEC_TO_COUNT(201000U, CLOCK_GetFreq(kCLOCK_BusClk)));

	// Enable PIT2 ISR for ch1
	PIT_EnableInterrupts(PIT, kPIT_Chnl_2, kPIT_TimerInterruptEnable);


	/*** PIT CONFIG: DAC Freq ***/
	// Setting PIT ch3 period
	PIT_SetTimerPeriod(PIT, kPIT_Chnl_3, USEC_TO_COUNT(502500U, CLOCK_GetFreq(kCLOCK_BusClk)));

	// Enable PIT3 ISR for ch1
	PIT_EnableInterrupts(PIT, kPIT_Chnl_3, kPIT_TimerInterruptEnable);


	/*** DAC PIT Priority ***/
	// PIT NVIC enable and priority (DAC freq)
	NVIC_EnableIRQ(PIT3_IRQn);
	NVIC_SetPriority(PIT3_IRQn, 2);

	// Start PIT ch3 (DAC freq)
	PIT_StartTimer(PIT, kPIT_Chnl_3);


	/*** KEYPAD PIT Priority ***/
	// PIT NVIC enable and priority (keypad)
	NVIC_EnableIRQ(PIT2_IRQn);
	NVIC_SetPriority(PIT2_IRQn, 4);

	// Start PIT ch0 (keypad)
	PIT_StartTimer(PIT, kPIT_Chnl_2);
}

void setPeriodPitCh3(uint32_t value){
	PIT_SetTimerPeriod(PIT, kPIT_Chnl_3, USEC_TO_COUNT(value, CLOCK_GetFreq(kCLOCK_BusClk)));
}
