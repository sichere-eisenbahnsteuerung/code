/*****************************************************************************
 *
 *        Dateiname:    Leitzentrale.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Jan Kremer
 *
 *
 *        Modul:        Leitzentrale, Version 1.1
 *
 *        Beschreibung:
 *        Die Leitzentrale ist fuer die Steueurung der beiden Loks zu-
 *        staendig. Sie holt sich Fahranweisungen von den Fahrprogrammen und
 *        setzt diese in Streckenbefehle um, die zur Ueberpruefung an die
 *        Befehlsverwaltung in der Sicherheitsschicht weitergegeben werden.
 *        Um Kollisionen zu vermeiden, verwaltet sie ein aktuelles Abbild
 *        der Belegegungen und Sperrungen der Gleise sowie der Positionen
 *        der Loks und Wagons.
 *
 ****************************************************************************/

/* Includes *****************************************************************/

#include "Leitzentrale.h"
#include "Betriebsmittelverwaltung.h"

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

typedef enum {
	LOK1,
	LOK2,
	KEINE
} Lok;

typedef enum {
	FAHREND,
	WARTEND,
	ANGEHALTEN,
	ANKUPPELND,
	ABKUPPELND, 
	HOLT_FAHRANWEISUNG
} Zustand;

typedef enum {
	JETZT,
	BEIM_NAECHSTEN_MAL,
	NIE
} CheckKuppelAktion;

typedef enum { 
	ANKUPPELN, 
	ABKUPPELN,
	KEINE
} KuppelAktion;

/* Lokale Konstanten ********************************************************/

#define MODULE_ID 0 // fürs auditing und sw watchdog
#define ERROR_ZUSTAND 0 // unmöglicher zustand

/* Lokale Variablen *********************************************************/

bit wiederholen[2];
Fahranweisung fahranweisung[3];
byte nextFahranweisung;
Zustand zustand[2];
int zeit;
byte wagonsErwartet;
byte wagonsGezaehlt;
KuppelAktion kuppelAktion;
CheckKuppelAktion checkKuppelAktion;
Lok gleisSperrung[9];
Lok weichenSperrung[3];
byte nachricht[6]; // fürs auditing
Lok lok;

/* Prototypen fuer lokale Funktionen ****************************************/

Fahranweisung getNeueFahranweisung();
Fahranweisung getAlteFahranweisung();
Sensordaten getSensordaten();
bit checkBefahrbarkeit(byte gleisabschnittNr);
bit checkZielpositionErreicht();
bit checkZeit();
bit checkBelegt(byte gleisabschnittNr);
bit checkGesperrt(byte gleisabschnittNr);
bit checkAnzahlWagons();
bit checkPruefSensor(byte sensorNr);
void setGleisabschnittGesperrt(byte gleisabschnittNr, bit gesperrt);
void setStreckenbefehl(Streckenbefehl streckenbefehl);
void initLZ();
void workLZ();

/* Funktionsimplementierungen ***********************************************/

void initLZ() {
	int i=0;
	wiederholen[LOK1] = FALSE;
	wiederholen[LOK2] = FALSE;
	Fahranweisung tmp;
	tmp.fahrbefehl = 0;
	tmp.gleisabschnittNr = 0;
	for(i=0;i<3;++i) {
		fahranweisung[i] = tmp;
	}
	nextFahranweisung = 0;
	zustand[LOK1] = HOLT_FAHRANWEISUNG;
	zustand[LOK2] = HOLT_FAHRANWEISUNG;
	zeit = 0xFFFF; // höchster wert
	wagonsErwartet = 0;
	wagonsGezaehlt = 0;
	kuppelAktion = KEINE;
	checkKuppelAktion = FALSE;
	for(i=0;i<9;++i) {
		gleisSperrung[i] = KEINE;
	}
	for(i=0;i<3;++i) {
		weichenSperrung[i] = KEINE;
	}
	for(i=0;i<6;++i) {
		nachricht[i] = 0;
	}
}

void workLZ() {
	byte gleisabschnittNr = 0;
	Fahranweisung fahranweisung = { 0, 0 }; // Fahrbefehl-Typ definieren?
	for(int lok=LOK1;lok<=LOK2;++lok) {
		if(zustand[lok] == HOLT_FAHRANWEISUNG) {
			if(wiederholen) {
				fahranweisung = getNeueFahranweisung();
				// TODO;
			}
			else {
				fahranweisung = getAlteFahranweisung();
				// TODO;
			}
			gleisabschnittNr = fahranweisung.gleisabschnittNr;
			if(checkBefahrbarkeit(gleisabschnittNr) == TRUE) {
				// feststellen, welcher fahrbefehl
				// zustand setzen
			}
			else {
				//setStreckenbefehl(); Streckenbefehl setzen
				zustand[lok] = ANGEHALTEN;
			}			
		}		
		switch(zustand[lok]) {
			case FAHREND:
				break;
			case WARTEND:
				break;
			case ANGEHALTEN:
				break;
			case ANKUPPELND:
				break;
			case ABKUPPELND:
				break;
			case HOLT_FAHRANWEISUNG:

			default:
				nachricht[0] = zustand[LOK1];
				nachricht[1] = zustand[LOK2];
				nachricht[2] = lok;
				nachricht[3] = kuppelAktion;
				nachricht[4] = wagonsGezaehlt;
				nachricht[5] = wagonsErwartet;
				nachricht[6] = ERROR_ZUSTAND;				
				hello(nachricht,MODULE_ID);
				break;
		}
	}
}