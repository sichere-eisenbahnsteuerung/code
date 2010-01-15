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
#include<Fahraufgabe.h>

/* Definition globaler Konstanten *******************************************/
#define Lok0 0x0	//Lokomotive 0
#define Lok1 0x1	//Lokomotive 1
#define err 0xFF	//Error
//Definition ArraySize
#define row 10		//10 Zeilen (Test)
#define col 3		//3 Spalten

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/
typedef unsigned char byte;
//Definition Fahrprogrammarrays
typedef int Fahrprogramm[col][row];
//Definition struct Command
typedef struct Command{
	byte Fahrbefehl;
	byte Zug;
	byte Gleisabschnitt;
}Command;

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/
static Fahrprogramm Fahrprogramm_1 , Fahrprogramm_2;

/* Prototypen fuer lokale Funktionen ****************************************/

/* Funktionsimplementierungen ***********************************************/


/***************************
 ******main zum Testen!!****
 ***************************/
int main(int argc, char **argv)
{
	/*TODO*/
	//Fahrprogramm_1, FP_1_AW1=( Fahrbefehl , Lok , Gleisabschnitt )
	//Pseudo
	static Command FP_1_AW1 = {0x0,Lok0,0x1};
	static Command FP_1_AW2 = {0x0,Lok0,0x0};
	static Command FP_1_AW3 = {0x0,Lok0,0x0};
	static Command FP_1_AW4 = {0x0,Lok0,0x0};
	static Command FP_1_AW5 = {0x0,Lok0,0x0};
	static Command FP_1_AW6 = {0x0,Lok0,0x0};
	static Command FP_1_AW7 = {0x0,Lok0,0x0};
	static Command FP_1_AW8 = {0x0,Lok0,0x0};
	static Command FP_1_AW9 = {0x0,Lok0,0x0};
	static Command FP_1_AW10= {0x0,Lok0,0x0};

	/*TODO*/
	//Definition Fahrgrogramm_1
	Fahrprogramm_1[0][0]=FP_1_AW1.Fahrbefehl;
	Fahrprogramm_1[0][1]=FP_1_AW1.Zug;
	Fahrprogramm_1[0][2]=FP_1_AW1.Gleisabschnitt;

}
/**********************************************************************/
//Schnittstelle
//Übergabe Fahranweisung an Leitzentrale
Command get_Command(byte Lok)
{
	//Counter für Fahrprogramm_1 bzw. _2
	static unsigned int f0index = 0 , f1index = 1; 
	Command Anweisung; //Übergabe var
		
	//Anweisung für Lok 0
	/*TODO*/
	/*-Array überlauf*/
	if (Lok == Lok0){
		f0index ++;
		//Zuweisung
		Anweisung.Fahrbefehl = Fahrprogramm_1[f0index][0];
		Anweisung.Zug = Lok0;
		Anweisung.Gleisabschnitt = Fahrprogramm_1[f0index][2];
		//Rückgabe aktuelle Anweisung = (Fahrbefehl,Zug0,Gleisabschnit)
		return Anweisung;
	}
	//Anweisung für Lok 1
	else if(Lok == Lok1){
		f1index ++;
		//Zuweisung
		Anweisung.Fahrbefehl = Fahrprogramm_1[f1index][0];
		Anweisung.Zug = Lok1;
		Anweisung.Gleisabschnitt = Fahrprogramm_1[f1index][2];
		
		//Rückgabe aktuelle Anweisung = (Fahrbefehl,Zug1,Gleisabschnit)
		return Anweisung;
	}
	//Fehler Anweisung
	else{
		//Error Zuweisung
		Anweisung.Fahrbefehl = err;
		Anweisung.Zug = err;
		Anweisung.Zug = err;
		//Rückgabe aktuelle Anweisung = (Fahrbefehl,Zug,Gleisabschnit)
		return Anweisung;
	}
}//Ende get_Command
