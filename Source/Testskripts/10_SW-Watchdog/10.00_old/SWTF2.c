/*****************************************************************************
 *
 *        Dateiname:    SWTF2.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Felix Theodor Blueml
 *
 *
 *        Testobjekt:   Software Watchdog, Version 1.6
 *
 *        Beschreibung:
 *        Diese Datei ist Teil des Testfalls 2 fuer den Programmcode zum Modul
 *        Software Watchdog in der Version 1.6.
 *        Testname: Modul-Rueckmeldung, negativ
 *        Szenario: Abschalten des SW und Modul-Rueckmeldung beim SW.
 *        Testspezifikationsversion: 0.1
 *
 ****************************************************************************/

/* Includes *****************************************************************/
#include <reg515c.h>
#include "SWTF2.h"
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
    
	/* Testscenario Abschalten des SW und Modul-Rueckmeldung beim SW*/
 	initNOTAUS();
 	initSW();
	stopSW();
	//helloModul(0, 255); // Fehler: 255 ist erste Stausmeldung

	//helloModul(7, 0); // Fehler: ungueltige Modul-ID

	//helloModul(255, 0); // Fehler: ungueltige Modul-ID

	//helloModul(0, 0);
	//helloModul(0, 0); // Fehler: 2 mal die selbe Statusmeldung eines Moduls hintereinander



	//helloModul(0, 0);
	//helloModul(1, 1);
	//helloModul(0, 0); // Fehler: 2 mal die selbe Statusmeldung eines Moduls hintereinander


	helloModul(0, 1);
	helloModul(0, 2);
	helloModul(1, 1);
	helloModul(1, 2);
	helloModul(2, 1);
	helloModul(2, 2);
	helloModul(3, 1);
	helloModul(3, 2);
	helloModul(4, 1);
	helloModul(4, 2);
	helloModul(5, 1);
	helloModul(5, 2);
	helloModul(6, 1);
	helloModul(6, 2);
	helloModul(0, 2); // Fehler: 2 mal die selbe Statusmeldung eines Moduls hintereinander


	// SW sollte jetzt zuschlagen!

	/* ENDE Testscenario Abschalten des SW und Modul-Rueckmeldung beim SW*/

	for(;;)	//Endlosschleife
	{
	}
}
