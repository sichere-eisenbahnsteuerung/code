/*****************************************************************************
 *
 *        Dateiname:    SSC.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Matthias Buß	
 *		  Datum:		03.01.2010
 *
 *        Modul:        SSC, 0.6
 *
 *        Beschreibung:
 *        Das SSC-Modul sorgt für die Kommunikation zwischen den
 *  	  redudanten Mikrocontrollern
 *
 ****************************************************************************/

/* Includes *****************************************************************/
#include <reg515C.h>
#include <stdio.h>

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/

/* Prototypen fuer lokale Funktionen ****************************************/

/* Funktionsimplementierungen ***********************************************/
boolean befehlAufGueltigkeitPruefen(boolean lesen) //bei gelesenem Streckenbefehl wird true übergeben, bei zu sendendem false
{

}

SSCinterrupt() interrupt 18
{
//überprüfen, welche Interrupt vorliegt und danach entsprechnete Funktion aufrufen
}

void kollisionVerarbeiten()
{
}

void datenLesen()
{
}

void work()
{

}
/*void main (void)
{

}*/
