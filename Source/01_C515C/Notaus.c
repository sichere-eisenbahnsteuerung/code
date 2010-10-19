/*****************************************************************************
 *
 *        Dateiname:    Notaus.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Icken, Jan-Christopher
 *
 *
 *        Modul:        NotAus-Treiber, 2.0
 *
 *        Beschreibung:
 *        Dieses Modul stellt den Zugriff auf das extern angeschlossene
 *        Relais zum ausschalten der Strecke bereit.
 *        ________________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *
 ****************************************************************************/

/* Includes *****************************************************************/

/* Includes *****************************************************************/
#include "Betriebsmittelverwaltung.h"
#include "Notaus.h"
#include "SoftwareWatchdogStop.h"
#include "AuditingSystemReportAllMsg.h"

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/

/* Prototypen fuer lokale Funktionen ****************************************/

/* Funktionsimplementierungen ***********************************************/

void initNOTAUS(void)
{
	NOTAUS_PIN = TRUE; //Relais setzen und Strecke mit Strom versorgen
}

/*void workNOTAUS(void)
{
} */

void emergency_off(void)
{
	// Relais zuruecksetzen und Strom von der Strecke nehmen
	NOTAUS_PIN = FALSE;

	//SoftwareWatchdog anhalten
	stopSW();

	// Verbliebene Meldungen der Module versenden
	reportAllMsg();

	//Endlosschleife, bis Reset-Taste betätigt wird
	while(!Neustart)
	{
	}
			/*
	initAll();
	work();	  */
}


