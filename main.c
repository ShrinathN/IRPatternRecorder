#define F_CPU 16000000UL
#include </usr/avr/include/avr/io.h>
#include </usr/avr/include/util/delay.h>
#include </usr/avr/include/avr/eeprom.h>
#include </usr/avr/include/avr/interrupt.h>
#define SAMPLES 100

typedef unsigned int WORD;
typedef unsigned char BYTE;
WORD memory[SAMPLES],temp;
BYTE counter = 0, tempcounter = 0;

void inter()
{
    MCUCR |= (1 << ISC10);
    GICR |= (1 << INT0);
    PORTD |= (1 << PD2); //enabling the PD2 pin pull up resistor
}

void uart_init()
{
    UCSRB |= (1 << TXEN) | (1 << RXEN) | (1 << RXCIE);
    UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);
    UBRRL = 103;
}

void uart_send(BYTE data)
{
    while(UCSRA & (1 << UDRE) == 0);
    UDR = data;
}

ISR(INT0_vect)
{
    memory[counter++] = TCNT1;
    TCNT1 = 0;
    if(~PIND & (1 << PD2))
    {
	memory[counter-1] |= 1 << 15;
    }
    if(counter > SAMPLES)
    {
	GICR &= ~(1 << INT0);
	PORTB = 0b11;
    }
}

ISR(USART_RXC_vect)
{
    uart_send('T');
    uart_send('e');
    uart_send('s');
    uart_send('t');
    for(tempcounter = 0; tempcounter < SAMPLES;tempcounter++)
    {
	temp = memory[tempcounter];
	if(temp & (1 << 15))
	    uart_send('+');
	else
	    uart_send('-');
	temp &= ~(1 << 15);
	//binary
	/*
	uart_send((temp >> 8) & 0xff);
	uart_send(temp & 0xff);
	*/
	//ASCII coded
	uart_send(48 + (temp / 10000) % 10);
	uart_send(48 + (temp / 1000) % 10);
	uart_send(48 + (temp / 100) % 10);
	uart_send(48 + (temp / 10) % 10);
	uart_send(48 + (temp % 10));
	uart_send('\t');
    }

}

void main()
{
    cli();
    //blinky to show booting up
    DDRB = 0b11;
    PORTB = 1;
    _delay_ms(1000);
    PORTB = 0;
    _delay_ms(1000);
    PORTB = 1;
    TCCR1B = (1 << CS10);//timer init, starting with F_CPU/8
    uart_init();
    for(counter = 0; counter < SAMPLES; counter++)
	memory[counter] = 0;
    counter = 0;
    inter(); //setup interrupts
    sei();
    while(1)
    {
    }
}
