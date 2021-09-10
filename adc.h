/*
 * ADC.h
 *
 *  Created on: Oct 15, 2020
 *      Author: mattg1
 */

#include <stdint.h>
#include<stdbool.h>
#include "timer.h"
#include "lcd.h"
#include <inc/tm4c123gh6pm.h>
#ifndef ADC_H_
#define ADC_H_

void adc_init(void);
int adc_read(void); //reading distance
int adc_convert(int adcRaw);

#endif
