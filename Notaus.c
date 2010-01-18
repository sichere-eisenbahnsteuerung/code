/*****************************************************************************
 *
 *        Dateiname:    Notaus.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Icken, Jan-Christopher
 *
 *
 *        Modul:        NotAus-Treiber, 0.1
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
}

void workNOTAUS(void)
{
}

void emergency_off(void)
{
	NOTAUS_PIN = TRUE; //Notausmachen
	//SoftwareWatchdog anhalten
	//Endlosschleife um keine weiteren Aktionen zuzulassen
	for(;;)
	{
	}
}


