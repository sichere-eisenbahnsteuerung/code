/*****************************************************************************
 *
 *        Dateiname:    SSC.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Matthias Bu�	
 *		  Datum:		03.01.2010
 *
 *        Modul:        SSC, 0.6
 *
 *        Beschreibung:
 *        Das SSC-Modul sorgt f�r die Kommunikation zwischen den
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
boolean befehlAufGueltigkeitPruefen(boolean lesen) //bei gelesenem Streckenbefehl wird true �bergeben, bei zu sendendem false
{

}

SSCinterrupt() interrupt 18
{
//�berpr�fen, welche Interrupt vorliegt und danach entsprechnete Funktion aufrufen
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
