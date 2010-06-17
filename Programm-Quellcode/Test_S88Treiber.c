/*****************************************************************************
 *
 *        Dateiname:    Test_S88Treiber.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Jan-Christopher Icken
 *
 *
 *        Modul:        Testskript für das S88-Treiber-Modul
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
#include "S88Treiber.h"
#include "Test_S88Treiber.h"
#include <stdio.h>

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/

/* Prototypen fuer lokale Funktionen ****************************************/
void run_test_1(void);
void run_test_2(void);
void run_test_3(void);
/* Funktionsimplementierungen ***********************************************/
void work_s88_tests(void) {
	//Serielle Schnittstelle für Debug Ausgabe einrichten
	BD = 1; // Baudratengenerator einschalten
	SM0 = 0; // Mode 1  8Bit variable Baudrate
	SM1 = 1;
	SRELH = 0x03; // 9600 Baud
	SRELL = 0xDF;
	REN = 1; // seriellen Empfang einschalten
	TI = 1;
	run_test_1(); //Ersten Test durchführen
	run_test_2(); //Zweiten Test durchführen
	run_test_3(); //Dritten Test durchführen
	for(;;) {
	}
}

/*
*   Test_S88-Treiber_01_SensordatenNochNichtVerarbeitet
*/

void run_test_1(void)	   
{      
	initS88();
	S88_BV_sensordaten.Byte0 = 0x12;
	S88_BV_sensordaten.Byte1 = 0x34;
	workS88();
	if(S88_BV_sensordaten.Byte0 == 0x12 && S88_BV_sensordaten.Byte1 == 0x34) {
		printf("Test 1 S88-Treiber-Modul erfolgreich.\n");
	}
}

/*
*   Test_S88-Treiber_02_geaenderteSensorsignale
*/

void run_test_2(void) {
	initS88();
	//S88_Data = 1;
	workS88();
	if(S88_BV_sensordaten.Byte0 == 0xFF && S88_BV_sensordaten.Byte1 == 0xFF) {
		printf("Test 2 S88-Treiber-Modul erfolgreich.\n");
	}
}

/*
*   Test_S88-Treiber_03_gleicheSensorsignale
*/

void run_test_3(void) {
	initS88();
	S88_Data = 0;
	workS88();
		if(S88_BV_sensordaten.Byte0 == 0x00 && S88_BV_sensordaten.Byte1 == 0x00) {
		printf("Test 3 S88-Treiber-Modul erfolgreich.\n");
	}
}