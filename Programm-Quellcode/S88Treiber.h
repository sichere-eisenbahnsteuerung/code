#ifndef S88_H
#define S88_H

/*****************************************************************************
 *
 *        Dateiname:    S88.h
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Icken, Jan-Christopher
 *
 *
 *        Modul:        s88-Treiber, 0.3
 *
 *        Beschreibung:
 *        Dieses Modul liest die Sensordaten ueber den S88-Bus ein und
 *        speichert diese im Shared Memory
 *        ________________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *
 ****************************************************************************/
#include "Betriebsmittelverwaltung.h"
#include "SoftwareWatchdogHelloModul.h"

/* Globale Makrodefinitionen ************************************************/

/* Globale Typdefinitionen **************************************************/

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

/* Deklaration von globalen Funktionen **************************************/
void initS88(void); 
void workS88(void); 

#endif /* s88_H */