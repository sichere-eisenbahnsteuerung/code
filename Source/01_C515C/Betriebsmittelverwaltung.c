/*****************************************************************************
 *
 *        Dateiname:    Betriebsmittelverwaltung.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Altan Gottwald, Vitali Voroth, Jan-Christopher Icken
 *
 *
 *        Modul:        Betriebsmittelverwaltung
 *
 *        Beschreibung:
 *        Init aller Module
 *        Aufruf aller Module
 *        _______________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *
 ****************************************************************************/

/* Includes *****************************************************************/
#include "Betriebsmittelverwaltung.h"

#include "SoftwareWatchdog.h"
#include "Notaus.h"
#include "AuditingSystem.h"
#include "RS232Treiber.h"
#include "S88Treiber.h"
#include "SSCTreiber.h"
#include "Ergebnisvalidierung.h"
#include "Befehlsvalidierung.h"
#include "Leitzentrale.h"
#include "Fahrprogramm.h"

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

Streckenbefehl LZ_BV_streckenbefehl = {LEER,LEER,LEER,0};
Streckenbefehl BV_EV_streckenbefehl = {LEER,LEER,LEER,0};
Streckenbefehl SSC_EV_streckenbefehl = {LEER,LEER,LEER,0};
Streckenbefehl EV_SSC_streckenbefehl = {LEER,LEER,LEER,0};
Streckenbefehl EV_RS232_streckenbefehl = {LEER,LEER,LEER,0};
Streckenbefehl RS232_EV_streckenbefehl = {LEER,LEER,LEER,0};

Sensordaten BV_LZ_sensordaten = {LEER,LEER,0}; 	
Sensordaten S88_BV_sensordaten = {LEER,LEER,0};

byte BV_LZ_bestaetigung = LEER; 
byte EV_SSC_failure = LEER;
byte SSC_EV_failure = LEER; 


/**
   Interrupt 1 für SW
   Interrupt 4 für RS232
   Interrupt 18 für S88
**/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/


/* Funktionsimplementierungen ***********************************************/
void main (void)
{
	EAL = 1; //enable all interrupts
	
	initAll();
	work();
}

void initAll(void)
{
	//alles initialisieren
	initNOTAUS();
 	initSW();
	initAS();
 	hello();
 	initRS232(); 
 	hello();
 	initS88();
 	hello(); 
 	initSSC();
 	hello();
 	initEV();
 	hello();
	initBV();
 	hello();
 	initLZ();
 	hello();
 	initFP();
 	hello();
}

void work(void)
{
  	for(;;)	//Endlosschleife der aufrufe, s. "C-Programmierung"
	{
		workLZ();		//modulaufruf
 		hello();		//beim SW rueckmelden
 		workRS232();
 		hello();
 		workS88();
 		hello();
 		workSSC();
 		hello();
 		workBV();
 		hello();
 		workAS();
 		hello();
 		workRS232();
 		hello();
 		workSSC();
 		hello();
 		workRS232();
 		hello();
 		workEV();
		hello();
 		workSSC();
 		hello();
		workAS();
 		hello();
	}
}