# IRPatternRecorder
An AVR microcontroller based project to record the LED patterns
I wrote this firmware to help me dump all the data from a remote control, by tapping directly into the IR LED.
This was originally written for an atmega8, please change the connections and code to the microcontroller you're porting this firmware to
These are all the connections you have to make

PB0	->  LED (power LED?)
PB1	->  Status LED
PD0(RX)	->  TX
PD1(TX) ->  RX
PD2	->  IR LED
