#ifndef Fahrprogramm_H
#define Fahrprogramm_H

/*****************************************************************************
 *
 * Dateiname:    Fahrprogramm.h
 *
 * Projekt:      Sichere Eisenbahnsteuerung
 *
 * Autor:        Thomas Musialski
 *
 *
 * Modul:        Fahrprogramm, Version 0.2
 *
 * Beschreibung:
 * Das Fahrprogramm hat die Aufgabe die notwendige Funktionalität
 * für die im Pflichtenheft beschriebene Fahraufgabe bereitzustellen. 
 * Das Modul Fahraufgabe befindet sich in der Anwenderschicht und steht
 * in direkter Kommunikation mit der Leitzentrale.
 * Die notwendigen Anweisungen für eine Fahraufgabe werden in einem 
 * Ringpuffer hinterlegt und der Leitzentrale zur Verfügung gestellt.
 *
 ****************************************************************************/

#include "Betriebsmittelverwaltung.h"

/* Globale Makrodefinitionen ************************************************/

/* Globale Typdefinitionen **************************************************/

/*
 * Definition einer Fahranweisung. 
 *
 * Eine Fahranweisung besteht aus zwei Bytes, die angeben in welchen Gleisabschnitt der
 * angegebene Fahrbefehlt ausgeführt werden soll.
 */
typedef struct
{
    byte fahrbefehl;
    byte gleisabschnittNr; 
} Fahranweisung;

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

/* Deklaration von globalen Funktionen **************************************/

Fahranweisung get_command(byte Lok);
void initFP();

#endif
