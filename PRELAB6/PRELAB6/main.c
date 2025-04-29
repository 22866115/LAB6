/*
 * PRELAB6.c
 *
 * Created: 21/04/2025 23:58:20
 * Author : Rodrigo Lara
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

// Inicialización del UART
void uart_init(unsigned int ubrr) {
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;

	// Habilita recepción y transmisión
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	// Configura: 8 bits de datos, sin paridad, 1 bit de stop
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Enviar carácter
void uart_transmit(unsigned char data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

// Recibir carácter
unsigned char uart_receive(void) {
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

int main(void) {
	uart_init(MYUBRR);   // Inicializar UART
	DDRB = 0xFF;         // Puerto B como salida
	PORTB = 0x00;

	uart_transmit('H');  // Parte 1: Enviar un carácter

	while (1) {
		// Parte 2: Mostrar carácter recibido en el puerto B
		unsigned char received = uart_receive();
		PORTB = received;
	}
}
