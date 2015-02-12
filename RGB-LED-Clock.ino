/******************************************************************************
 * ShiftPWM Configuration
 ******************************************************************************/

 /* Initialize these before including ShiftPWM.h */
const int ShiftPWM_latchPin=8;
//Uncomment the below lines to disable SPI
// #define SHIFTPWM_NOSPI
// const int ShiftPWM_dataPin = 11;
// const int ShiftPWM_clockPin = 13;
const bool ShiftPWM_invertOutputs = true;
const bool ShiftPWM_balanceLoad = false;

#include <ShiftPWM.h>

const unsigned char maxBrightness = 255;
const unsigned char pwmFrequency = 75;
const int numRegisters = 1;
const int pinGrouping = 1; /* Grouping: 1 == RGBRGB, 2 == RRGGBB, ...*/

/******************************************************************************
 * Global Variables
 ******************************************************************************/
 //empty

 /*****************************************************************************
  * Helper Functions
  *****************************************************************************/

  /*
  Adjust the RGB values for their different resistances.  I.e., red is less
  resistive than green and blue, while green and blue are almost the same
  resistance.  Therefore, to obtain the color white, the power being drawn 
  by the red led should be less than that of green and blue.  
  Parameters:
    byte * r - Pointer to the red byte    (returns)
    byte * g - Pointer to the green byte  (returns)
    byte * b - Pointer to the blue byte   (returns) */
  void AdjustRGB(byte * r, byte * g, byte * b){
    *r = (*r) * .5;
    *b = (*b) * 1;
    *g = (*g) * 1;
  }


/******************************************************************************
 * Setup Method
 ******************************************************************************/
void setup() {

  Serial.begin(9600);

  /* Initialize ShiftPWM ******************************************************/
  ShiftPWM.SetAmountOfRegisters(numRegisters);
  ShiftPWM.SetPinGrouping(pinGrouping);
  ShiftPWM.Start(pwmFrequency, maxBrightness);

}//End of setup

/******************************************************************************
 * Main Loop
 ******************************************************************************/
void loop() {

  //test for 1 shift register with 2 rgb leds 
  ShiftPWM.SetRGB(0,255,0,0);
  ShiftPWM.SetRGB(1,0,255,0);

}//end of main loop

