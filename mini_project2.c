#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

unsigned char tick =0;
unsigned char seconds =0;
unsigned char minutes =0;
unsigned char hours =0;

void INT (void)
{
	SREG &=~(1<<7); //disable I-bit
	// enable the 3 external interrupts
	GICR |=(1<<INT0) |(1<<INT1) |(1<<INT2);
	MCUCR |=(1<<ISC01); //INT0 with falling edge
	MCUCR |=(1<<ISC11) |(1<<ISC10) ; //INT1 with raising edge
	MCUCSR &=~(1<<ISC2); // INT2 with falling edge
	DDRD  &= (~(1<<PD2));     // Configure INT0/PD2 as input pin
	DDRD  &= (~(1<<PD3));  // Configure INT1/PD3 as input pin
	DDRB  &= (~(1<<PB2));   // Configure INT2/PB2 as input pin
	PORTD |=(1<<PD2);   //enable internal pull up in INT0
	PORTB |=(1<<PB2);   //enable internal pull up in INT2
	SREG |=(1<<7); //enable I-bit

}

void TIMER1 (unsigned short TICK)
{

	TCNT1=0; //set timer initial value to 0
	OCR1A= TICK; //set compare value
	TCCR1A |=(1<<FOC1A) |(1<<FOC1B); // non-PWM mode
	TIMSK |=(1<<OCIE1A);
	TCCR1B |=(1<<WGM12); // CTC mode
	TCCR1B |=(1<<CS12) |(1<<CS10); // prescaler 1024
	SREG |=(1<<7); //enable I-bit
}

ISR(INT0_vect)
{
	seconds =0;
	minutes =0;
	hours =0;

}

ISR(INT1_vect)
{
	TCCR1B &= 0XF8; // No clock source (Timer/Counter stopped)
}

ISR (INT2_vect)
{

	TCCR1B |=(1<<CS10) |(1<<CS12);

}

ISR (TIMER1_COMPA_vect)
{
	tick =1;
}

int main (void)
{
	DDRC |= 0x0F; 	 	  // Configure the first four pins in PORTC as output pins.
	PORTC &= 0xF0;   	 // 7-segment display zero at the beginning.
	DDRA |= 0x3F;   	// Configure the first six pins in PORTA s the enable/disable pins for the six 7-segments.
	//PORTA &= 0xC0;   //disable pins in PORTA
	INT();            //enable external interrupts
	TIMER1(1000);
	while(1)
	{
		if (tick ==1)
		{
			seconds++;
			if(seconds==60)
			{
				seconds=0;
				minutes++;
			}
			if(minutes==60)
			{
				minutes=0;
				hours++;
			}
			tick=0;
		}
		else
		{

			PORTA = (PORTA & 0xC0) | 0x01;
			PORTC = (PORTC & 0xF0) | (seconds%10);


			_delay_ms(2);

			PORTA = (PORTA & 0xC0) | 0x02;
			PORTC = (PORTC & 0xF0) | (seconds/10);

			_delay_ms(2);


			PORTA = (PORTA & 0xC0) | 0x04;
			PORTC = (PORTC & 0xF0) | (minutes%10);

			_delay_ms(2);

			PORTA = (PORTA & 0xC0) | 0x08;
			PORTC = (PORTC & 0xF0) | (minutes/10);

			_delay_ms(2);


			PORTA = (PORTA & 0xC0) | 0x10;
			PORTC = (PORTC & 0xF0) | (hours%10);

			_delay_ms(2);

			PORTA = (PORTA & 0xC0) | 0x20;
			PORTC = (PORTC & 0xF0) | (hours/10);

			_delay_ms(2);

		}
	}
}

