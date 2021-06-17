/*
 * @file    dac.h
 * @brief   Initilizes DAC and let adjust its value
 * @author	Félix Armenta A - PADTS IECA 3
 * @date	Jun 17th 2021
 * @vers	v0.1
 */

#ifndef DAC_H_
#define DAC_H_

#include <stdint.h>
#include "fsl_dac.h"

void initDac(void);
void setDacValue(uint32_t *dacValue);


#endif /* DAC_H_ */
