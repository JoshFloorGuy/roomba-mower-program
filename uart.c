#include "uart.h"

void uart_init(void){
    SYSCTL_RCGCUART_R |= 0x02;
    SYSCTL_RCGCGPIO_R |= 0b000010;
    GPIO_PORTB_AFSEL_R |= 0x03;
    GPIO_PORTB_PCTL_R &= 0xFFFFFF00;
    GPIO_PORTB_PCTL_R |= 0x00000011;
    GPIO_PORTB_DEN_R   |= 0x03;
    GPIO_PORTB_DIR_R   |= 0x02;
    GPIO_PORTB_DIR_R   &= ~0x01;


    double fbrd;
    int    ibrd;

    fbrd = (double) (16000000.0 / (16.0*115200.0)); // page 903
    ibrd = (int) fbrd;
    fbrd = (int) ((fbrd-ibrd) * 64 + 0.5);

    UART1_CTL_R &= 0xFFFFFFFE;      // disable UART1 (page 918)
    UART1_IBRD_R = ibrd;        // write integer portion of BRD to IBRD
    UART1_FBRD_R = fbrd;   // write fractional portion of BRD to FBRD
    UART1_LCRH_R = 0x60;        // write serial communication parameters (page 916) * 8bit and no parity
    UART1_CC_R   = 0x0;          // use system clock as clock source (page 939)
    UART1_CTL_R |= 0x1;        // enable UART1
    
    uart_interrupt_init();
}

void uart_sendChar(char data){
    while(UART1_FR_R & 0x20);
    UART1_DR_R = data;
}

int uart_receive(void){
    int ret;

    while(UART1_FR_R & 0x10);
    ret = UART1_DR_R;
    return (ret & 0x000000FF);
}

void uart_sendStr(const char *data){
	while(*data!='\0') {
	    uart_sendChar(*data);
	    data++;
	}
}

void uart_interrupt_init()
{
    // Enable interrupts for receiving bytes through UART1
    UART1_IM_R |= 0x60; //enable interrupt on receive - page 924

    // Find the NVIC enable register and bit responsible for UART1 in table 2-9
    // Note: NVIC register descriptions are found in chapter 3.4
    NVIC_EN0_R |= 0b01000000; //enable uart1 interrupts - page 104

    // Find the vector number of UART1 in table 2-9 ! UART1 is 22 from vector number page 104
    IntRegister(22, &uart_interrupt_handler); //give the microcontroller the address of our interrupt handler - page 104 22 is the vector number

}

void uart_interrupt_handler()
{
// STEP1: Check the Masked Interrup Status
    if(UART1_MIS_R & 0x60) {
        if(UART1_MIS_R & 0x40) {
            //STEP2:  Copy the data
            uart_data = uart_receive();
            flag |= 0x1;
        }
        if(UART1_MIS_R & 0x20) {
            flag |= 0x2; // 2 means transmit is done

        }
    }

    UART1_ICR_R |= 0x30;
//STEP3:  Clear the interrup

}
