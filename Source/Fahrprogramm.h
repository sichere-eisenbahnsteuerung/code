#ifndef Fahrprogramm_H
#define Fahrprogramm_H
/*****************************************************************************
 *
 *        Dateiname:    Fahrprogramm.h
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Thomas Musialski
 *
 *
 *        Modul:        Fahrprogramm, Version 0.2
 *
 *        Beschreibung:
 *        Das Fahrprogramm hat die Aufgabe die notwendige Funktionalität
 * 	  für die im Pflichtenheft beschriebene Fahraufgabe bereitzustellen. 
 * 	  Das Modul Fahraufgabe befindet sich in der Anwenderschicht und steht
 * 	  in direkter Kommunikation mit der Leitzentrale.
 * 	  Die notwendigen Anweisungen für eine Fahraufgabe werden in einem 
 * 	  Ringpuffer hinterlegt und der Leitzentrale zur Verfügung gestellt.
 *
 ****************************************************************************/

#include "Betriebsmittelverwaltung.h"
/* Globale Makrodefinitionen ************************************************/

/* Globale Typdefinitionen **************************************************/
typedef struct 
{ 
	byte fahrbefehl; 
	byte gleisabschnittNr; 
} Fahranweisung;

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

/* Deklaration von globalen Funktionen **************************************/
/*
 * Schnittstelle get_command,
 * Aufruf durch Leitzentralle.
 * Übergabewert gültige Fahranweisung = (fahrbefehl,gleisabschnittNr)
 */
Fahranweisung get_command(byte Lok);

void initFP();
/*void workFP();*/

#endif /* Fahraufgabe.h_H */
