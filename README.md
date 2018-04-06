# IRPatternRecorder
An AVR microcontroller based project to record the LED patterns
I wrote this firmware to help me dump all the data from a remote control, by tapping directly into the IR LED.
This was originally written for an atmega8, please change the connections and code to the microcontroller you're porting this firmware to

These are all the connections you have to make

PB0	->  LED (power LED?)<br>
PB1	->  Status LED<br>
PD0(RX)	->  TX<br>
PD1(TX) ->  RX<br>
PD2	->  IR LED<br>
