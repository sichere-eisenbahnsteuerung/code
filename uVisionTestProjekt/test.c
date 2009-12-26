/*------------------------------------------------------------------------------
test.c
------------------------------------------------------------------------------*/

#include <reg515C.h>				/* define 8051 registers */
#include <stdio.h>				/* define I/O functions */
#include "test.h";

void main (void)  {				/* main program */
	byte n1, n2, n3;			/* define operation registers */
	byte z = 1;

	SCON = 0x52;	/* SCON */		/* setup serial port control */
	TMOD = 0x20;	/* TMOD */		/* hardware (2400 BAUD @12MHZ) */
	TCON = 0x69;	/* TCON */
	TH1 =  0xf3;	/* TH1 */

	printf ("\n\nC compiler demonstration program\n\n");
	n1 = 1;
	n2 = 2;

	while (z++ < 20)  {
		n1++;
		n2++;
		n3 = n1 + n2;
	}
}
