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
#include "Leitzentrale.h"
#include "Betriebsmittelverwaltung.h"

/* Definition globaler Konstanten *******************************************/
#define lok0 0x0	//Lokomotive 0 nur im Zugbetrieb
#define lok1 0x1	//Lokomotive 1 im Zug- und Rangierbetrieb
#define err 0xFF	//Error
#define null '\0'	//Null

//Fahrprogramm ArraySize
#define row 50
#define col 2

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/
typedef int Fahrprogramm[row][col]; 	//Definition Fahrprogrammarrays

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/
static Fahrprogramm Fahrprogramm_1 , Fahrprogramm_2; //Initialisiere Fahrprogramm(e)

/* Prototypen fuer lokale Funktionen ****************************************/

/* Funktionsimplementierungen ***********************************************/


/**********************************************************************/
//Schnittstelle
//Übergabe nächster Fahranweisung an Leitzentrale
Fahranweisung get_command(byte lok)
{
	//Counter für Fahrprogramm f1 und f2
	static unsigned int f1_Index = 0;
	static unsigned int f2_Index = 0;
	Fahranweisung Anweisung;

	//Anweisung für Lok 0
	if (lok == lok0) {

		if (Fahrprogramm_1[f1_Index][1] == null) {
			//Es gibt keinen Gleischabschnitt = null Array Ende
			f1_Index = 0;
		}

		//Zuweisung
		Anweisung.fahrbefehl = Fahrprogramm_1[f1_Index][0];
		Anweisung.gleisabschnittNr = Fahrprogramm_1[f1_Index][1];

		f1_Index ++;//Index inkrementieren
		//Rückgabe aktuelle Anweisung = (Fahrbefehl_n,Gleisabschnit_n)
		return Anweisung;
	}
	//Anweisung für Lok 1
	else if (lok == lok1) {

		if (Fahrprogramm_2[f2_Index][1] == null) {
			//Es gibt keinen Gleischabschnitt = null Array Ende
			f2_Index = 0;
		}

		//Zuweisung
		Anweisung.fahrbefehl = Fahrprogramm_2[f2_Index][0];
		Anweisung.gleisabschnittNr = Fahrprogramm_2[f2_Index][1];

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


void initFP_1()
{
	/*
	 * Fahrprogramm_1_Anweisung_n, FP_1_AW_n =(Fahrbefehl,Gleisabschnitt)
	 * Lok#1 == Lok0 immer im Zugbetrieb mit 3 Wagons
	 */
	static Fahranweisung FP_1_AW_1 = {0x0,0x4};
	static Fahranweisung FP_1_AW_2 = {0x0,0x5};
	static Fahranweisung FP_1_AW_3 = {0x0,0x6};
	static Fahranweisung FP_1_AW_4 = {0x0,0x1};
	static Fahranweisung FP_1_AW_5 = {0x0,0x7};

}//Ende initFP_1


void initFP_2()
{
	/*
	 * Fahrprogramm_2, FP_2_AW_n=(Fahrbefehl,Gleisabschnitt)
	 * Lok#2 == Lok1 kann Zugbetrieb und Rangierbetrieb mit max 3 Wagons
	 * */
	static Fahranweisung FP_2_AW_1 = {0x1,0x1};
	static Fahranweisung FP_2_AW_2 = {0x1,0x7};
	static Fahranweisung FP_2_AW_3 = {0x1,0x4};
	static Fahranweisung FP_2_AW_4 = {0x1,0x3};
	static Fahranweisung FP_2_AW_5 = {0x3,0x2};//Ankuppeln der 3 Wagons
	static Fahranweisung FP_2_AW_6 = {0x1,0x1};
	static Fahranweisung FP_2_AW_7 = {0x1,0x8};
	static Fahranweisung FP_2_AW_8 = {0x5,0x9};//Abkupeln der Wagons
	static Fahranweisung FP_2_AW_9 = {0x1,0x8};
	static Fahranweisung FP_2_AW_10= {0x1,0x1};
	static Fahranweisung FP_2_AW_11= {0x1,0x2};
	static Fahranweisung FP_2_AW_12= {0x17,0x2};//Warte 20 Sekunden
	static Fahranweisung FP_2_AW_13= {0x1,0x1};
	static Fahranweisung FP_2_AW_14= {0x1,0x8};
	static Fahranweisung FP_2_AW_15= {0x3,0x9};//Ankuppeln der drei Wagons
	static Fahranweisung FP_2_AW_16= {0x1,0x8};
	static Fahranweisung FP_2_AW_17= {0x1,0x1};
	static Fahranweisung FP_2_AW_18= {0x1,0x2};
	static Fahranweisung FP_2_AW_19= {0x5,0x3};//Abkupeln der Wagons
	static Fahranweisung FP_2_AW_20= {0x1,0x3};
	static Fahranweisung FP_2_AW_21= {0x1,0x4};
	static Fahranweisung FP_2_AW_22= {0x1,0x7};
	static Fahranweisung FP_2_AW_23= {0x1,0x1};
	static Fahranweisung FP_2_AW_24= {0x1,0x8};
	static Fahranweisung FP_2_AW_25= {0x17,0x8};//Warte 20 Sekunden
}//Ende initFP_2
 

/*
 * void initFP() initialisiert die Fahraufgaben
 * Diese Funktion muss einmal zu Beginn des gesamten Programmablaufs
 * aufgerufen werden
 */
void initFP()
{
	/*
	 * Initialisiere Fahrprogramm_1
	 * Fahrprogramm_1 enthält Fahrbefehle für die Lok#1 (nur Zugbetreieb)
	 */
	void initFP_1();
	/*
	 * Initialisiere Fahrprogramm_2
	 * Fahrprogramm_2 enthält Fahrbefehle für die Lok#2 (Zug- und Rangierbetreieb)
	 */
	void initFP_2();

}
//Ende initFP

void workFP();

