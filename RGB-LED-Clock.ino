#include <Time.h>
#define TIME_HEADER "T"
#define TIME_REQUEST 7


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
const int numRegisters = 5;
const int pinGrouping = 12; /* Grouping: 1 == RGBRGB, 2 == RRGGBB, ...*/

/******************************************************************************
 * Global Variables
 ******************************************************************************/
 //empty
 
/******************************************************************************
 * Setup Method
 ******************************************************************************/
void setup() {

  Serial.begin(9600);

  /* Initialize ShiftPWM ******************************************************/
  ShiftPWM.SetAmountOfRegisters(numRegisters);
  ShiftPWM.SetPinGrouping(pinGrouping);
  ShiftPWM.Start(pwmFrequency, maxBrightness);
  
  /* Initialize Time **********************************************************/
  setSyncProvider(requestSync);

}//End of setup

/******************************************************************************
 * Main Loop
 ******************************************************************************/
void loop() {
  
  /* Sync the time if serial connection is active */
  if(Serial.available()){
    processSyncMessage();
  }
  /* If the time is set */
  if(timeStatus() == timeSet){
    //Show the time
  }
  
  ShiftPWM.OneByOneFast();

}//end of main loop

/*****************************************************************************
 * Functions
 *****************************************************************************/

/*---------------------------------------------------------------------------*
 * LED Functions
 *---------------------------------------------------------------------------*/

/*
Adjust the RGB values for their different resistances.  I.e., red is less
resistive than green and blue, while green and blue are almost the same
resistance.  Therefore, to obtain the color white, the power being drawn 
by the red led should be less than that of green and blue.  
Parameters:
  int led_index - the led to set.
  byte * r - the red byte
  byte * g - the green byte
  byte * b - the blue byte */
void SetRGB(int led_index, byte r, byte g, byte b){
  /*Adjust the values */
  r *= .5;
  g *= .75;
  b *= 1;
  /*Set the led */
  ShiftPWM.SetRGB(led_index, r, g, b);
}

/*---------------------------------------------------------------------------*
 * Clock/Time Keeping Functions
 *---------------------------------------------------------------------------*/
  
void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       setTime(pctime); // Sync Arduino clock to the time received on the serial port
     }
  }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}


