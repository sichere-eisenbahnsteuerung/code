#ifndef Notaus_H
#define Notaus_H
/*****************************************************************************
 *
 *        Dateiname:    Notaus.h
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Icken, Jan-Christopher
 *
 *
 *        Modul:        Notaus-Treiber, 0.1
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




/* Globale Makrodefinitionen ************************************************/

/* Globale Typdefinitionen **************************************************/

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/ 

/* Deklaration von globalen Funktionen **************************************/
void initNOTAUS(void); 
void workNOTAUS(void);
void emergency_off(void);

#endif /* Notaus_H */