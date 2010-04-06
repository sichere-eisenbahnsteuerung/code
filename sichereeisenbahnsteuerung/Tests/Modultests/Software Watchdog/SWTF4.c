/*****************************************************************************
 *
 *        Dateiname:    SWTF4.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Felix Theodor Blueml
 *
 *
 *        Testobjekt:   Software Watchdog, Version 1.6
 *
 *        Beschreibung:
 *        Diese Datei ist Teil des Testfalls 4 fuer den Programmcode zum Modul
 *        Software Watchdog in der Version 1.6.
 *        Testname: Zeitfenster
 *        Szenario: Pruefen der Groesse des SW-Zeitfensters.
 *        Testspezifikationsversion: 0.1
 *
 ****************************************************************************/

/* Includes *****************************************************************/
#include <reg515c.h>
#include "SWTF4.h"
#include "SoftwareWatchdog.h"
#include "SoftwareWatchdogStop.h"
#include "Notaus.h"

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/

/* Prototypen fuer lokale Funktionen ****************************************/

/* Funktionsimplementierungen ***********************************************/

void main (void)
{
	EAL = 1; //enable all interrupts
	
	/* Testscenario Pruefen der Groesse des SW-Zeitfensters */
	NOTAUS_PIN = FALSE;

 	initSW();
	hello();

	// SW-Test: START DER ZEITMESSUNG
	NOTAUS_PIN = TRUE;
	// SW-Test: ENDE DER ZEITMESSUNG
	// (durch ausloesen des Not-Aus ueber den Interrupt timer0overflow)
	// Gemessenes Zeitfenster per Logiganalysator "Agilent LogicWave" liegt
	// bei 20 ms.

	/* ENDE Testscenario Pruefen der Groesse des SW-Zeitfensters */

	for(;;)	//Endlosschleife
	{
	}
}
