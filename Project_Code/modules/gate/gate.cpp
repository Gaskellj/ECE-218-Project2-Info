#include "mbed.h"
#include "arm_book_lib.h"

#include "servo.h"
#include "gate.h"
#include "matrix_keypad.h"

void gateInit()
{

}

void gateCycle(bool code)
{
    code = isCorrectCode();
    if(code)
    {
        gateRaise();
        delay(10000);
        gateLower();
        delay(5000);
    }
}