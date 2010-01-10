/*****************************************************************************
 *
 *        Dateiname:    Befehlsvalidierung.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Vitali Voroth
 *
 *
 *        Modul:        <Modulname>, <Version des Moduldesigns>
 *
 *        Modul:        Befehlsvalidierung, 1.0
 *
 *        Beschreibung:	Prüft Streckenbefehle der Leitzentrale auf Gültigkeit
 *			Leitet Sensordaten vom S88-Treiber an die Leitzentrale
 *			Macht ein Not-Aus bei kritischem Zustand auf Strecke
 *
 ****************************************************************************/

/* Includes *****************************************************************/
#include "Betriebsmittelverwaltung.h"
#include "Befehlsvalidierung.h"
#include <stdio.h>

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/

/* Prototypen fuer lokale Funktionen ****************************************/

/* Funktionsimplementierungen ***********************************************/

byte b1, b2, b3;

void initBV(void)
{
	b1 = 10;
	b2 = 20;


}

void workBV(void)
{
	b3 = b1 + b2;
	b1++;
	b2++;
	printf("b3 = %d \n", (int)b3);
}