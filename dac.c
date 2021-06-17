/*
 * @file    dac.c
 * @brief   Initilizes DAC and let adjust its value
 * @author	Félix Armenta A - PADTS IECA 3
 * @date	Jun 17th 2021
 * @vers	v0.1
 */
#include "dac.h"

void initDac(void){
	dac_config_t dacConfigStruct;
	uint32_t dacValue = 4095;

	DAC_GetDefaultConfig(&dacConfigStruct);
	DAC_Init(DAC0, &dacConfigStruct);
	DAC_Enable(DAC0, true);             /* Enable output. */
	DAC_SetBufferReadPointer(DAC0, 0U); /* Make sure the read pointer to the start. */
	                                    /*
	                                     * The buffer is not enabled, so the read pointer can not move automatically. However, the buffer's read pointer
	                                     * and items can be written manually by user.
	                                     */
	DAC_SetBufferValue(DAC0, 0U, dacValue);
}

void setDacValue(uint32_t *dacValue){
	if ((*dacValue >= 0) && (*dacValue < 4096)){
		DAC_SetBufferValue(DAC0,0U, *dacValue);
	}
}



