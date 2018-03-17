/*  Copyright (C) 2018 Shrinath Nimare
    This file is part of IRPatternRecorder
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define F_CPU 16000000UL
#include </usr/avr/include/avr/io.h>
#include </usr/avr/include/util/delay.h>
#include </usr/avr/include/avr/eeprom.h>
#include </usr/avr/include/avr/interrupt.h>
#define SAMPLES 400

typedef unsigned int WORD;
typedef unsigned char BYTE;
WORD memory[SAMPLES],temp, counter, tempcounter;

//interupt setup
void inter()
{
    MCUCR |= (1 << ISC00); //any logical change will trigger INT0
    GICR |= (1 << INT0); //enabling INT0
    PORTD |= (1 << PD2); //enabling the PD2 pin pull up resistor
}

//uart interrupt setup
void uart_init()
{
    UCSRB |= (1 << TXEN) | (1 << RXEN) | (1 << RXCIE); //enabling TX, RX and RX interrupt
    UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1); //8 data bits
//    UBRRL = 0b00000100; //300 BAUD
//    UBRRH = 0b1101;
    UBRRL = 103; //setting baud rate to 9600, this can be changed for higher or lower
}

//routine to send data through uart
void uart_send(BYTE data)
{
    while((UCSRA & (1 << UDRE)) == 0){}; //while the UDR is not empty, loop this
    UDR = data; //write data out
}

//INT0 service routine
ISR(INT0_vect)
{
    memory[counter++] = TCNT1; //saving to memory, incrementing counter after
    TCNT1 = 0; //reseting timer
    memory[counter - 1] |= (~PIND & (1 << PD2))?(1 << 15):0;
    if(counter > SAMPLES) //if the sampling is done, turn on the status led, aka PB1
    {
	GICR &= ~(1 << INT0); //we turn off the INT0 (this) interrupt too
	PORTB = 0b11;
    }
}

//RX complete service routine
ISR(USART_RXC_vect)
{
    //test message
    uart_send('T');
    uart_send('e');
    uart_send('s');
    uart_send('t');
    uart_send('\n');
    uart_send('\r'); //return carriage
    for(tempcounter = 0; tempcounter < SAMPLES;tempcounter++) //loop to print all the data out of uart. tempcounter is used so that even if not all SAMPLES are taken, the data can still be dumped
    {
	temp = memory[tempcounter]; //setting temp as memory
	if(temp & (1 << 15))
	    uart_send('+');
	else
	    uart_send('-');
	temp &= ~(1 << 15);
	//binary format
	/*
	uart_send((temp >> 8) & 0xff);
	uart_send(temp & 0xff);
	*/
	//ASCII format, sending the data as A
	uart_send(48 + (temp / 10000) % 10);
	uart_send(48 + (temp / 1000) % 10);
	uart_send(48 + (temp / 100) % 10);
	uart_send(48 + (temp / 10) % 10);
	uart_send(48 + (temp % 10));
	uart_send('\n'); //newline
	uart_send('\r'); //return carriage
    }
    temp = UDR; //reading UDR because the datasheet states that in the ISR for RXC, UDR has to be read at least once to avoid an interrupt loop
}

void main()
{
    cli(); //disabling interrupts before the starup process
    //blinky to show booting up
    DDRB = 0b11; //setting PB0 and PB1 as output
    PORTB = 1; //PB0 turns on
    _delay_ms(1000); //delay 1 sec
    PORTB = 0; //PB0 turns off
    _delay_ms(1000); //delay 1 sec
    PORTB = 1; //PB0 turns on again
    TCCR1B = (1 << CS10); //timer init, ticking at F_CPU, this can be changed
    uart_init(); //uart setup
    for(counter = 0; counter < SAMPLES; counter++) //setting all memory as 0
	memory[counter] = 0;
    counter = 0; //setting counter back to 0
    inter(); //setup interrupts
    sei(); //enabling interrupts
    while(1) //forever loop
    {
    }
}
