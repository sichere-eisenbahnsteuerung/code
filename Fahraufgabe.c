/*****************************************************************************
 *
 *        Dateiname:    Fahrprogramm.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Thomas Musialski
 *
 *
 *        Modul:        Leitzentrale, Version 0.1
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
#include <stdio.h>
/* Definition globaler Konstanten *******************************************/
#define Lok0 0x0	//Lokomotive 0 nur Zugbetrieb
#define Lok1 0x1	//Lokomotive 1 Zug- und Rangierbetrieb
#define err 0xFF	//Error
#define null '\0'	//Null

//Fahrprogramm ArraySize
#define row 10		///Zeilenlänge für Fahrprogramm
#define col 3		//Spaltenlänge für Fahrprogramm

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/
typedef unsigned char byte;
//Definition Fahrprogrammarrays
typedef int Fahrprogramm[col][row];
//Definition struct Command
typedef struct Command {
	byte Fahrbefehl;
	byte Zug;
	byte Gleisabschnitt;
}Command;

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/
static Fahrprogramm Fahrprogramm_1 , Fahrprogramm_2; 
/* Prototypen fuer lokale Funktionen ****************************************/

/* Funktionsimplementierungen ***********************************************/


/**********************************************************************/
//Schnittstelle
//Übergabe nächster Fahranweisung an Leitzentrale
Command get_Command(byte Lok)
{
	//Counter für Fahrprogramm_1 und _2
	static unsigned int f1index = 0;
	static unsigned int f2index = 0;
	Command Anweisung; //Übergabe var

	//Anweisung für Lok 0
	if (Lok == Lok0) {
		
		if (Fahrprogramm_1[f1index][2] == null){
		//Es gibt keinen Gleischabschnitt = null Array Ende
		f1index = 0;
		}
		
		//Zuweisung
		Anweisung.Fahrbefehl = Fahrprogramm_1[f1index][0];
		Anweisung.Zug = Fahrprogramm_1[f1index][1];
		Anweisung.Gleisabschnitt = Fahrprogramm_1[f1index][2];

		f1index ++;//Index inkrementieren
		//Rückgabe aktuelle Anweisung = (Fahrbefehl_n,Zug0,Gleisabschnit_n)
		return Anweisung;
	}
	//Anweisung für Lok 1
	else if (Lok == Lok1) {
		
		if (Fahrprogramm_2[f2index][2] == null){
		//Es gibt keinen Gleischabschnitt = null Array Ende
		printf("TEST");
		f2index = 0;	
		} 

		//Zuweisung
		Anweisung.Fahrbefehl = Fahrprogramm_2[f2index][0];
		Anweisung.Zug = Fahrprogramm_2[f2index][1];
		Anweisung.Gleisabschnitt = Fahrprogramm_2[f2index][2];

		f2index ++;//Index inkrementieren
		//Rückgabe aktuelle Anweisung = (Fahrbefehl_n,Zug1,Gleisabschnit_n)
		return Anweisung;
	}
	//Fehler Anweisung bei falscher bzw. nicht vorhandener Lok
	else {
		//Error Zuweisung
		Anweisung.Fahrbefehl = err;
		Anweisung.Zug = err;
		Anweisung.Zug = err;
		//Rückgabe aktuelle Anweisung = (Fahrbefehl_err,Zug_err,Gleisabschnit_err)
		return Anweisung;
	}
}//Ende get_Command

/***************************
 ******main zum Testen!!****
 ***************************/
int main(int argc, char **argv)
{
	//Fahrprogramm_1_Anweisung_n, FP_1_AW_n =(Fahrbefehl,Lok0,Gleisabschnitt)
	//Lok#1 == Lok0 immer Zugbetrieb mit 3 Wagons
	static Command FP_1_AW_1 = {0x0,Lok0,0x1};
	static Command FP_1_AW_2 = {0x0,Lok0,0x2};
	static Command FP_1_AW_3 = {0x0,Lok0,0x3};
	static Command FP_1_AW_4 = {0x0,Lok0,0x1};
	static Command FP_1_AW_5 = {0x0,Lok0,0x7};

	//Fahrprogramm_2, FP_2_AW1=( Fahrbefehl , Lok1 , Gleisabschnitt )
	//Lok#2 == Lok1 kann Zugbetrieb und Rangierbetrieb mit max 3 Wagons
	static Command FP_2_AW_1 = {0x0,Lok1,0x1};
	static Command FP_2_AW_2 = {0x0,Lok1,0x7};
	static Command FP_2_AW_3 = {0x0,Lok1,0x4};
	static Command FP_2_AW_4 = {0x0,Lok1,0x3};
	static Command FP_2_AW_5 = {0x1,Lok1,0x3};//Ankuppeln
	/*TODO*/
	//Definition Reihenfolge der Wagons


	//Schreibe Anweisungen FP_1_AW_n in Fahrgrogramm_1
	Fahrprogramm_1[0][0]=FP_1_AW_1.Fahrbefehl;
	Fahrprogramm_1[0][1]=FP_1_AW_1.Zug;
	Fahrprogramm_1[0][2]=FP_1_AW_1.Gleisabschnitt;
	
	Fahrprogramm_1[1][0]=FP_1_AW_2.Fahrbefehl;
	Fahrprogramm_1[1][1]=FP_1_AW_2.Zug;
	Fahrprogramm_1[1][2]=FP_1_AW_2.Gleisabschnitt;
	
	Fahrprogramm_1[2][0]=FP_1_AW_3.Fahrbefehl;
	Fahrprogramm_1[2][1]=FP_1_AW_3.Zug;
	Fahrprogramm_1[2][2]=FP_1_AW_3.Gleisabschnitt;
	
	Fahrprogramm_1[3][0]=FP_1_AW_4.Fahrbefehl;
	Fahrprogramm_1[3][1]=FP_1_AW_4.Zug;
	Fahrprogramm_1[3][2]=FP_1_AW_4.Gleisabschnitt;
	
	Fahrprogramm_1[4][0]=FP_1_AW_5.Fahrbefehl;
	Fahrprogramm_1[4][1]=FP_1_AW_5.Zug;
	Fahrprogramm_1[4][2]=FP_1_AW_5.Gleisabschnitt;

	/*TODO*/
	//Schreibe Anweisungen FP_1_AW_n in Fahrgrogramm_2
	Fahrprogramm_2[0][0]=FP_2_AW_1.Fahrbefehl;
	Fahrprogramm_2[0][1]=FP_2_AW_1.Zug;
	Fahrprogramm_2[0][2]=FP_2_AW_1.Gleisabschnitt;
	
	Fahrprogramm_2[1][0]=FP_2_AW_2.Fahrbefehl;
	Fahrprogramm_2[1][1]=FP_2_AW_2.Zug;
	Fahrprogramm_2[1][2]=FP_2_AW_2.Gleisabschnitt;
	
	Fahrprogramm_2[2][0]=FP_2_AW_3.Fahrbefehl;
	Fahrprogramm_2[2][1]=FP_2_AW_3.Zug;
	Fahrprogramm_2[2][2]=FP_2_AW_3.Gleisabschnitt;
	
	Fahrprogramm_2[3][0]=FP_2_AW_4.Fahrbefehl;
	Fahrprogramm_2[3][1]=FP_2_AW_4.Zug;
	Fahrprogramm_2[3][2]=FP_2_AW_4.Gleisabschnitt;
	
	Fahrprogramm_2[4][0]=FP_2_AW_5.Fahrbefehl;
	Fahrprogramm_2[4][1]=FP_2_AW_5.Zug;
	Fahrprogramm_2[4][2]=FP_2_AW_5.Gleisabschnitt;
	
	/****************************************************************************/
	/*******TEST Ausgabe*******/
	byte Zug;
	Command result;
	int index;
	//Endlosschleife zum Testen
	for (index = 0; ; index = index + 1) {

		printf ("Bitte Lokomotive waehlen: ");
		scanf ("%d", &Zug);
		
		result = get_Command(Zug); //Aufruf

		printf ("\nFahrbefehl : %d \nLok: %d\nGleisabschnitt: %d\n\n",result.Fahrbefehl,result.Zug,result.Gleisabschnitt);
	}

}
