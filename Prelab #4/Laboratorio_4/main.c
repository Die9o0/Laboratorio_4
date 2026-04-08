/*
 * Laboratorio_4.c
 *
 * Created: 5/04/26
 * Author: Diego Camey
 * Description: Laboratorio #4 Porgramación de microcontroladores
 */
/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
/****************************************/
// Function prototypes
void setup();
void initADC();

//Valores a usar
volatile uint8_t contador= 0;

/****************************************/
// Main Function
int main(void)
{
	cli();
	setup();
	//ADCSRA	|= (1<<ADSC) | (1<<ADIE);
	sei();
	while(1)
	{
		
	}
}
/****************************************/
// NON-Interrupt subroutines
void setup()
{ 
	//Sistema a 1MHz
	CLKPR	= (1<<CLKPCE);
	CLKPR	= (1<<CLKPS2);
	//Entradas y salidas
	DDRD	= 0xFF;
	PORTD	= 0x00;
	UCSR0B	= 0x00;
	DDRC	= 0x0F;
	PORTC	= 0x30;
	DDRB	= 0x3F;
	PORTB	= 0x00;
	//Interrupciones puerto C en bits 4 y 5
	PCICR	= (1<<PCIE1);
	PCMSK1	= (1<<PCINT12) | (1<<PCINT13);
}
void initADC()
{
	//Aref = AVcc; Izquierda; ADC6
	ADMUX	= 0;
	ADMUX	|= (1<<REFS0) |  (1<<ADLAR) | (1<<MUX2) | (1<<MUX1);
	//Habilitar ADC y prescaler de 8 para 125kHz
	ADCSRA	= (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);
}
/****************************************/
// Interrupt routines
ISR(ADC_vect)
{
	ADCSRA	|= (1<<ADSC);
}
ISR(PCINT1_vect)
{
	uint8_t B1 = (PINC & 0x10);
	uint8_t B2 = (PINC & 0x20);
	if (B1==0)
	{
		contador++;
	} else if(B2==0){
		contador--;
	}

	PORTB = contador & 0x3F;
	PORTC = (0xFC |((contador>>6) & 0x03));
}