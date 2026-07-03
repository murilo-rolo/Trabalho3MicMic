/*
 * TrabalhoMicMic3.c
 *
 * Created: 23/06/2026 15:59:07
 * Author : muril
 */ 

#define F_CPU 16000000UL // Avisa o compilador que estamos em 16 MHz
#include <avr/io.h>
#include "teclado.h"

int main(void)
{
	setup_teclado();

	while (1) {
		char tecla = teclado_scan();
		if (tecla != 0) {
			// aqui você já recebeu uma tecla válida
		}
	}
}

