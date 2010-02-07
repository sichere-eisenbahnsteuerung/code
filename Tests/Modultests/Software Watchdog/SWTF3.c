/*****************************************************************************
 *
 *        Dateiname:    SWTF3.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Felix Theodor Blueml
 *
 *
 *        Testobjekt:   Software Watchdog, Version 1.6
 *
 *        Beschreibung:
 *        Diese Datei ist Teil des Testfalls 3 fuer den Programmcode zum Modul
 *        Software Watchdog in der Version 1.6.
 *        Testname: Modul-Rueckmeldung, positiv
 *        Szenario: Abschalten des SW und Modul-Rueckmeldung beim SW.
 *        Testspezifikationsversion: 0.1
 *
 ****************************************************************************/

/* Includes *****************************************************************/
#include <reg515c.h>
#include "SWTF3.h"
#include "SoftwareWatchdog.h"
#include "SoftwareWatchdogStop.h"
#include "SoftwareWatchdogHelloModul.h"
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
	int i;
	EAL = 1; //enable all interrupts
	
	/* Testscenario Testscenario Abschalten des SW und Modul-Rueckmeldung
       beim SW */
 	initNOTAUS();
 	initSW();
	stopSW();
	helloModul(0, 0);
	helloModul(0, 2);
	helloModul(2, 5);
	helloModul(0, 1);
	helloModul(1, 1);
	helloModul(2, 1);
	helloModul(3, 1);
	helloModul(4, 1);
	helloModul(5, 1);
	helloModul(6, 1);
	helloModul(1, 7);
	helloModul(3, 255);
	helloModul(3, 1);

	// SW sollte jetzt ueberhaupt nicht zuschlagen!
	/* ENDE Testscenario Abschalten des SW und Modul-Rueckmeldung beim SW */

	for(;;)	//Endlosschleife
	{
	}
}
