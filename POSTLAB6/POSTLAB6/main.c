/*
 * POSTLAB6.c
 *
 * Created: 28/04/2025 00:00:45
 * Author : Rodrigo Lara
 */ 
#define F_CPU 16000000UL  // Frecuencia del Arduino Nano
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

// Definiciones de pines
#define LED_PORT1 PORTC  // A0-A5 
#define LED_DDR1  DDRC
#define LED_PORT2 PORTB  // D8, D9 
#define LED_DDR2  DDRB
#define POT_PIN   6      // A6 

// Variables globales
uint8_t modo = 0;           // 0: menú, 1: potenciómetro, 2: ascii
char ultimo_char = 0;

// Función para inicializar UART
void UART_init(unsigned int ubrr) {
	UBRR0H = (ubrr >> 8);
	UBRR0L = ubrr;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);             // Habilita recepción y transmisión
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);           // 8 bits, sin paridad
}

// Mostrar menú
void mostrar_menu() {
	
	UART_print("1. Leer Potenciometro\r\n");
	UART_print("2. Enviar ASCII\r\n");
	
}

// Transmitir carácter
void UART_tx(char c) {
	while (!(UCSR0A & (1 << UDRE0)));  // Espera a que esté listo
	UDR0 = c;
}

// Transmitir string
void UART_print(const char* str) {
	while (*str) {
		UART_tx(*str++);
	}
}

// Leer carácter recibido
char UART_rx() {
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

// Leer string (hasta '\n')
void UART_readline(char* buffer, uint8_t max_len) {
	uint8_t i = 0;
	char c;
	while (i < max_len - 1) {
		c = UART_rx();
		if (c == '\n' || c == '\r') break;
		buffer[i++] = c;
	}
	buffer[i] = '\0';
}

// Mostrar en LEDs
void mostrar_en_LEDs(uint8_t valor) {
	// Parte baja: PC0 - PC5
	LED_PORT1 = (LED_PORT1 & 0xC0) | (valor & 0x3F);
	// Parte alta: PB0 - PB1
	LED_PORT2 = (LED_PORT2 & 0xFC) | ((valor >> 6) & 0x03);
}



// Inicializar ADC
void ADC_init() {
	ADMUX = (1 << REFS0) | POT_PIN;  // AVcc como referencia, canal ADC6 (A6)
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);  // Habilita ADC, prescaler 64
}

// Leer ADC
uint16_t ADC_read() {
	ADMUX = (ADMUX & 0xF0) | (POT_PIN & 0x0F); // Selección del canal
	ADCSRA |= (1 << ADSC);  // Inicia conversión
	while (ADCSRA & (1 << ADSC));  // Espera fin
	return ADC;
}

// Mostrar número en serial
void UART_print_int(uint16_t num) {
	char buffer[6];
	itoa(num, buffer, 10);
	UART_print(buffer);
	UART_print("\r\n");
}

int main(void) {
	char buffer[16];
	DDRC = 0x3F;  // PC0-PC5 como salida
	DDRB |= 0x03; // PB0 y PB1 como salida

	UART_init(103);  // 9600 baudios con F_CPU = 16 MHz
	ADC_init();

	mostrar_menu();

	while (1) {
		if (UCSR0A & (1 << RXC0)) {
			UART_readline(buffer, sizeof(buffer));

			if (strcmp(buffer, "00") == 0) {
				modo = 0;
				UART_print("\nRegresando al menu...\r\n");
				mostrar_menu();
				continue;
			}

			if (modo == 1) {
				if (strcmp(buffer, "1") == 0) {
					modo = 1;
					UART_print("\nModo Potenciometro activado\r\n");
					} else if (strcmp(buffer, "2") == 0) {
					modo = 2;
					UART_print("\nModo ASCII activado. Envía un caracter\r\n");
					} else {
					UART_print("Opción invalida\r\n");
					mostrar_menu();
				}
				} else if (modo == 2 && strlen(buffer) > 0) {
				ultimo_char = buffer[0];
				UART_print("Carácter recibido: ");
				UART_tx(ultimo_char);
				UART_print("\r\n");
				mostrar_en_LEDs(ultimo_char);
			}
		}

		if (modo == 2) {
			uint16_t val = ADC_read();
			uint8_t val8 = val >> 2;  // convertir a 8 bits
			UART_print("Potenciómetro: ");
			UART_print_int(val);
			mostrar_en_LEDs(val8);
			_delay_ms(300);
			} else if (modo == 2) {
			mostrar_en_LEDs(ultimo_char);
		}
	}
}
