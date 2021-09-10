/*
 * ADC.c
 *
 *  Created on: Oct 14, 2020
 *      Author: mattg1
 */
#include <stdint.h>
#include<stdbool.h>
#include "timer.h"
#include "lcd.h"
#include <math.h>
#include <inc/tm4c123gh6pm.h>
#include "ADC.h"

volatile int distance = 0;

void adc_init(void){

    SYSCTL_RCGCGPIO_R |= 0b00000010;

    while ((SYSCTL_PRGPIO_R & 0b00000010) == 0) {};

    GPIO_PORTB_DIR_R &= ~0x10;
    GPIO_PORTB_AFSEL_R |= 0x10;
    GPIO_PORTB_DEN_R &= ~0x10;
    GPIO_PORTB_AMSEL_R |= 0x10;

    SYSCTL_RCGCADC_R |= 0x01;

    while ((SYSCTL_PRADC_R & 0b00000001) == 0) {};

    ADC0_SAC_R &= 0x4; //sets hardware oversampling
    ADC0_ACTSS_R &= ~0x0008;
    ADC0_EMUX_R &= ~0xF000;  //configure trigger
    ADC0_SSMUX3_R &= ~0xF000;
    ADC0_SSMUX3_R += 10;
    ADC0_SSCTL3_R = 0x0006;
    ADC0_ACTSS_R |= 0x0008;
}

int adc_read(void){

    int result;

    ADC0_PSSI_R = 0x0008; //trigger

    while((ADC0_RIS_R & 0x08) == 0) {} //Interrupt

    result = ADC0_SSFIFO3_R & 0xFFFF; //
    ADC0_ISC_R = 0x0008; //Clears the flags
    return result;

}


int adc_convert(int adcRaw) {
    if(distance < 13){
        distance = 68.8855164 * pow(0.9993975959, adcRaw);
    }
    else if (distance >= 13 && distance <= 16){
        distance = 68.8855164 * pow(0.9993975959, adcRaw) - 2;
    }
    else if (distance > 16 && distance <= 30){
        distance = 68.8855164 * pow(0.9993975959, adcRaw) - 3;
    }
    else if (distance > 30 && distance <= 32){
        distance = 68.8855164 * pow(0.9993975959, adcRaw) - 2;
    }
    else if (distance > 32 && distance <= 35){
        distance = 68.8855164 * pow(0.9993975959, adcRaw) - 1;
    }
    else if (distance == 36) {
        distance = 68.8855164 * pow(0.9993975959, adcRaw);
    }
    else if (distance > 36 && distance <= 39){
        distance = 68.8855164 * pow(0.9993975959, adcRaw) + 1;               //yo TA im sorry :(
    }
    else if (distance > 39 && distance <= 41){
        distance = 68.8855164 * pow(0.9993975959, adcRaw) + 2;
    }
    else if (distance > 41 && distance <= 43){
        distance = 68.8855164 * pow(0.9993975959, adcRaw) + 3;
    }
    else if (distance > 43 && distance <= 45){
        distance = 68.8855164 * pow(0.9993975959, adcRaw) + 4;
    }
    else if (distance > 45 && distance <= 46){
        distance = 68.8855164 * pow(0.9993975959, adcRaw) + 5;
    }
    else if (distance > 46 && distance <= 48){
        distance = 68.8855164 * pow(0.9993975959, adcRaw) + 6;
    }
    else if (distance > 48 && distance <= 49){
        distance = 68.8855164 * pow(0.9993975959, adcRaw) + 7;
    }
    else if (distance == 50){
        distance = 68.8855164 * pow(0.9993975959, adcRaw) + 8;
    }
    else if (distance > 50){
        distance = 68.8855164 * pow(0.9993975959, adcRaw);
    }
    return distance;
}
