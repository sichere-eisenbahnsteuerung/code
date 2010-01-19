/*****************************************************************************
 *
 *        Dateiname:    Betriebsmittelverwaltung.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Altan Gottwald
 *
 *
 *        Modul:        Betriebsmittelverwaltung, ver 0.3
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
/*
#include <Notaus.c>
#include <AuditingSystem>
#include <RS232Treiber>	      //
#include <S88>
#include <SSC>			//
#include <Ergebnisvalidierung>	  //
#include <Befehlsvalidierung>	    //
#include <Leitzentrale>		      //
#include <Fahrprogramm>
*/

/* Funktionsimplementierungen ***********************************************/
void main (void)
{
	EAL = 1; //enable all interrupts
	//alles initialisieren
/*	
	initNOTAUS();
	initSW ();
	initAS (void);
	hello ();
	initRS232 (void); 
	hello ();
	initS88 (void);
	hello (); 
	initSSC (void);
	hello ();
	initEV (void);
	hello ();
	initBV (void);
	hello ();
	initLZ (void);
	hello ();
	initFP (void);
	hello ();
*/
	
	for(;;)	//Endlosschleife der aufrufe, s. "C-Programmierung"
	{




		void workLZ (void);		//modulaufruf
		void hello (void);		//beim SW rückmelden
		void workRS232 (void);
		void hello (void);
		void workSSC (void);
		void hello (void);
		void workS88 (void);
		void hello (void);
		void workBV (void);
		void hello (void);
		void workRS232 (void);
		void hello (void);
		void workSSC (void);
		void hello (void);
		void workAS (void);
		void hello (void);
		void workEV (void);
		void hello (void);
		void workRS232 (void);
		void hello (void);
		void workSSC (void);
		void hello (void);
		void workAS (void);
		void hello (void);
	}
}