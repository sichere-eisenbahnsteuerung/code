#ifndef Fahrprogramm_H
#define Fahrprogramm_H

/**
 * @file    Fahrprogramm.c
 *
 * @author  Thomas Musialski
 *
 * @brief   Bereitstellung der für Fahraufgabe nötigen Funktionen
 *
 *          Das Fahrprogramm hat die Aufgabe die notwendige Funktionalitaet
 * 	        fuer die im Pflichtenheft beschriebene Fahraufgabe bereitzustellen.
 * 	        Das Modul Fahraufgabe befindet sich in der Anwenderschicht und steht
 * 	        in direkter Kommunikation mit der Leitzentrale.
 * 	        Die notwendigen Anweisungen fuer eine Fahraufgabe werden in einem
 * 	        Ringpuffer hinterlegt und der Leitzentrale zur Verfuegung gestellt.
 */

#include "Betriebsmittelverwaltung.h"

// Globale Makrodefinitionen

// Globale Typdefinitionen

/*
 * Definition einer Fahranweisung. 
 *
 * Eine Fahranweisung besteht aus zwei Bytes, die angeben in welchen Gleisabschnitt der
 * angegebene Fahrbefehlt ausgefuehrt werden soll.
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
