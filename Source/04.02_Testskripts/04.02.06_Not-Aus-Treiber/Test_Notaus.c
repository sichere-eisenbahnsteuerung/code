/*****************************************************************************
 *
 *        Dateiname:    Test_NotAus.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Jan-Christopher Icken
 *
 *
 *        Modul:        Testskript für das RS232-Treiber-Modul
 *
 *        Beschreibung:
 *        
 *        _______________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *
 ****************************************************************************/

/* Includes *****************************************************************/
#include "Notaus.h"
#include "Test_Notaus.h"
#include <stdio.h>

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/

/* Prototypen fuer lokale Funktionen ****************************************/
void run_notaus_test1(void);  
/* Funktionsimplementierungen ***********************************************/

void work_notaus_tests(void) {
	//Serielle Schnittstelle für Debug Ausgabe einrichten
	BD = 1; // Baudratengenerator einschalten
	SM0 = 0; // Mode 1  8Bit variable Baudrate
	SM1 = 1;
	SRELH = 0x03; // 9600 Baud
	SRELL = 0xDF;
	REN = 1; // seriellen Empfang einschalten
	TI = 1;
	run_notaus_test1();	     
}

void run_noutaus_test1(void) {
	int i;
	for(i = 0; i < 3;i++) { //Test 3x durchlaufen lassen
		initNOTAUS();
		if(NOTAUS_PIN == TRUE) {
			printf("Notaus Relais erfolgreich aktiviert.\n");
		}
		else {
			printf("Notaus Relais nicht erfolgreich aktiviert.\n");
		}
		emergency_off();
		if(NOTAUS_PIN == FALSE) {
			printf("Notaus Relais erfolgreich deaktiviert.\n");
		}
		else {
			printf("Notaus Relais nicht erfolgreich deaktiviert.\n");
		}
	}
	
}