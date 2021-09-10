#include "open_interface.h"
#include "Timer.h"
#include "ping.h"
#include <inc/tm4c123gh6pm.h>
#include "movement.h"
#include "Lcd.h"
#include "servo.h"
#include "stdio.h"
#include "uart.h"
#include "adc.h"
#include "resetSimulation.h"

int cybot_Scan(oi_t *sensor, int type);

volatile char uart_data;
volatile char flag;

#define SCAN_INTERVAL 1

typedef struct {
    int beginningAngle;
    int endingAngle;
    int beginningDistance;
    int endingDistance;
    int middleAngle;
    int width;
} object;

object* objects[20];

object measureObject();
int sensorStatus(oi_t *sensor);

void main() {
    resetSimulationBoard();
    timer_init();
    ping_init();
    lcd_init();
    servo_init();
    uart_init();
    adc_init();
    oi_t *movement = oi_alloc();
    oi_init(movement);

    int pingCountdown = 0;
    stopMoving(movement);
    startMovingF(movement);
    int found = 0;
    int status;
    while(!found) {

        //oi_update(movement);

        status = sensorStatus(movement);
        if(status & 0x1) {
            move_Backward(movement,10);
            counterClockwise(movement,90);
            startMovingF(movement);
        }
        if(status & 0x2) {
            move_Backward(movement,10);
            Clockwise(movement,90);
            startMovingF(movement);
        }
        if(status & 0xC) {
            switch(status & 0xC) {
                case 0x4:
                    startRotatingCC(movement);
                    while(sensorStatus(movement) & 0xC);
                    break;
                case 0x8:
                    startRotatingC(movement);
                    while(sensorStatus(movement) & 0xC);
                    break;
                case 0xC:
                    move_Backward(movement,10);
                    Clockwise(movement,180);
                    break;
            }
            startMovingF(movement);
        }

        // Cliff and Border detection
        if(movement->cliffFrontLeftSignal==0 || movement->cliffLeftSignal==0) {
            startRotatingCC(movement);
            while(movement->cliffFrontLeftSignal==0 || movement->cliffLeftSignal==0) {oi_update(movement);}
            startMovingF(movement);
        }
        if(movement->cliffFrontRightSignal==0 || movement->cliffRightSignal==0) {
            startRotatingC(movement);
            while(movement->cliffFrontRightSignal==0 || movement->cliffRightSignal==0) {oi_update(movement);}
            startMovingF(movement);
        }
        if(movement->cliffFrontLeftSignal>2000 || movement->cliffLeftSignal>2000) {
            startRotatingCC(movement);
            while(movement->cliffFrontLeftSignal>2000 || movement->cliffLeftSignal>2000) {oi_update(movement);}
            startMovingF(movement);
        }
        if(movement->cliffFrontRightSignal>2000 || movement->cliffRightSignal>2000) {
            startRotatingC(movement);
            while(movement->cliffFrontRightSignal>2000 || movement->cliffRightSignal>2000) {oi_update(movement);}
            startMovingF(movement);
        }

        // TODO: sensor detection for dodging big objects
        // TODO: break loop on small object detection


        // Bump Detection

        if(movement->bumpLeft) {
            move_Backward(movement,10);
            counterClockwise(movement,90);
            startMovingF(movement);
        }
        if(movement->bumpRight) {
            move_Backward(movement,10);
            Clockwise(movement,90);
            startMovingF(movement);
        }



        // Large Object detection


        if(pingCountdown==0) {
            pingCountdown = 3;
            if(ping_getDistance() < 30) {
                stopMoving(movement);
                int smallestIndex = cybot_Scan(movement,0);
                if(smallestIndex > -1) {
                    if(objects[smallestIndex]->middleAngle<=90) {
                        Clockwise(movement,abs(objects[smallestIndex]->middleAngle-90));
                    } else if(objects[smallestIndex]->middleAngle>90 || objects[smallestIndex]->middleAngle <= 270) {
                        counterClockwise(movement,objects[smallestIndex]->middleAngle-90);
                    } else {
                        Clockwise(movement,450-objects[smallestIndex]->middleAngle);
                    }

                    move_forward(movement, objects[smallestIndex]->beginningDistance - 15);

                    smallestIndex = cybot_Scan(movement,1);
                    if(smallestIndex > -1) {
                        object foundObject = *objects[smallestIndex];
                        if(foundObject.width <= 6 && foundObject.width>0) {
                            int i = 0;
                            int otherObject = -1;
                            // Navigate towards the center
                            if(foundObject.middleAngle<=90) {
                                for(i=0;objects[i] != 0; i++) {
                                    if(objects[i]->middleAngle > 270) {
                                        otherObject = i;
                                    }
                                }
                                if(otherObject>-1) {
                                    Clockwise(movement,360 + foundObject.middleAngle-objects[otherObject]->middleAngle);
                                    move_forward(movement,50);
                                } else {
                                    Clockwise(movement,80);
                                    move_forward(movement,15);
                                    counterClockwise(movement,80);
                                    move_forward(movement,50);
                                }
                            } else if(foundObject.middleAngle<=180) {
                                for(i=0;objects[i] != 0; i++) {
                                    if(objects[i]->middleAngle > 180 && foundObject.middleAngle <= 270) {
                                        otherObject = i;
                                    }
                                }
                                if(otherObject>-1) {
                                    counterClockwise(movement,objects[otherObject]->middleAngle-foundObject.middleAngle);
                                    move_forward(movement,50);
                                } else {
                                    counterClockwise(movement,80);
                                    move_forward(movement,15);
                                    Clockwise(movement,80);
                                    move_forward(movement,50);
                                }
                            } else if(foundObject.middleAngle<=270) {
                                for(i=0;objects[i] != 0; i++) {
                                    if(objects[i]->middleAngle > 90 && foundObject.middleAngle <= 180) {
                                        otherObject = i;
                                    }
                                }
                                if(otherObject>-1) {
                                    counterClockwise(movement,foundObject.middleAngle-objects[otherObject]->middleAngle);
                                    move_forward(movement,50);
                                } else {
                                    Clockwise(movement,270);
                                    move_forward(movement,15);
                                    counterClockwise(movement,80);
                                    move_forward(movement,50);
                                }
                            } else {
                                for(i=0;objects[i] != 0; i++) {
                                    if(objects[i]->middleAngle <= 90) {
                                        otherObject = i;
                                    }
                                }
                                if(otherObject>-1) {
                                    Clockwise(movement,360 + objects[otherObject]->middleAngle - foundObject.middleAngle);
                                    move_forward(movement,50);
                                } else {
                                    counterClockwise(movement,270);
                                    move_forward(movement,15);
                                    Clockwise(movement,80);
                                    move_forward(movement,50);
                                }
                            }
                            // Scan for surrounding objects
                            cybot_Scan(movement,0);
                            int total;
                            for(total=0; objects[total]!=0; total++) {
                                if(objects[total]->width==0) total--;
                            }
                            if(total>=3) {
                                found = 1;
                            } else {
                                Clockwise(movement,180);
                                startMovingF(movement);
                                pingCountdown = 10;
                            }
                        } else if(foundObject.width==0) {
                            startMovingF(movement);
                        } else {
                            pingCountdown = 10;
                            if(foundObject.middleAngle<=90) {
                                // Turn Right
                                counterClockwise(movement,90);
                                move_forward(movement,25);
                                Clockwise(movement,90);
                                startMovingF(movement);
                            } else {
                                // Turn Left
                                Clockwise(movement,90);
                                move_forward(movement,25);
                                counterClockwise(movement,90);
                                startMovingF(movement);
                            }
                        }
                    } else {
                        startMovingF(movement);
                    }

                } else {
                    startMovingF(movement);
                }
            }

        }
        pingCountdown--;

    }

    oi_free(movement);

}

//TODO: use ping sensor to find objects

// scan with Ping sensor


// 360 scan with servo and adc
int cybot_Scan(oi_t *sensor, int type) {
    float i = 0.0;
    int objectIndex = 0;
    char sendStr[100];
    int prevDistance = 0;

    memset(objects, 0, sizeof(objects));

    int k;
    //uart_sendStr("Angle\tDistance (cm)\n");
    for(k = 0; k<2; k++) {
        for(i = 0.0; i<=180 && objectIndex < 20; i+=SCAN_INTERVAL) {
            servo_set(i);
            //float d = ping_getDistance();
            int adc = adc_convert(adc_read());
            if(adc <= 45 && objects[objectIndex] == 0) {
                objects[objectIndex] = (object *) calloc(1,sizeof(object));
                objects[objectIndex]->beginningAngle = (int) i + 180*k;
                objects[objectIndex]->beginningDistance = adc;
            }
            if(adc > 45 && objects[objectIndex] != 0) {
                objects[objectIndex]->endingAngle = (int) i-SCAN_INTERVAL + (180*k);
                objects[objectIndex]->endingDistance = prevDistance;
                if(objects[objectIndex]->endingAngle == objects[objectIndex]->beginningAngle) {
                    objects[objectIndex] = 0;
                } else {
                    objects[objectIndex]->middleAngle = objects[objectIndex]->beginningAngle + ((int) i-SCAN_INTERVAL + (180*k) - objects[objectIndex]->beginningAngle)/2;
                    objectIndex++;
                }
            }
            prevDistance = adc;
            //sprintf(sendStr,"%f\t%d\n",i+(180*k),adc);
            //uart_sendStr(sendStr);
        }
        if(objects[objectIndex]->endingAngle == 0) {
            objects[objectIndex]->endingAngle = (int) 180-SCAN_INTERVAL + (180*k);
            objects[objectIndex]->endingDistance = prevDistance;
            objects[objectIndex]->middleAngle = objects[objectIndex]->beginningAngle + ((int) 180-SCAN_INTERVAL + (180*k) - objects[objectIndex]->beginningAngle)/2;
        }
        if(k) {
            Clockwise(sensor, 180);
        } else {
            counterClockwise(sensor,180);
        }
    }

    int j = 0;
    float finalAngle = 180;
    int smallest = 181;
    if(type) smallest = 0;
    int smallestIndex;
    while(objects[j] != 0) {
        if(j==0) {
            uart_sendStr("\nObject #\tWidth (rad)\tWidth (cm)\n");
        }
        if(objects[j]->endingAngle != 0) {
            int angle = (objects[j]->endingAngle-objects[j]->beginningAngle);
            double angleSin = sin(angle*M_PI/180);
            double angleCos = cos(angle*M_PI/180);
            int L = abs((angleCos*objects[j]->endingDistance)-objects[j]->beginningDistance);
            int H = abs(angleSin*objects[j]->endingDistance);
            int width = sqrt((L*L+H*H));
            objects[j]->width = width;
            if(type == 1) {
                if(angle > smallest) {
                    smallest = angle;
                    smallestIndex = j;
                    finalAngle = objects[j]->beginningAngle + angle/2;
                }
            } else {
                if(width < smallest && width > 0) {
                    smallest = width;
                    smallestIndex = j;
                    finalAngle = objects[j]->beginningAngle + angle/2;
                }
            }
            sprintf(sendStr,"%d\t\t%d\t\t%d\n",j+1,angle,width);
            uart_sendStr(sendStr);
        }
        j++;
    }
    servo_set(90);
    if(finalAngle == 180) return -1;
    return smallestIndex;
}

int sensorStatus(oi_t *sensor) {
    int ret = 0;
    oi_update(sensor);
    // Check Bump Sensors
    //ret = sensor->bumpLeft + 0b10*sensor->bumpRight;

    if(sensor->bumpLeft) {
        ret |= 0x1;
    }
    if(sensor->bumpRight) {
        ret |= 0x2;
    }
    // Check Cliff and Drop sensors
    if(sensor->cliffFrontLeftSignal>2000 || sensor->cliffLeftSignal>2000 || sensor->cliffFrontLeftSignal<999 || sensor->cliffLeftSignal<999 ) {
        ret |= 0x4;
    }
    if(sensor->cliffFrontRightSignal>2000 || sensor->cliffRightSignal>2000 || sensor->cliffFrontRightSignal<999 || sensor->cliffRightSignal<999 ) {
        ret |= 0x8;
    }

    // Check for tall objects
    /*if(ping_getDistance() > 30) {
        ret |= 0x10;
    } */

    return ret;
}

