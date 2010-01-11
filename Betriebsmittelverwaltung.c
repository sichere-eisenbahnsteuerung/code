#include <reg515C.h>				/* define 8051 registers */
#include <stdio.h>
#include "Betriebsmittelverwaltung.h"
#include "Befehlsvalidierung.h"

void main(void) {
	byte z;	

	SCON = 0x52;	/* SCON */		/* setup serial port control */
	TMOD = 0x20;	/* TMOD */		/* hardware (2400 BAUD @12MHZ) */
	TCON = 0x69;	/* TCON */
	TH1 =  0xf3;	/* TH1 */

	printf ("\n\nC compiler demonstration program\n\n");

	initBV();

	for (z=1; z < 5; z++)  {
		workBV();
	}

	while(TRUE); // Endlosschleife
}