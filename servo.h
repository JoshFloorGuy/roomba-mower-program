
#include <inc/tm4c123gh6pm.h>
#ifndef SERVO_H_
#define SERVO_H_

void servo_init(void);
int servo_set(float degrees);
int servo_move(float degrees);



#endif /* SERVO_H_ */
