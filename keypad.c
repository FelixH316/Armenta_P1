/*
 * @file    keypad.c
 * @brief   For keypads 4x4 and handles them through ISR & PIT
 * @author	Félix Armenta A - PADTS IECA 3
 * @date	Jun 13th 2021
 * @vers	v0.1
 */

#include "keypad.h"

volatile bool col1Flag = false;
volatile bool col2Flag = false;
volatile bool col3Flag = false;
volatile bool col4Flag = false;

volatile bool sw2Flag = false;
volatile bool sw3Flag = false;

volatile freqSequence_t currentFreqSeq = fstep1;
volatile ampSequence_t currentAmpSeq = astep1;

void PORTD_IRQHandler(void)
{
	uint32_t dFlags = 0u;
	dFlags = GPIO_PortGetInterruptFlags(GPIOD);

	if (dFlags & (1u << COL1_PIN))
	{
		GPIO_PortClearInterruptFlags(GPIOD, 1u << COL1_PIN);
		col1Flag = true;
	}
	else if (dFlags & (1u << COL2_PIN))
	{
		GPIO_PortClearInterruptFlags(GPIOD, 1u << COL2_PIN);
		col2Flag = true;
	}
	else if (dFlags & (1u << COL3_PIN))
	{
		GPIO_PortClearInterruptFlags(GPIOD, 1u << COL3_PIN);
		col3Flag = true;
	}
	else if (dFlags & (1u << COL4_PIN))
	{
		GPIO_PortClearInterruptFlags(GPIOD, 1u << COL4_PIN);
		col4Flag = true;
	}
	SDK_ISR_EXIT_BARRIER;
}

void PORTC_IRQHandler(void){
	GPIO_PortClearInterruptFlags(GPIOC, 1U << SW2);
	sw2Flag = true;
	if((currentFreqSeq == fstep1) && (currentAmpSeq == astep1)){
		col1Flag = false;
		col2Flag = false;
		col3Flag = false;
		col4Flag = false;
		currentFreqSeq = fstep2;
	}
	else if (currentFreqSeq == fstep3){
		currentFreqSeq = fstep4;
	}
	else if((currentAmpSeq == astep5) || (currentAmpSeq == astep3)){
		col1Flag = false;
		col2Flag = false;
		col3Flag = false;
		col4Flag = false;
		currentAmpSeq = astep6;
	}
	SDK_ISR_EXIT_BARRIER;
}

void PORTA_IRQHandler(void){
	GPIO_PortClearInterruptFlags(GPIOA, 1U << SW3);
	sw3Flag = true;
	if((currentFreqSeq == fstep1) && (currentAmpSeq == astep1)){
		col1Flag = false;
		col2Flag = false;
		col3Flag = false;
		col4Flag = false;
		currentAmpSeq = astep2;
	}
	SDK_ISR_EXIT_BARRIER;
}

void initKeypad(void)
{
	/*** OUTPUT STRUCTs ***/
	const gpio_pin_config_t row_init = {
			kGPIO_DigitalOutput,
			1,	// Initial output value
	};

	const port_pin_config_t row_config = {
			kPORT_PullDisable,
			kPORT_FastSlewRate,
			kPORT_PassiveFilterDisable,
			kPORT_OpenDrainDisable,
			kPORT_HighDriveStrength,
			kPORT_MuxAsGpio,
			kPORT_UnlockRegister,
	};


	/*** INPUT STRUCTs ***/
	const gpio_pin_config_t column_init = {
			kGPIO_DigitalInput,
			0,
	};

	const port_pin_config_t column_config = {
			kPORT_PullUp,
			kPORT_FastSlewRate,
			kPORT_PassiveFilterEnable,
			kPORT_OpenDrainDisable,
			kPORT_HighDriveStrength,
			kPORT_MuxAsGpio,
			kPORT_UnlockRegister,
	};


	/*** KEYPAD ROWS CONFIG ***/
	// Keypad rows Clock Gating
	CLOCK_EnableClock(kCLOCK_PortB);

	// Keypad rows as GPIO
	PORT_SetPinConfig(PORTB, ROW1_PIN, &row_config);
	PORT_SetPinConfig(PORTB, ROW2_PIN, &row_config);
	PORT_SetPinConfig(PORTB, ROW3_PIN, &row_config);
	PORT_SetPinConfig(PORTB, ROW4_PIN, &row_config);

	// Keypad rows as outputs and start value
	GPIO_PinInit(GPIOB, ROW1_PIN, &row_init);
	GPIO_PinInit(GPIOB, ROW2_PIN, &row_init);
	GPIO_PinInit(GPIOB, ROW3_PIN, &row_init);
	GPIO_PinInit(GPIOB, ROW4_PIN, &row_init);


	/*** KEYPAD COLUMN CONFIG ***/
	// Keypad column Clock Gating
	CLOCK_EnableClock(kCLOCK_PortD);

	// Keypad columns as GPIO
	PORT_SetPinConfig(PORTD, COL1_PIN, &column_config);
	PORT_SetPinConfig(PORTD, COL2_PIN, &column_config);
	PORT_SetPinConfig(PORTD, COL3_PIN, &column_config);
	PORT_SetPinConfig(PORTD, COL4_PIN, &column_config);

	// Keypad columns as inputs
	GPIO_PinInit(GPIOD, COL1_PIN, &column_init);
	GPIO_PinInit(GPIOD, COL2_PIN, &column_init);
	GPIO_PinInit(GPIOD, COL3_PIN, &column_init);
	GPIO_PinInit(GPIOD, COL4_PIN, &column_init);

	// Keypad columns as extern ISR
	PORT_SetPinInterruptConfig(PORTD, COL1_PIN, kPORT_InterruptFallingEdge);
	PORT_SetPinInterruptConfig(PORTD, COL2_PIN, kPORT_InterruptFallingEdge);
	PORT_SetPinInterruptConfig(PORTD, COL3_PIN, kPORT_InterruptFallingEdge);
	PORT_SetPinInterruptConfig(PORTD, COL4_PIN, kPORT_InterruptFallingEdge);

	// SWs ISR enable and priorities
	NVIC_SetPriority(PORTD_IRQn, 3);
	NVIC_EnableIRQ(PORTD_IRQn);


	/*** SW2 AND SW3 CONFIG ***/
	// SW2 and SW3 Clock Gating
	CLOCK_EnableClock(kCLOCK_PortC);
	CLOCK_EnableClock(kCLOCK_PortA);

	// SWs config
	PORT_SetPinConfig(PORTC, SW2, &column_config);
	PORT_SetPinConfig(PORTA, SW3, &column_config);

	// SWs as inputs
	GPIO_PinInit(GPIOC, SW2, &column_init);
	GPIO_PinInit(GPIOA, SW3, &column_init);

	// SWs as extern ISR
	PORT_SetPinInterruptConfig(PORTC, SW2, kPORT_InterruptRisingEdge);
	PORT_SetPinInterruptConfig(PORTA, SW3, kPORT_InterruptRisingEdge);

	// SWs ISR enable and priorities
	NVIC_SetPriority(PORTC_IRQn, 3);
	NVIC_EnableIRQ(PORTC_IRQn);

	NVIC_SetPriority(PORTA_IRQn, 3);
	NVIC_EnableIRQ(PORTA_IRQn);
}

void sweepSelect(sweep_t row){
	switch(row)
	{
	case row1:
		GPIO_PortClear(GPIOB, 1u << ROW1_PIN);
		GPIO_PortSet(GPIOB, 1u << ROW2_PIN);
		GPIO_PortSet(GPIOB, 1u << ROW3_PIN);
		GPIO_PortSet(GPIOB, 1u << ROW4_PIN);
		break;
	case row2:
		GPIO_PortSet(GPIOB, 1u << ROW1_PIN);
		GPIO_PortClear(GPIOB, 1u << ROW2_PIN);
		GPIO_PortSet(GPIOB, 1u << ROW3_PIN);
		GPIO_PortSet(GPIOB, 1u << ROW4_PIN);
		break;
	case row3:
		GPIO_PortSet(GPIOB, 1u << ROW1_PIN);
		GPIO_PortSet(GPIOB, 1u << ROW2_PIN);
		GPIO_PortClear(GPIOB, 1u << ROW3_PIN);
		GPIO_PortSet(GPIOB, 1u << ROW4_PIN);
		break;
	case row4:
		GPIO_PortSet(GPIOB, 1u << ROW1_PIN);
		GPIO_PortSet(GPIOB, 1u << ROW2_PIN);
		GPIO_PortSet(GPIOB, 1u << ROW3_PIN);
		GPIO_PortClear(GPIOB, 1u << ROW4_PIN);
		break;
	default:
		GPIO_PortSet(GPIOB, 1u << ROW1_PIN);
		GPIO_PortSet(GPIOB, 1u << ROW2_PIN);
		GPIO_PortSet(GPIOB, 1u << ROW3_PIN);
		GPIO_PortSet(GPIOB, 1u << ROW4_PIN);
	}
}

void nextSweep(bool pitFlagStatus, sweep_t *rowVal, void (*fptrClearPitFlag)(void)){
	if (pitFlagStatus){
		sweepSelect(*rowVal);
		(*fptrClearPitFlag)();
		(*rowVal == row4) ? *rowVal = row1 : (*rowVal = *rowVal + 1);
	}
}

uint8_t getColChar (const sweep_t *rowVal, column_t column){
	uint8_t key = 0;
	if (column == col1){
		switch (*rowVal)
		{
		case row1:
			key = '.';
			break;
		case row2:
			key = '1';
			break;
		case row3:
			key = '4';
			break;
		case row4:
			key = '7';
			break;
		default:
			key = '1';
		}
	}
	else if (column == col2){
		switch (*rowVal)
		{
		case row1:
			key = '0';
			break;
		case row2:
			key = '2';
			break;
		case row3:
			key = '5';
			break;
		case row4:
			key = '8';
			break;
		default:
			key = '0';
		}
	}
	else if (column == col3){
		switch (*rowVal)
		{
		case row1:
			key = '#';
			break;
		case row2:
			key = '3';
			break;
		case row3:
			key = '6';
			break;
		case row4:
			key = '9';
			break;
		default:
			key = '3';
		}
	}
	else if (column == col4){
		switch (*rowVal)
		{
		case row1:
			key = 'D';
			break;
		case row2:
			key = 'A';
			break;
		case row3:
			key = 'B';
			break;
		case row4:
			key = 'C';
			break;
		default:
			key = 'A';
		}
	}
	return key;
}

void getPressedKey(const sweep_t *ptrRow, uint8_t *ptrVal){
	if(col1Flag){
		*ptrVal = getColChar (ptrRow, col1);
		//printf("\r%c\n", *ptrVal);
		col1Flag = false;
	}
	else if(col2Flag){
		*ptrVal = getColChar (ptrRow, col2);
		//printf("\r%c\n", *ptrVal);
		col2Flag = false;
	}
	else if(col3Flag){
		*ptrVal = getColChar (ptrRow, col3);
		//printf("\r%c\n", *ptrVal);
		col3Flag = false;
	}
	else if(col4Flag){
		*ptrVal = getColChar (ptrRow, col4);
		//printf("\r%c\n", *ptrVal);
		col4Flag = false;
	}
}

bool getCol1Flag(void){
	return col1Flag;
}

bool getCol2Flag(void){
	return col2Flag;
}

bool getCol3Flag(void){
	return col3Flag;
}

bool getCol4Flag(void){
	return col4Flag;
}

bool getSw2Flag(void){
	return sw2Flag;
}

bool getSw3Flag(void){
	return sw3Flag;
}

void clearCol1Flag(void){
	col1Flag = false;
}

void clearCol2Flag(void){
	col2Flag = false;
}

void clearCol3Flag(void){
	col3Flag = false;
}

void clearCol4Flag(void){
	col4Flag = false;
}

void clearSw2Flag(void){
	sw2Flag = false;
}

void clearSw3Flag(void){
	sw3Flag = false;
}

freqSequence_t getFreqStep(void){
	return currentFreqSeq;
}

void setFreqStep (freqSequence_t step){
	currentFreqSeq = step;
}

ampSequence_t getAmpStep (void){
	return currentAmpSeq;
}

void setAmpStep (ampSequence_t step){
	currentAmpSeq = step;
}
