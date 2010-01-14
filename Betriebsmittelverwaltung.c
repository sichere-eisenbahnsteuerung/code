/*****************************************************************************
 *
 *        Dateiname:    Betriebsmittelverwaltung.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Altan Gottwald, Vitali Voroth, Jan-Christopher Icken
 *
 *
 *        Modul:        Betriebsmittelverwaltung, ver 0.4
 *
 *        Beschreibung:
 *        Init aller Module
 *        Aufruf aller Module
 *        ________________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *
 ****************************************************************************/

/* Includes *****************************************************************/
#include <reg515c.h>
#include <stdio.h>
#include "Betriebsmittelverwaltung.h"

// #include "Notaus.h"
// #include "AuditingSystem.h"
// #include "RS232.h"
// #include "S88.h"
// #include "SSC.h"
// #include "Ergebnisvalidierung.h"
   #include "Befehlsvalidierung.h"
// #include "Leitzentrale.h"
// #include "Fahrprogramm.h"


/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

Streckenbefehl LZ_BV_streckenbefehl = {LEER,LEER,LEER,0};
Sensordaten BV_LZ_sensordaten = {LEER,LEER,0}; 
byte BV_LZ_bestaetigung = LEER; 
Sensordaten S88_BV_sensordaten = {LEER,LEER,0};
Streckenbefehl BV_EV_streckenbefehl = {LEER,LEER,LEER,0};
Streckenbefehl SCC_EV_streckenbefehl = {LEER,LEER,LEER,0};
Streckenbefehl EV_SSC_streckenbefehl = {LEER,LEER,LEER,0};
byte EV_SSC_failure = LEER;
byte SSC_EV_failure = LEER; 
byte SW_status_array[6] = {LEER,LEER,LEER,LEER,LEER,LEER}; 
Streckenbefehl EV_RS232_streckenbefehl = {LEER,LEER,LEER,0};
byte AS_msg_counter = LEER;

/**
   Interrupt 1 für SW
   Interrupt 4 für RS232
   Interrupt 18 für S88
**/

//EAL = 1; //enable all interrupts

//Pins
//P4.1 bis P4.4 für SSC reserviert
//P4.6 und P4.7 für CAN reserviert


/*sbit S88_PS = P1^6; 
sbit S88_RESET = P1^7;     
sbit S88_CLK = P3^2;
sbit S88_Data = P3^3;
sbit NOTAUS_PIN	= P3^4;
sbit RS232TREIBER_CTSPIN = P3^5;
sbit AS_PORT_I2C_SDA = P3^6;
sbit AS_PORT_I2C_SCL = P3^7;*/

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
 	initBV();
// 	hello();
// 	initLZ();
// 	hello();
// 	initFP();
// 	hello();


	for(;;)	//Endlosschleife der aufrufe, s. "C-Programmierung"
	{
// 		workLZ();		//modulaufruf
// 		hello();		//beim SW rückmelden
// 		workRS232();
// 		hello();
// 		workS88();
// 		hello();
// 		workSSC();
// 		hello();
 		workBV();
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