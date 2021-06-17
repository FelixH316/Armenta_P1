/*
 * @file    keypad.h
 * @brief   For keypads 4x4 and handles them through ISR & PIT
 * @author	Félix Armenta A - PADTS IECA 3
 * @date	Jun 13th 2021
 * @vers	v0.1
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

/*** GPIO LIB REQUIRED ***/
#include <stdint.h>
#include <stdbool.h>
//#include <stdio.h>
#include "fsl_clock.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "MK64F12.h"

/*** KEYBOARD ROW PINS ***/
#define ROW1_PIN 2u
#define ROW2_PIN 3u
#define ROW3_PIN 10u
#define ROW4_PIN 11u

/*** KEYBOARD ROW PINS ***/
#define COL1_PIN 0u
#define COL2_PIN 2u
#define COL3_PIN 3u
#define COL4_PIN 1u

/*** SELECT PINS ***/
#define SW2 6u
#define SW3 4u

// Keypad sweep sequence
typedef enum{row1, row2, row3, row4} sweep_t;
typedef enum{col1, col2, col3, col4} column_t;
typedef enum{fstep1, fstep2, fstep3, fstep4} freqSequence_t;
typedef enum{astep1, astep2, astep3, astep4, astep5, astep6} ampSequence_t;

/*** ISR PROTOTYPES ***/
void PORTD_IRQHandler(void);
void PORTC_IRQHandler(void);
void PORTA_IRQHandler(void);

/*** API PROTOTYPES ***/
void initKeypad(void);
void sweepSelect(sweep_t row);
void nextSweep(bool pitFlagStatus, sweep_t *rowVal, void (*fptrClearPitFlag)(void));

bool getCol1Flag(void);
bool getCol2Flag(void);
bool getCol3Flag(void);
bool getCol4Flag(void);
bool getSw2Flag(void);
bool getSw3Flag(void);

void clearCol1Flag(void);
void clearCol2Flag(void);
void clearCol3Flag(void);
void clearCol4Flag(void);
void clearSw2Flag(void);
void clearSw3Flag(void);

uint8_t getColChar (const sweep_t *rowVal, column_t column);

freqSequence_t getFreqStep (void);
void setFreqStep (freqSequence_t step);

ampSequence_t getAmpStep (void);
void setAmpStep (ampSequence_t step);

void getPressedKey(const sweep_t *ptrRow, uint8_t *ptrVal);

#endif /* KEYPAD_H_ */
