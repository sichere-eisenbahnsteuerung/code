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
#define row 50	//Zeilen
#define col 2	//Spalten

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/
typedef int Fahrprogramm[row][col]; 	//Definition Fahrprogrammarrays

/*
 * typdef enum Gleisabschnitt, 
 * listet nummerisch alle verwendeten Geisabschnitte auf.
 * Das verwendete Gleislayout, siehe 'Modul-Design Befehlsvaledierung',
 * besteht aus 9 Geisabschnitten. Eine Gleisabschnitt hat die Größe 1 Byte.
 * 
*/
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
/*
 * typdef enum Fahrbefehl, 
 * listet alle verwendeten Fahrbefehle auf.
 * Für Fahraufgabe 1, siehe 'Pflichtenheft', 
 * werden 7 Fahrbefehle, mit ihrer  benötigt, siehe 'Modul-Design Fahrprogram'.
 *  
*/
typedef enum {
	fahreLok1 = 0x0,
	fahreLok2 = 0x1,
	kuppelLok2 = 0x3,
	abkuppelLok2 =0x5,
	haltLok2_10 = 0x7,
	haltLok2_20 = 0x17,
	haltLok2_30 = 0x27
}Fahrbefehl;
/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/

/*
 * Definiere Fahrprogramm_1
 * Fahrprogramm_1 enthält Fahrbefehle für die Lok#1 (nur Zugbetreieb).
 * Fahraufgabe, siehe 'Pflichtenheft'.
 */
static Fahrprogramm fahrprogramm_1  = {
	/*
	 *Eine Fahrprogramm enthält 2 Byte Fahranweisungen,
	 * Fahrbefehl und Gleisabschnit.
	 */
	{fahreLok1,gleisAbschnitt_4},
	{fahreLok1,gleisAbschnitt_5},
	{fahreLok1,gleisAbschnitt_6},
	{fahreLok1,gleisAbschnitt_1},
	{fahreLok1,gleisAbschnitt_7}
};//Ende Fahrprogramm_1
/*
 * Initialisiere Fahrprogramm_2
 * Fahrprogramm_2 enthält Fahrbefehle für die Lok#2 (Zug- und Rangierbetreieb).
 * Fahraufgabe, siehe 'Pflichtenheft'
 */
static Fahrprogramm fahrprogramm_2  = {
	/*
	 *Eine Fahrprogramm enthält 2 Byte Fahranweisungen,
	 * Fahrbefehl und Gleisabschnit.
	 */

	{fahreLok2,gleisAbschnitt_1},
	{fahreLok2,gleisAbschnitt_7},
	{fahreLok2,gleisAbschnitt_4},
	{fahreLok2,gleisAbschnitt_3},
	{kuppelLok2,gleisAbschnitt_2},//Ankuppeln der 3 Wagons
	{fahreLok2,gleisAbschnitt_1},
	{fahreLok2,gleisAbschnitt_8},
	{abkuppelLok2,gleisAbschnitt_9},//Abkupeln der Wagons
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
};//Ende Fahrprogramm_2

/* Prototypen fuer lokale Funktionen ****************************************/

/* Funktionsimplementierungen ***********************************************/

void initFP()
{

}//Ende initFP

/*void workFP()
{

}//Ende workFP */

/*
 * Schnittstelle get_command(),
 * wird vond er Leitzentrale aufgerufen und
 * übergibt die Aktuelle Fahranweisung einer Lok.
 */ 
Fahranweisung get_command(byte lok)
{
	//Counter für Fahrprogramm f1 und f2
	static unsigned int f1_Index = 0; 
	static unsigned int f2_Index = 0;
	Fahranweisung anweisung;

	/*
	 * if-Anweisung,
	 * überprüft den Wert von 'lok' und übergibt die jeweils gültige
	 * Fahranweisung an die Leitzentrale. Die Fahraufgabe beinhaltet 2 Loks,
	 * siehe 'Pflichtenheft'.
	 * 
	 */
	if (lok == lok1) { //true, fals lok == 0

		if (fahrprogramm_1[f1_Index][1] == null) { // Prüfung, fahrprogramm_1 Array Ende
			f1_Index = 0; //Rücksetzen des Counter f1_Index
		}

		//Aufruf von gültiger Fahranweisung
		anweisung.fahrbefehl = fahrprogramm_1[f1_Index][0];
		anweisung.gleisabschnittNr = fahrprogramm_1[f1_Index][1];
		
		f1_Index ++;//Index inkrementieren
		//Rückgabe aktuelle Fahranweisung, anweisung = (fahrbefehl_n,gleisabschnit_n)
		return anweisung;

	}
	else if (lok == lok2) { //true, fals lok == 1

		if (fahrprogramm_2[f2_Index][1] == null) { // Prüfung, fahrprogramm_2 Array Ende
			f2_Index = 0; //Rücksetzen des Counter f2_Index
		}

		//Aufruf von gültiger Fahranweisung
		anweisung.fahrbefehl = fahrprogramm_2[f2_Index][0];
		anweisung.gleisabschnittNr = fahrprogramm_2[f2_Index][1];

		f2_Index ++;//Index inkrementieren
		//Rückgabe aktuelle Fahranweisung, anweisung = (fahrbefehl_n,gleisabschnit_n)
		return anweisung;
	}
	//Fehler Anweisung bei falscher bzw. nicht vorhandener Lok
	else {
		//Error Zuweisung
		anweisung.fahrbefehl = err;
		anweisung.gleisabschnittNr = err;
		//Rückgabe aktuelle Anweisung = (Fahrbefehl_err,Gleisabschnit_err)
		return anweisung;
	}
}//Ende get_command

