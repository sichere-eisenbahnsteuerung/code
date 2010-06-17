/*****************************************************************************
 *
 *        Dateiname:    Test_SSCTreiber.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Jan-Christopher Icken
 *
 *
 *        Modul:        Testskript für das SSC-Treiber-Modul
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
#include "SSCTreiber.h"
#include "Test_SSCTreiber.h"
#include "Betriebsmittelverwaltung.h"
#include <stdio.h>

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/
int k;
int l;
/* Prototypen fuer lokale Funktionen ****************************************/
void run_ssctest_1(void);
void run_ssctest_2(void);
void run_ssctest_3(void);
void run_ssctest_4(void);
void run_ssctest_5(void);
/* Funktionsimplementierungen ***********************************************/
void work_ssc_tests(void) {
	//Serielle Schnittstelle für Debug Ausgabe einrichten
	BD = 1; // Baudratengenerator einschalten
	SM0 = 0; // Mode 1  8Bit variable Baudrate
	SM1 = 1;
	SRELH = 0x03; // 9600 Baud
	SRELL = 0xDF;
	REN = 1; // seriellen Empfang einschalten
	TI = 1;
	run_ssctest_1(); //Ersten Test durchführen
	run_ssctest_2(); //Zweiten Test durchführen
	run_ssctest_3(); //Dritten Test durchführen
	run_ssctest_4();
	run_ssctest_5();
	for(;;) {
	}   
}

void run_ssctest_1(void) {
	//Modul initialisieren
	initSSC();
	//zu vergleichenden Streckenbefehl in Shared Memory laden
	EV_SSC_streckenbefehl.Lok = 0x25;
	EV_SSC_streckenbefehl.Weiche = 0x92;
	EV_SSC_streckenbefehl.Entkoppler = 0x5D;
	for(k = 0; k < 3;k++) { //Test 3x durchlaufen lassen
	
	//Streckenbefehle versenden  
	workSSC();
	workSSC();
	workSSC();
	//Entgegengenommene Bytes vergleichen
	  
	
		if(SSC_EV_streckenbefehl.Lok == 0x25 && SSC_EV_streckenbefehl.Weiche == 0x92 && SSC_EV_streckenbefehl.Entkoppler == 0x5D) {
		printf("SSC Testfall 1 erfolgreich.\n");
	}
	else {
		printf("SSC Testfall 1 fehlgeschlagen.\n");
	}
	}
}

void run_ssctest_2(void) {
	//Modul initialisieren
	//initSSC();
	//zu vergleichenden Streckenbefehl in Shared Memory laden
	//EV_SSC_streckenbefehl.Lok = 0x25;
	//EV_SSC_streckenbefehl.Weiche = 0x92;
	//EV_SSC_streckenbefehl.Entkoppler = 0x5D;
	//SRB = 0x25;
	//workSSC();
}

void run_ssctest_3(void) {
	//initSSC();
}

void run_ssctest_4(void) {
	//initSSC();
}

void run_ssctest_5(void) {
	//initSSC();
}   