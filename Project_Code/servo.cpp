#include "mbed.h"
#include "arm_book_lib.h"

#define DUTY_MIN 0.020
#define DUTY_MAX 0.895
#define PERIOD 0.02

PwmOut servo(PF_9);

void servoInit()
{
    servo.period(PERIOD);
    servo.write(DUTY_MIN);
}

void gateOpen()
{
    servo.write(DUTY_MAX/2);
}

void gateClose()
{
    servo.write(DUTY_MIN);
}