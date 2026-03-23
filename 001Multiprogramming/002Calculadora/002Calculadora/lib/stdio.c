#include "stdio.h"
#include "../os/os.h"
#include <stdarg.h>

//---------------------------------------------stof
static float stof(const char *s) {

	int sign_bit = 1;

	int integer_part = 0;        // parte entera de la mantisa
	float decimal_part = 0.0f;   // parte decimal de la mantisa
	float decimal_scale = 0.1f;  // 10^-1, 10^-2, ...

	int exponent_10 = 0;         // exponente decimal conceptual

	// Signo
	if (*s == '-') {
		sign_bit = -1;
		s++;
	}

	// Parte entera
	while (*s >= '0' && *s <= '9') {
		integer_part = integer_part * 10 + (*s - '0');
		s++;
	}

	// Parte decimal
	if (*s == '.') {
		s++;
		while (*s >= '0' && *s <= '9') {
			decimal_part += (*s - '0') * decimal_scale;
			decimal_scale *= 0.1f;
			exponent_10--;
			s++;
		}
	}

	(void)exponent_10; 

	return sign_bit * (integer_part + decimal_part);
}

//-----------------------------------------ftos
static void ftos(float value) {

	int sign_bit = 1;

	if (value < 0) {
		sign_bit = -1;
		value = -value;
	}

	int integer_part = (int)value;
	float decimal_part = value - integer_part;

	// Signo
	if (sign_bit < 0)
		uart_putc('-');

	// Parte entera
	char int_buffer[16];
	uart_itoa(integer_part, int_buffer);
	uart_puts(int_buffer);

	uart_putc('.');

	// Parte decimal (4 dígitos)
	for (int exponent_10 = 0; exponent_10 < 4; exponent_10++) {
		decimal_part *= 10;
		int digit = (int)decimal_part;
		uart_putc('0' + digit);
		decimal_part -= digit;
	}
}

//-----------------------------------------print
void PRINT(const char *f, ...) {

	va_list args;
	va_start(args, f);

	while (*f) {

		if (*f == '%' && *(f + 1) == 'd') {

			int integer_value = va_arg(args, int);
			char int_buffer[16];

			uart_itoa(integer_value, int_buffer);
			uart_puts(int_buffer);
			f += 2;
		}

		else if (*f == '%' && *(f + 1) == 'f') {

			float float_value = (float)va_arg(args, double);
			ftos(float_value);
			f += 2;
		}

		else {
			uart_putc(*f++);
		}
	}

	va_end(args);
}

//------------------------------------------------read
void READ(const char *f, ...) {

	va_list args;
	va_start(args, f);

	if (*f == '%' && *(f + 1) == 'd') {

		int *integer_target = va_arg(args, int *);
		char input_buffer[32];

		uart_gets_input(input_buffer, sizeof(input_buffer));
		*integer_target = uart_atoi(input_buffer);
	}

	else if (*f == '%' && *(f + 1) == 'f') {

		float *float_target = va_arg(args, float *);
		char input_buffer[32];

		uart_gets_input(input_buffer, sizeof(input_buffer));
		*float_target = stof(input_buffer);
	}

	va_end(args);
}
