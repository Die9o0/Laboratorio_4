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
#include <stdbool.h>
/****************************************/
// Function prototypes
void setup();
void initADC();
void initTMR0();

//Valores a usar
volatile uint8_t contador	= 0;
volatile uint8_t LecturaADC = 0;
volatile uint8_t LecturaADCH= 0;
volatile uint8_t LecturaADCL= 0;
volatile uint8_t ADCCounter = 0;
volatile bool display_flag;
const uint8_t TMR0_VALUE = 100;
const uint8_t tabladisplay[16] = {
	0b0111111, //0
	0b0000110, //1
	0b1011011, //2
	0b1001111, //3
	0b1100110, //4
	0b1101101, //5
	0b1111101, //6
	0b0000111, //7
	0b1111111, //8
	0b1101111, //9
	0b1110111, //A
	0b1111100, //B
	0b0111001, //C
	0b1011110, //D
	0b1111001, //E
	0b1110001 //F	
};
/****************************************/
// Main Function
int main(void)
{
	//Setup
	cli();
	setup();
	initADC();
	initTMR0();
	ADCSRA	|= (1<<ADSC) | (1<<ADIE);
	sei();
	//Loop
	while(1)
	{
		PORTC &= 0xF3;								//Apaga displays
		if (display_flag)							//Revisa que display encender ahora
		{
			PORTD = tabladisplay[LecturaADCH>>4];	//Envia a PORTD el valor del display
			PORTC |= 0x04;							//Enciende el display correspondiente
		} else {
			PORTD = tabladisplay[LecturaADCL];		//Envia a PORTD el valor del display
			PORTC |= 0x08;							//Enciende el display correspondiente
		}
		if (LecturaADC>contador)					//Revisa si el valor en el potenciómetro es mayor al contador
		{
			PORTD |= 0x80;							//Si sí, enciende el led en PORTD7
		} else {
			PORTD &= 0x7F;							//Si no, apaga el led en PORTD7
		}
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
	//Habilitar ADC y prescaler de 16 para 62.5 kHz
	ADCSRA	= (1<<ADEN) | (1<<ADPS2);
}
void initTMR0()
{
	//Interrupción cada 10ms
	TCCR0B = 0;
	TCCR0B |= (1 << CS01) |	(1 << CS00);
	TIMSK0 = (1 << TOIE0);
	TCNT0 = TMR0_VALUE;
}
/****************************************/
// Interrupt routines
ISR(ADC_vect)
{
	LecturaADCH	= ADCH & 0xF0;		//Nibble alto de la lectura
	LecturaADCL	= ADCH & 0x0F;		//Nibble bajo de la lectura
	LecturaADC	= ADCH;				//Lectura completa
}
ISR(PCINT1_vect)
{
	//Dependiendo de que botón se haya pulsado, aumenta o disminuye el contador
	uint8_t B1 = (PINC & 0x10);		
	uint8_t B2 = (PINC & 0x20);
	if (B2==0)
	{
		contador++;
	} else if(B1==0){
		contador--;
	}
	//Separa el contador para enviar una parte en cada puerto
	PORTB = contador & 0x3F;					//6 bits en B
	PORTC = (0xFC |((contador>>6) & 0x03));		//2 bits en C, moviendo los 2 más significativos de contador a los menos de C, y sin alterar el resto de C
}
ISR(TIMER0_OVF_vect)
{
	TCNT0		 = TMR0_VALUE;		//Carga el valor correspondiente a TCNT0
	display_flag = !display_flag;	//Alterna el display a encender
	//Cuenta 10 interrupciones
	ADCCounter ++;					
	if (ADCCounter == 10)
	{
	ADCCounter	= 0;				//Reinicia
	ADCSRA		|= (1<<ADSC);		//Inicia lectura ADC
	}
}