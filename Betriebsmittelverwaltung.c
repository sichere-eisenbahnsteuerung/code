/*****************************************************************************
 *
 *        Dateiname:    Betriebsmittelverwaltung.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Altan Gottwald, Vitali Voroth, Jan-Christopher Icken
 *
 *
 *        Modul:        Betriebsmittelverwaltung, ver 0.5
 *
 *        Beschreibung:
 *        Init aller Module
 *        Aufruf aller Module
 *        _enum Konstanten für SW Modul IDs_______________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *
 ****************************************************************************/

/* Includes *****************************************************************/
#include "Betriebsmittelverwaltung.h"

// #include "Notaus.h"
// #include "AuditingSystem.h"
// #include "RS232.h"
// #include "S88.h"
// #include "SSC.h"
// #include "Ergebnisvalidierung.h"
// #include "Befehlsvalidierung.h"
// #include "Leitzentrale.h"
// #include "Fahrprogramm.h"

/* Definition globaler Konstanten *******************************************/
/*
  Modul IDs für SW

  Leitzentrale - 0
  Befehlsvalidierung - 1
  Ergebnisvalidierung - 2
  S88-Treiber - 3
  SSC-Treiber - 4
  RS232-Treiber - 5
  Auditing System - 6
*/
enum module_id { LZ, BV, EV, S88, SSC, RS232, AS};

/* Definition globaler Variablen ********************************************/

Streckenbefehl LZ_BV_streckenbefehl = {LEER,LEER,LEER,0};
Sensordaten BV_LZ_sensordaten = {LEER,LEER,0}; 
byte BV_LZ_bestaetigung = LEER; 
Sensordaten S88_BV_sensordaten = {LEER,LEER,0};
Streckenbefehl BV_EV_streckenbefehl = {LEER,LEER,LEER,0};
Streckenbefehl SSC_EV_streckenbefehl = {LEER,LEER,LEER,0};
Streckenbefehl EV_SSC_streckenbefehl = {LEER,LEER,LEER,0};
byte EV_SSC_failure = LEER;
byte SSC_EV_failure = LEER; 
byte SW_status_array[6] = {LEER,LEER,LEER,LEER,LEER,LEER}; 
Streckenbefehl EV_RS232_streckenbefehl = {LEER,LEER,LEER,0};
byte AS_msg_counter = LEER;

/**
   Interrupt 1 f¸r SW
   Interrupt 4 f¸r RS232
   Interrupt 18 f¸r S88
**/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/

/* Prototypen fuer lokale Funktionen ****************************************/

/* Funktionsimplementierungen ***********************************************/
void main (void)
{
	EAL = 1; //enable all interrupts
	
//alles initialisieren
	
// 	initNOTAUS();
// 	initSW();
// 	initAS();
// 	hello();
// 	initRS232(); 
// 	hello();
// 	initS88();
// 	hello(); 
// 	initSSC();
// 	hello();
// 	initEV();
// 	hello();
// 	initBV();
// 	hello();
// 	initLZ();
// 	hello();
// 	initFP();
// 	hello(); 

	for(;;)	//Endlosschleife der aufrufe, s. "C-Programmierung"
	{
// 		workLZ();		//modulaufruf
// 		hello();		//beim SW r¸ckmelden
// 		workRS232();
// 		hello();
// 		workS88();
// 		hello();
// 		workSSC();
// 		hello();
// 		workBV();
// 		hello();
// 		workAS();
// 		hello();
// 		workRS232();
// 		hello();
// 		workSSC();
// 		hello();
// 		workRS232();
// 		hello();
// 		workEV();
// 		hello();
// 		workSSC();
// 		hello();
// 		workAS();
// 		hello();
	}
}