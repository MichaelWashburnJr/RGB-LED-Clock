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
  } else{/* else, flash leds in some manor */
    CycleLeds(500);
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
   /* 0 <= fadePercent <= 1
    fadePercent is the percent of brightness the first hand led should have.
    1 - fadePercent is the percent of brightness the led after the first hand
    led should have. (fades the led to the next led depending on how far along
    the time is.  mimicks real clock hands moving between numbers. ). */ 
   float fadePercent = 1; 
   SetAllLedValues(0,0,0);
   /* Set the multiplier to one if the current time is during the day */
   byte brightness = 255;
   /* Set brightness to 5 if it is night time */
   if(IsNightTime())
   {
     brightness = 5;
   }
   
   /* Update the hour hand --------------------------------------------------*/
   /* Set the hour hand */
   hourLed = hour() % 12;
   Leds[hourLed].r = brightness;
   
   /* Update the minute hand ------------------------------------------------*/
   /* Set the minute hand */
   minuteLed = minute()/5;
   fadePercent = 5-((minute() + second()/60) % 5);
   fadePercent /= 5;
   Leds[minuteLed].g = brightness * fadePercent;
   Leds[(minuteLed+1)%12].g = brightness * (1-fadePercent);
  
   /* Update the seconds hand -----------------------------------------------*/
   /* Dont display the seconds hand during night time */
   if(!IsNightTime()){
     /*Set the second hand */
     secondLed = second()/5;
     fadePercent = 5 - (second()%5);
     fadePercent /= 5;
     Leds[secondLed].b = brightness * fadePercent;
     Leds[(secondLed+1)%12].b = brightness * (1-fadePercent);
     
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
       adjustTime(-1*(4*3600));
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
 Set all leds to red, green, or blue, then shift them repetedly, clockwise,
 one full cycle (3 shifts clockwise). */
 void CycleLeds(int timeout){
   rgb colors[3] = {
     {255,0,0},
     {0,255,0},
     {0,0,255}
   };
   int colorIndex = 0;
   /* Set all leds to be individually all red, then all green, then all blue,
      in that order, until there are no more leds to set */
   for(int i = 0; i < 36; i++){ /* 36 = 12 leds * 3 iterations */
     Leds[i%12].r = colors[colorIndex].r;
     Leds[i%12].g = colors[colorIndex].g;
     Leds[i%12].b = colors[colorIndex].b;
     
     /* After all 12 leds have been set delay*/
     if(i % 12 == 0)
     {
       SetLeds();
       delay(100);
       /* increment color index by an extra one */
       colorIndex++;
     }
     /* Increment the color index by one */
     colorIndex = (colorIndex + 1) % 3;
   }
 }
 
/*
Sets all leds to the value but doesn't shift out. */
void SetAllLedValues(byte r, byte g, byte b){
  for(int i = 0; i < 12; i++){
    Leds[i].r = r;
    Leds[i].g = g;
    Leds[i].b = b;
  }
}

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


