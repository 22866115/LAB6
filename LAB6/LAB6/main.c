/*
 * LAB6.c
 *
 * Created: 22/04/2025 14:33:57
 * Author : Rodrigo Lara
 */ 

#define F_CPU 16000000UL			//Frecuencia de 16MHZ

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//Definir prototipo de funciones
void initUART9600(void);			//El microcontrolador solo tiene un modulo UART
void writeUART(char caractrer);		//Funci?n para escribir caracteres

void writeTextUART(char* texto);	//Automatizar mensajes

volatile char bufferRX;				//Se pine volatile a las variables que pueden cambiar en cualquier momento

int main(void)
{
    initUART9600();		//Se llama a la funci?n de UART
	sei();				//Se activan las interupcciones Globales
	
	//Configuraci?n de los pines para las salidas (LEDS)
	DDRB = 0xFF;
	PORTB = 0;
	
	DDRD = 0xFF;
	PORTD = 0;
	
	//Mandar caracteres al UART
	writeUART('H');
	writeUART('o');
	writeUART('l');
	writeUART('a');
	writeUART(' ');
	
	//Mandar cadena de caracteres al UART
	writeTextUART("\n\nMuchas gracias por usar el programa\n");
	
    while (1) 
    {
		//Configuraci?n salidas del ASCII
		PORTB = bufferRX;					//Muestra los pimeros 6 bits 0b00111111 -> PB0 a PB5
		PORTD = (bufferRX<<6);				//Muestra los ultimos 2 bits 0b11000000 -> PD6 a PD7
		
    }
}

//Inicializar la funci?n del UART
void initUART9600(void){
	//Paso1: Configurar pines TX y RX
	DDRD &= ~(1<<DDD0);		//Se configura el RX (PD0) como entrada
	DDRD |= (1<<DDD1);		//Se configura el TX (PD1) como salida
	
	//Paso2: Configurar registro A ---> Modo FAST U2X0 = 1
	UCSR0A = 0;
	UCSR0A |= (1<<U2X0);
	
	//Paso3: Configurar registro B --> Habilitar ISR RX, se habilita RX y TX
	//->>Se configura para tener interrupciones
	UCSR0B = 0;		//Se pone en 0 para mayor facilidad de configurar posteriomente
	UCSR0B |= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	
	//Paso4: Configurar C > Frame (Se define el frame): 8 bits datos, no paridad, 1 bit de stop
	UCSR0C = 0;    //--> Se configura si se desea modo: asincrono o sincrono
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);	//Se configura el tama?o del caracter --> 8 bits
	
	//Paso5: Baudrate = 9600
	UBRR0 = 207;
}

void writeUART(char caractrer){
	while(!(UCSR0A &(1<<UDRE0)));		//La funci?n se queda en espera hasta que UDR este en 1
	UDR0 = caractrer;
}

void writeTextUART(char* texto){			// "*" es un puntero del char
	uint8_t i;
	for(i = 0; texto[i]!= '\0'; i++){
		while(!(UCSR0A &(1<<UDRE0)));		//La funci?n se queda en espera hasta que UDR este en 1
		UDR0 = texto[i];
	}
}

//Escribir desde la computador (mandar un eco)
ISR (USART_RX_vect){
	bufferRX = UDR0;
	
	while(!(UCSR0A &(1<<UDRE0)));		//La funci?n se queda en espera hasta que UDR este en 1
	UDR0 = bufferRX;
}

