/*****************************************************************************
 *
 *        Dateiname:    SWTF1.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Felix Theodor Blueml
 *
 *
 *        Testobjekt:   Software Watchdog, Version 1.6
 *
 *        Beschreibung:
 *        Diese Datei ist Teil des Testfalls 1 fuer den Programmcode zum Modul
 *        Software Watchdog in der Version 1.6.
 *        Testname: Rueckmeldung, abschalten
 *        Szenario: Rueckmeldung beim SW und hinterher Abschalten des SW.
 *        Testspezifikationsversion: 0.1
 *
 ****************************************************************************/

/* Includes *****************************************************************/
#include <reg515c.h>
#include "SWTF1.h"
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
	int i; 
	EAL = 1; //enable all interrupts 	 
	/* Testszenario Rueckmeldung beim SW und hinterher Abschalten des SW */ 
 	initNOTAUS(); 
 	initSW(); 
	hello(); 
	for(i=0; i<1000; i++); // Warte ein wenig 
	hello(); 
	for(i=0; i<1000; i++); // Warte ein wenig 
	hello(); 
	for(i=0; i<1000; i++); // Warte ein wenig 
	hello(); 
	for(i=0; i<1000; i++); // Warte ein wenig 
	hello(); 
	for(i=0; i<1000; i++); // Warte ein wenig 
	hello(); 
	for(i=0; i<1000; i++); // Warte ein wenig 
	hello(); 
	for(i=0; i<1000; i++); // Warte ein wenig 
	hello(); 
	for(i=0; i<1000; i++); // Warte ein wenig 
	hello(); 
	for(i=0; i<1000; i++); // Warte ein wenig 
	hello(); 
	for(i=0; i<1000; i++); // Warte ein wenig 
	hello(); 
	for(i=0; i<1000; i++); // Warte ein wenig 
	hello(); 
	for(i=0; i<1000; i++); // Warte ein wenig 
	hello(); 
	for(i=0; i<1000; i++); // Warte ein wenig 
	stopSW(); 
	// SW sollte jetzt ueberhaupt nicht mehr zuschlagen! 
	/* ENDE Testszenario Rueckmeldung beim SW und hinterher Abschalten des SW */
	for(;;)	//Endlosschleife
	{ 
	} 
}
