#ifndef OS_H
#define OS_H

void uart_putc(char c);
char uart_getc(void);
void uart_puts(const char *s);
void uart_gets_input(char *buffer, int max_length);
int uart_atoi(const char *s);
void uart_itoa(int num, char *buffer);

#endif