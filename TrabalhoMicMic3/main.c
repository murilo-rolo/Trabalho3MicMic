/*
 * TrabalhoMicMic3.c
 *
 * Created: 23/06/2026 15:59:07
 * Author : muril
 */ 

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

