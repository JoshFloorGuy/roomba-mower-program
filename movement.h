#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "Timer.h"
#include <inc/tm4c123gh6pm.h>
#include "lcd.h"



 void move_Backward(oi_t *self, int centimeters);
 int move_forward(oi_t *self, int centimeters);
 void counterClockwise(oi_t *sensor, int degrees);
 void Clockwise(oi_t *sensor, int degrees);
 void startMovingF(oi_t *sensor);
void startMovingB(oi_t *sensor);
void stopMoving(oi_t *sensor);
void startRotatingC(oi_t *sensor);
void startRotatingCC(oi_t *sensor);
#endif
