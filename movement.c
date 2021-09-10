#include "lcd.h"
#include "open_interface.h"
#include "timer.h"
#include "movement.h"

int move_forward(oi_t *self, int centimeters)
    {

    int distancemm = centimeters*10;
    double sum = 0;

    oi_setWheels(250,250);


    while(sum < distancemm)
    {
        oi_update(self);
        sum +=self->distance;
    }
    oi_setWheels(0,0);

    return sum;
}

void move_Backward(oi_t *self, int centimeters)
    {

    int distancemm = centimeters*10;
    double sum = distancemm;

    oi_setWheels(-250, -250);

    while(sum > 0)
    {
        oi_update(self);
        sum += self->distance;
    }

    oi_setWheels(0,0);

    return;
}

void counterClockwise(oi_t *sensor, int degrees)
{
    oi_setWheels(250,-250);
    double sum = 0;
    while(sum < degrees)
    {
             oi_update(sensor);
             sum += sensor -> angle;
    }
    oi_setWheels(0,0);

    return;
}

void Clockwise(oi_t *sensor, int degrees)
    {

    degrees = degrees * -1;

    oi_setWheels(-250,250);

    double sum = 0;
    while(sum > degrees)
    {
        oi_update(sensor);
        sum += sensor -> angle;
    }
    oi_setWheels(0,0);

    return;
}

void startMovingF(oi_t *sensor) {
    oi_setWheels(250,250);
}

void startMovingB(oi_t *sensor) {
    oi_setWheels(-250,-250);
}

void startRotatingC(oi_t *sensor) {
    oi_setWheels(-250,250);
}

void startRotatingCC(oi_t *sensor) {
    oi_setWheels(250,-250);
}

void stopMoving(oi_t *sensor) {
    oi_setWheels(0,0);
}
