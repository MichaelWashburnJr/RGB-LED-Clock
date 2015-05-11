# RGB-LED-Clock
An Analog clock built using RGB LEDs and an Arduino.

##Objective:
To create an analog clock from a preexisting clock shell (at minimum, a standard clock face) using 12 RGBLEDs, and Arduino, and several shift registers.  Each clock hand will be a different color.  The hour hand will be red, the minute hand will be green, and the second hand will be blue.  Because I am only using 12 LEDs, the clock will be limited to telling time to the nearest 5 minutes of an hour, and the second hand will update every 5 seconds.

##Current Configuration:
- Arduino Uno
- 12x Common Anode RGB LED
- 5x 74HC595 8-bit shift register
- 1x 200 Ohm resistor

##Syncronization Script:
To sync the clock with real time install pyserial with pip:
```
pip install pyserial
```
Then run:
```
python sync.py [Com Port Number]
```

![alt tag](https://github.com/mdw7326/RGB-LED-Clock/blob/master/RGB-LED-Clock.jpg)
