/*****************************************************************************
 *
 *        Dateiname:    Fahrprogramm.c
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

/* Includes *****************************************************************/
#include "Fahrprogramm.h"
#include "Betriebsmittelverwaltung.h"


/* Definition globaler Konstanten *******************************************/
#define lok1 0x0	//Lokomotive 0 nur im Zugbetrieb
#define lok2 0x1	//Lokomotive 1 im Zug- und Rangierbetrieb
#define err  0xFF	//Error
#define null '\0'	//Null

//Fahrprogramm ArraySize
#define row 50
#define col 2

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/
typedef int Fahrprogramm[row][col]; 	//Definition Fahrprogrammarrays

typedef enum {
	gleisAbschnitt_1 = 0x1,
	gleisAbschnitt_2,
	gleisAbschnitt_3,
	gleisAbschnitt_4,
	gleisAbschnitt_5,
	gleisAbschnitt_6,
	gleisAbschnitt_7,
	gleisAbschnitt_8,
	gleisAbschnitt_9
} Gleisabschnitt;

typedef enum {
	fahreLok1 = 0x0,
	fahreLok2 = 0x1,
	kuppelLok2 = 0x3,
	abkuppelLok2 =0x5,
	haltLok2_10 = 0x7,
	haltLok2_20 = 0x17,
	haltLok2_30 = 0x27
} Fahrbefehl;
/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/
/*
 * Initialisiere Fahrprogramm_1
 * Fahrprogramm_1 enthält Fahrbefehle für die Lok#1 (nur Zugbetreieb)
 */
static Fahrprogramm fahrprogramm_1  = {
	{fahreLok1,gleisAbschnitt_4},
	{fahreLok1,gleisAbschnitt_5},
	{fahreLok1,gleisAbschnitt_6},
	{fahreLok1,gleisAbschnitt_1},
	{fahreLok1,gleisAbschnitt_7}
};
/*
 * Initialisiere Fahrprogramm_2
 * Fahrprogramm_2 enthält Fahrbefehle für die Lok#2 (Zug- und Rangierbetreieb)
 */
static Fahrprogramm fahrprogramm_2  = {
	{fahreLok2,gleisAbschnitt_1},
	{fahreLok2,gleisAbschnitt_7},
	{fahreLok2,gleisAbschnitt_4},
	{fahreLok2,gleisAbschnitt_3},
	{kuppelLok2,gleisAbschnitt_2},//Ankuppeln der 3 Wagons
	{fahreLok2,gleisAbschnitt_1},
	{fahreLok2,gleisAbschnitt_8},
	{fahreLok2,gleisAbschnitt_9},//Abkupeln der Wagons
	{fahreLok2,gleisAbschnitt_8},
	{fahreLok2,gleisAbschnitt_1},
	{fahreLok2,gleisAbschnitt_2},
	{haltLok2_20,gleisAbschnitt_2},//Warte 20 Sekunden
	{fahreLok2,gleisAbschnitt_1},
	{fahreLok2,gleisAbschnitt_8},
	{kuppelLok2,gleisAbschnitt_9},//Ankuppeln der drei Wagons
	{fahreLok2,gleisAbschnitt_8},
	{fahreLok2,gleisAbschnitt_1},
	{fahreLok2,gleisAbschnitt_2},
	{fahreLok2,gleisAbschnitt_3},
	{abkuppelLok2,gleisAbschnitt_2},//Abkupeln der Wagons
	{fahreLok2,gleisAbschnitt_3},
	{fahreLok2,gleisAbschnitt_4},
	{fahreLok2,gleisAbschnitt_5},
	{fahreLok2,gleisAbschnitt_6},
	{fahreLok2,gleisAbschnitt_1},
	{fahreLok2,gleisAbschnitt_8},
	{haltLok2_20,gleisAbschnitt_8}//Warte 20 Sekunden
};

/* Prototypen fuer lokale Funktionen ****************************************/

/* Funktionsimplementierungen ***********************************************/
/*
 * void initFP() initialisiert die Fahraufgaben
 * Diese Funktion muss einmal zu Beginn des gesamten Programmablaufs
 * aufgerufen werden
 */
void initFP()
{

}
//Ende initFP

void workFP()
{

}

//Schnittstelle
//Übergabe nächster Fahranweisung an Leitzentrale
Fahranweisung get_command(byte lok)
{
	//Counter für Fahrprogramm f1 und f2
	static unsigned int f1_Index = 0;
	static unsigned int f2_Index = 0;
	Fahranweisung Anweisung;

	//Anweisung für Lok 1
	if (lok == lok1) {

		if (fahrprogramm_1[f1_Index][1] == null) {
			//Es gibt keinen Gleischabschnitt = null, Array Ende
			f1_Index = 0;

		}

		//Zuweisung
		Anweisung.fahrbefehl = fahrprogramm_1[f1_Index][0];
		Anweisung.gleisabschnittNr = fahrprogramm_1[f1_Index][1];
		//Anweisung.gleisabschnittNr = fahrprogramm_1[f1_Index][1];

		f1_Index ++;//Index inkrementieren
		//Rückgabe aktuelle Anweisung = (Fahrbefehl_n,Gleisabschnit_n)
		return Anweisung;

	}
	//Anweisung für Lok 2
	else if (lok == lok2) {

		if (fahrprogramm_2[f2_Index][1] == null) {
			//Es gibt keinen Gleischabschnitt = null, Array Ende
			f2_Index = 0;
		}

		//Zuweisung
		Anweisung.fahrbefehl = fahrprogramm_2[f2_Index][0];
		Anweisung.gleisabschnittNr = fahrprogramm_2[f2_Index][1];

		f2_Index ++;//Index inkrementieren
		//Rückgabe aktuelle Anweisung = (Fahrbefehl_n,Gleisabschnit_n)
		return Anweisung;
	}
	//Fehler Anweisung bei falscher bzw. nicht vorhandener Lok
	else {
		//Error Zuweisung
		Anweisung.fahrbefehl = err;
		Anweisung.gleisabschnittNr = err;
		//Rückgabe aktuelle Anweisung = (Fahrbefehl_err,Gleisabschnit_err)
		return Anweisung;
	}
}//Ende get_Command
