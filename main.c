/*
 * @file    main.c
 * @brief   P1 - Square wave generator with DAC & keypad 4x4 on FRDM-K64F
 * @author	Félix Armenta A - PADTS IECA 3
 * @date	June 13th, 2021
 */

/* TODO: insert other include files here. */
#include <stdint.h>
#include <stdbool.h>
#include "MK64F12.h"
#include "fsl_clock.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_common.h"
#include "fsl_pit.h"

#include "keypad.h"
#include "pit.h"
#include "dac.h"

/* TODO: insert other definitions and declarations here. */
#define TEST_PIN 20U
#define START_VAL 90U
#define ASCII 48U

uint8_t main(void) {
	/*** LOCAL STRUCTs CONFIG ***/
	const gpio_pin_config_t led_config = {
			kGPIO_DigitalOutput,
			1,	// Initial output value
	};

	const port_pin_config_t output_config = {
			kPORT_PullDisable,
			kPORT_FastSlewRate,
			kPORT_PassiveFilterDisable,
			kPORT_OpenDrainDisable,
			kPORT_HighDriveStrength,
			kPORT_MuxAsGpio,
			kPORT_UnlockRegister,
	};

	/*** START KEYPAD CONFIG ***/
	initKeypad();

	/*** START SWEEP & DAC FREQUENCY ***/
	initPIT();

	/*** START DAC AMPLITUDE ***/
	initDac();

	/*** TEST LED CONFIG ***/
	// Keypad rows as GPIO
	PORT_SetPinConfig(PORTB, TEST_PIN, &output_config);

	// Keypad rows as outputs and start value
	GPIO_PinInit(GPIOB, TEST_PIN, &led_config);


	/*** INITAL SYSTEM PARAMETERS ***/
	sweep_t currentSweep = row1;
	uint32_t period = 500000U;
	uint8_t keyChar = START_VAL;
	uint8_t unit = START_VAL;
	uint8_t dec = START_VAL;
	float amplitude = 90.0000;
	float reserve = 0.0000;
	bool decimalFlag = false;
	uint32_t dacVal = 4095U;
	uint32_t safeDac = dacVal;
	uint32_t lowDac = 0U;
	bool dacFlag = true;

	while(1){
		/*** SWEEPING KEYPAD ***/
		nextSweep(getPitCh2Flag(), &currentSweep, clearPitCh2Flag);

		/*** DAC SIGNAL ***/
		if (getPitCh3Flag()){
			GPIO_PortToggle(GPIOB, 1u << TEST_PIN);
			if (dacFlag){
				setDacValue(&safeDac);
				dacFlag = false;
			}
			else{
				setDacValue(&lowDac);
				dacFlag = true;
			}
			clearPitCh3Flag();
		}

		/*** FREQ CHANGE ***/
		if (getFreqStep() == fstep2){
			keyChar = START_VAL;
			getPressedKey(&currentSweep, &keyChar);
			keyChar -= ASCII;
			if ((keyChar > 0) && (keyChar < 6)){
				period = (keyChar * 1000000)/2;
				setFreqStep(fstep3);
			}
		}

		else if (getFreqStep() == fstep4){
			setPeriodPitCh3(period);
			setFreqStep(fstep1);
			keyChar = START_VAL;
		}

		/*** AMP CHANGE ***/
		if (getAmpStep() == astep2){
			keyChar = START_VAL;
			getPressedKey(&currentSweep, &keyChar);
			keyChar -= ASCII;
			if ((keyChar > 0) && (keyChar < 4)){
				unit = keyChar;
				setAmpStep(astep3);
			}
		}
		else if (getAmpStep() == astep3){
			getPressedKey(&currentSweep, &keyChar);
			if (keyChar == '.'){
				decimalFlag = true;
				setAmpStep(astep4);
			}
		}
		else if ((getAmpStep() == astep4) && (decimalFlag)){
			keyChar = START_VAL;
			getPressedKey(&currentSweep, &keyChar);
			keyChar -= ASCII;
			if (unit == 3U){
				if ((keyChar > 0) && (keyChar < 4)){
					dec = keyChar;
					setAmpStep(astep5);
				}
			}
			else if ((keyChar >= 0) && (keyChar <= 9)){
				dec = keyChar;
				setAmpStep(astep5);
			}
		}
		else if (getAmpStep() == astep6){
			if (decimalFlag){
				amplitude = (float) unit;
				reserve = (float) dec;
				amplitude += (reserve / 10.0000);
				dacVal = ((amplitude * 4095.0000) / 3.3);
				if (dacVal < 4095){
					safeDac = dacVal;
					setDacValue(&safeDac);
					decimalFlag = false;
					unit = START_VAL;
					dec = START_VAL;
					setAmpStep(astep1);
				}
			}
			else{
				amplitude = (float) ((unit * 4095.0000) / 3.3);
				dacVal = (uint32_t) amplitude;
				if (dacVal < 4095){
					safeDac = dacVal;
					setDacValue(&safeDac);
					decimalFlag = false;
					unit = START_VAL;
					dec = START_VAL;
					setAmpStep(astep1);
				}
			}
		}
	}
	return 0 ;
}
