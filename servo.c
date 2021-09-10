#include "servo.h"
#include "Timer.h"
#include "lcd.h"
#include "stdbool.h"

int CLOCKWISE = 1;
float DEGREES = 0.0;

void servo_init(void){
    SYSCTL_RCGCGPIO_R |= 0b000010;
    long delay = SYSCTL_RCGCGPIO_R;
    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_PCTL_R &= 0xFF7FFFFF;
    GPIO_PORTB_PCTL_R |= 0x00700000;
    GPIO_PORTB_DEN_R |= 0x20;
    GPIO_PORTB_DIR_R |= 0x20;

    SYSCTL_RCGCTIMER_R |= 0x2;
    long delay2 = SYSCTL_RCGCTIMER_R;
    TIMER1_CTL_R &= 0xEFF;
    TIMER1_CFG_R = 0x00000004;
    TIMER1_TBMR_R = 0xA;
    //3 is 1, 2 is 0, 1 and 0 is 2 to initialize timer for pwm mode, making 0xA
    //TIMER1_TBMR_R &= ~0x10;
    TIMER1_CTL_R &= ~0x4000;
    //bit 14 set to 0 so output unaffected
    // TIMER1_TBMR_R &= 0xFEF;
    TIMER1_TBPR_R = 0x04;
    //set to 4 for prescale register
    TIMER1_TBILR_R = 0xE200;
    //set to E200 for 320000
   // TIMER1_TBMATCHR_R = 296000;
    servo_set(90.0);
    //set to 16000 for match value
    TIMER1_CTL_R |= 0x100;

    // TIMER1_TBR_R
}

int servo_set(float degrees){
    int y = 300500 - (120 * degrees); // <--more semi-right
   // int y = -800/9 * (degrees) + 304000; <--semi-right
    DEGREES = degrees;
    TIMER1_TBMATCHR_R = y;
    return 0;
}

void print_function(void){
    lcd_printf("%s\n%f",(CLOCKWISE ? "Clockwise" : "Counter-Clockwise"),DEGREES);
}

void set_CLOCKWISE(void){
   CLOCKWISE = 1 - CLOCKWISE;
}

int servo_move(float degrees){
    if(CLOCKWISE){
      DEGREES += degrees;
  }else{
      DEGREES -= degrees;
  }
    if(DEGREES > 180){
        DEGREES = 180;
    }
    if(DEGREES < 0){
        DEGREES = 0;
    }
 servo_set(DEGREES);
 return 0;
}
