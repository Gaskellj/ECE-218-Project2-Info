//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "matrix_keypad.h"

#include "date_and_time.h"
#include "display.h"

//=====[Declaration of private defines]========================================

#define MATRIX_KEYPAD_NUMBER_OF_ROWS    4
#define MATRIX_KEYPAD_NUMBER_OF_COLS    4
#define DEBOUNCE_KEY_TIME_MS        40

//=====[Declaration of private data types]=====================================

typedef enum {
    MATRIX_KEYPAD_SCANNING,
    MATRIX_KEYPAD_DEBOUNCE,
    MATRIX_KEYPAD_KEY_HOLD_PRESSED
} matrixKeypadState_t;

//=====[Declaration and initialization of public global objects]===============

DigitalOut keypadRowPins[MATRIX_KEYPAD_NUMBER_OF_ROWS] = {PB_3, PB_5, PC_7, PA_15};
DigitalIn keypadColPins[MATRIX_KEYPAD_NUMBER_OF_COLS]  = {PB_12, PB_13, PB_15, PC_6};

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

bool correctCode = false;// this should be public since we want this to be accessed by all the modules that will utilize the correct code
static matrixKeypadState_t matrixKeypadState;
static int timeIncrement_ms = 0;
static int xStartDisp = 5;
static char enteredCode[50];
char codeSequence[4]   = { '1', '1', '0', '0' }; // correct code
static int countCodeKeys = 0;


//=====[Declarations (prototypes) of private functions]========================

static char matrixKeypadScan();
static void matrixKeypadReset();
static void writeOnLCDCharToString(char keypadKey);////
static void emptyList();
static void messageDisplay();


//=====[Implementations of public functions]===================================

void matrixKeypadInit( int updateTime_ms )
{
    timeIncrement_ms = updateTime_ms;
    matrixKeypadState = MATRIX_KEYPAD_SCANNING;
    int pinIndex = 0;
    for( pinIndex=0; pinIndex<MATRIX_KEYPAD_NUMBER_OF_COLS; pinIndex++ ) {
        (keypadColPins[pinIndex]).mode(PullUp);
    }
}

char matrixKeypadUpdate()
{
    static int accumulatedDebounceMatrixKeypadTime = 0;
    static char matrixKeypadLastKeyPressed = '\0';

    char keyDetected = '\0';
    char keyReleased = '\0';

    switch( matrixKeypadState ) {

    case MATRIX_KEYPAD_SCANNING:
        keyDetected = matrixKeypadScan();
        if( keyDetected != '\0' ) {
            writeOnLCDCharToString(keyDetected);//////////////////////////
            delay(500);
            matrixKeypadLastKeyPressed = keyDetected;
            accumulatedDebounceMatrixKeypadTime = 0;
            matrixKeypadState = MATRIX_KEYPAD_DEBOUNCE;
        }
        break;

    case MATRIX_KEYPAD_DEBOUNCE:
        if( accumulatedDebounceMatrixKeypadTime >=
            DEBOUNCE_KEY_TIME_MS ) {
            keyDetected = matrixKeypadScan();
            if( keyDetected == matrixKeypadLastKeyPressed ) {
                matrixKeypadState = MATRIX_KEYPAD_KEY_HOLD_PRESSED;
            } else {
                matrixKeypadState = MATRIX_KEYPAD_SCANNING;
                // writeOnLCDCharToString(keyDetected); ///////////////
            }
        }
        accumulatedDebounceMatrixKeypadTime =
            accumulatedDebounceMatrixKeypadTime + timeIncrement_ms;
        break;

    case MATRIX_KEYPAD_KEY_HOLD_PRESSED:
        keyDetected = matrixKeypadScan();
        if( keyDetected != matrixKeypadLastKeyPressed ) {
            if( keyDetected == '\0' ) {
                keyReleased = matrixKeypadLastKeyPressed;
            }
            matrixKeypadState = MATRIX_KEYPAD_SCANNING;
        }
        break;

    default:
        matrixKeypadReset();
        break;
    }
    return keyReleased;
}

//=====[Implementations of private functions]==================================

static char matrixKeypadScan()
{
    int row = 0;
    int col = 0;
    int i = 0; 

    char matrixKeypadIndexToCharArray[] = {
        '1', '2', '3', 'A',
        '4', '5', '6', 'B',
        '7', '8', '9', 'C',
        '*', '0', '#', 'D',
    };
    
    for( row=0; row<MATRIX_KEYPAD_NUMBER_OF_ROWS; row++ ) {

        for( i=0; i<MATRIX_KEYPAD_NUMBER_OF_ROWS; i++ ) {
            keypadRowPins[i] = ON;
        }

        keypadRowPins[row] = OFF;

        for( col=0; col<MATRIX_KEYPAD_NUMBER_OF_COLS; col++ ) {
            if( keypadColPins[col] == OFF ) {
                // displayCharPositionWrite ( 0,0 );
                // displayStringWrite("                ");
                // displayCharPositionWrite ( 0,1 );
                // displayStringWrite("                ");
                // displayCharPositionWrite ( 0,0 );
                // writeOnLCD(int (row*MATRIX_KEYPAD_NUMBER_OF_ROWS + col));
                // char keypadKey = matrixKeypadIndexToCharArray[row*MATRIX_KEYPAD_NUMBER_OF_ROWS + col];
                // writeOnLCDCharToString(keypadKey);
                
                // sprintf(codeString, "%c", matrixKeypadIndexToCharArray[row*MATRIX_KEYPAD_NUMBER_OF_ROWS + col] );
                
                // displayCharPositionWrite ( 8,0 );
                // displayStringWrite(codeString);

                return matrixKeypadIndexToCharArray[
                    row*MATRIX_KEYPAD_NUMBER_OF_ROWS + col];
            }
        }
    }
    return '\0';
}

static void writeOnLCDCharToString(char keypadKey){
    char codeString[10];
     
    // static int countCodeKeys = 0;
    
    // static char enteredCode[4];

    if ( countCodeKeys < 4 ){
    enteredCode[countCodeKeys] = keypadKey;
    displayCharPositionWrite ( 0,0 );
    displayStringWrite("Code:           ");
    // char codeString[10];

    for (int i = 0; i < 4; i++){
    char keyPadKey = enteredCode[i];
    sprintf(codeString, "%c", keyPadKey);
    displayCharPositionWrite ( i+xStartDisp,0);
    displayStringWrite(codeString);
    }
    // sprintf(codeString, "%c", keypadKey);
    // displayCharPositionWrite ( xStartDisp,0 );
    // displayStringWrite(codeString);
    // displayKeys

    // for (char c: enteredCode){
    // sprintf(codeString, "%c", c);
    // displayCharPositionWrite ( xStartDisp,0 );
    // displayStringWrite(codeString);
    
    countCodeKeys++;
    if(countCodeKeys == 4){
    
    for (int i = 0; i < 4; i++){

        if (enteredCode[i] == codeSequence[i] ){
            if(i == 3){
                correctCode = true;
            }
        }
    }

    messageDisplay();
    delay(5000);
    if ()
    // displayInput();
    
    }

    // xStartDisp++;
    // countCodeKeys++;
    // }

    
   

        // sprintf(codeString1, "%c", enteredCode[0]);
        // displayCharPositionWrite ( 5,0 );
        // displayStringWrite(codeString1);

        // sprintf(codeString2, "%c", enteredCode[1]);
        // displayCharPositionWrite ( 6,0 );
        // displayStringWrite(codeString2);

        // sprintf(codeString3, "%c", enteredCode[2]);
        // displayCharPositionWrite ( 7,0 );
        // displayStringWrite(codeString3);

        // sprintf(codeString4, "%c", enteredCode[3]);
        // displayCharPositionWrite ( 8,0 );
        // displayStringWrite(codeString4);
        
        
        // countCodeKeys = 0;
        // // enteredCode[countCodeKeys] = keypadKey;
        // // displayCharPositionWrite ( 0,0 );
        // // displayStringWrite("Code:           ");
        // emptyList();
        // writeOnLCDCharToString(keypadKey);
        
        
    }
}

static void messageDisplay(){
    if (correctCode){
            displayCharPositionWrite ( 0,0 );
            displayStringWrite("Congratulations ");
            displayCharPositionWrite ( 0,1 );
            displayStringWrite("..Gate is open..");
        }
        else{
            displayCharPositionWrite ( 0,0 );
            displayStringWrite(".# To try again.");
            displayCharPositionWrite ( 0,1 );
            displayStringWrite("Unlimited tries");
        }

}
static void emptyList(){
    for (int i =0; i < 4; i++){
        enteredCode[i] = 0;
    }
}

// static void displayInput(){
//     char codeString[10];
//     for (int i = 0; i < 4; i++){
//     char keyPadKey = enteredCode[i];
//     sprintf(codeString, "%c", keyPadKey);
//     displayCharPositionWrite ( i+5,0 );
//     displayStringWrite(codeString);
//     }
// }


bool isCorrectCode(){
    return correctCode;
}
static void matrixKeypadReset()
{
    matrixKeypadState = MATRIX_KEYPAD_SCANNING;
}
