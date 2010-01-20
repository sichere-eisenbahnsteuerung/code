#ifndef Fahraufgabe_H
#define Fahraufgabe_H
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

#include "Leitzentrale.h"
#include "Betriebsmittelverwaltung.h"
/* Globale Makrodefinitionen ************************************************/

/* Globale Typdefinitionen **************************************************/

/* Eine Fahranweisung besteht aus 2 Byte. Der Typ Gleisabschnitt wird in 
 * Befehlsvalidierung.h deklariert und ist ein Byte gro§.
 * Der Fahrbefehl enthaelt den eigentlichen Fahrbefehl und die Lok fuer die
 * er gilt.
 */
typedef struct Fahranweisung_struct {
	byte fahrbefehl; /*
	 * Description      : Der Fahrbefehl, enthlt zustzlich als 
	 *		      letztes Bit die Lok, fr die er gilt.
	 *
	 * Access Rights    : r (Leitzentrale) w (Fahrprogramm)
	 *
	 * Values           : Kodierung siehe Software-Design
	 */  
	byte gleisabschnittNr; /*
	 * Description      : Die Nummer des Gleisabschnitts, fr den der 
	 *		      Fahrbefehl gilt.
	 *
	 * Access Rights    : r (Leitzentrale) w (Fahrprogramm)
	 *
	 * Values           : (1 - 9)
	 *                    Kodierung siehe Befehlsvalidierung-Moduldesign
	 */  
} Fahranweisung;

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

/* Deklaration von globalen Funktionen **************************************/
Fahranweisung get_command(byte Lok);
void initFP();
void workFP();

#endif /* Fahraufgabe.h_H */
