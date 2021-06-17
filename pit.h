/*
 * @file    pit.h
 * @brief   Configures PIT_CH2->keypad & PIT_CH3->DAC
 * @author	Félix Armenta A - PADTS IECA 3
 * @date	Jun 13th 2021
 * @vers	v0.1
 */

#ifndef PIT_H_
#define PIT_H_

/*** PIT LIB REQUIRED ***/
#include <stdbool.h>
#include "fsl_pit.h"
#include "fsl_clock.h"
#include "fsl_common.h"
#include "core_cm4.h"
#include "MK64F12.h"

/*** PIT PROTOTYPES ***/
void PIT2_IRQHandler(void);
void PIT3_IRQHandler(void);

/*** API PROTOTYPES ***/
void initPIT(void);

bool getPitCh2Flag(void);
bool getPitCh3Flag(void);

void clearPitCh2Flag(void);
void clearPitCh3Flag(void);
void setPeriodPitCh3(uint32_t value);

#endif /* PIT_H_ */
