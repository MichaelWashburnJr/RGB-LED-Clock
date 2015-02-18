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
typedef struct{
  byte r;
  byte g;
  byte b;
}rgb;
rgb Leds[12];
/* Clock Hands */
int hourLed = -1;
int minuteLed = -1;
int secondLed = -1;
 
/******************************************************************************
 * Setup Method
 ******************************************************************************/
void setup() {
  Serial.begin(9600);

  /* Init Leds array */
  for(int i = 0; i < 12; i++){
    Leds[i].r = 0;
    Leds[i].g = 0;
    Leds[i].b = 0;
  }

  /* Initialize ShiftPWM ******************************************************/
  ShiftPWM.SetAmountOfRegisters(numRegisters);
  ShiftPWM.SetPinGrouping(pinGrouping);
  ShiftPWM.Start(pwmFrequency, maxBrightness);
  SetLeds();
  
  /* Initialize Time **********************************************************/
  setSyncProvider(requestSync);
  Serial.println("Waiting for sync message");  

}//End of setup

/******************************************************************************
 * Main Loop
 ******************************************************************************/
void loop() {
  /* Sync the time if serial connection is active */
  if(Serial.available()){
    Serial.println("Serial Available");
    processSyncMessage();
  }
  /* If the time is set */
  if(timeStatus() != timeNotSet){
    //Show the time
    UpdateClockHands();
  }
}//end of main loop

/*****************************************************************************
 * Functions
 *****************************************************************************/

/*---------------------------------------------------------------------------*
 * Clock/Time Keeping Functions
 *---------------------------------------------------------------------------*/
 
 /*
 Returns true if it is the user defined "night" time. */
 boolean IsNightTime(){
   if(hour() >= 20 || hour() <= 8)
   {
     return true;
   }
   else
   {
     return false;
   }
 }
 
 /*
 Updates each hand of the clock face to reflect the actual time. */
 void UpdateClockHands(){
   /* Set the multiplier to one if the current time is during the day */
   byte brightness = 255;
   /* Set brightness to 5 if it is night time */
   if(IsNightTime())
   {
     brightness = 5;
   }
   
   /* Update the hour hand --------------------------------------------------*/
   /* If the old position needs to be blanked */
   if(hourLed >= 0){
     Leds[hourLed].r = 0;
   }
   /* Set the hour hand */
   hourLed = hour() % 12;
   Leds[hourLed].r = brightness;
   
   /* Update the minute hand ------------------------------------------------*/
   /* If the old position needs to be blanked */
   if(minuteLed >= 0)
   {
     Leds[minuteLed].g = 0;
   }
   /* Set the minute hand */
   minuteLed = minute()/5;
   Leds[minuteLed].g = brightness;
  
   /* Update the seconds hand -----------------------------------------------*/
   /* Dont display the seconds hand during night time */
   if(!IsNightTime()){
     /* If the old position needs to be blanks */
     if(secondLed >= 0){
       Leds[secondLed].b = 0;
     }
     /*Set the second hand */
     secondLed = second()/5;
     Leds[secondLed].b = brightness;
   }
  
  SetLeds();
}

/* debug functions ----------------------------------------------------------*/

void printClockDisplay(){
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

/* Time Sync Functions ------------------------------------------------------*/

void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013
  
  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       Serial.println("Time Set");  
       setTime(pctime); // Sync Arduino clock to the time received on the serial port
       adjustTime(-18000);
     }
  }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}

/*---------------------------------------------------------------------------*
 * LED Functions
 *---------------------------------------------------------------------------*/

/*
Adjust the RGB values for their different resistances.  I.e., red is less
resistive than green and blue, while green and blue are almost the same
resistance.  Therefore, to obtain the color white, the power being drawn 
by the red led should be less than that of green and blue. After adjusting,
set the led.
Parameters:
  int led_index - the led to set.
  byte * r - the red byte
  byte * g - the green byte
  byte * b - the blue byte */
void SetLeds(){
  /* For each led */
  for(int i = 0; i < 12; i++)
  {
    /* Adjust the values */
    Leds[i].r *= .5;
    Leds[i].g *= .75;
    Leds[i].b *= 1;
    /* Set the led */
    ShiftPWM.SetRGB(i, Leds[i].r, Leds[i].g, Leds[i].b);
  }
}


