#ifndef TECLADO_H_
#define TECLADO_H_

#include <avr/io.h>
#include <util/delay.h>

void setup_teclado(void);
char teclado_scan(void);

#endif