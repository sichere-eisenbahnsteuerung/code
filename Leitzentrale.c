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

#include "Betriebsmittelverwaltung.h"
#include "Befehlsvalidierung.h"
#include "Leitzentrale.h"

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

typedef byte[6] Nachricht;

typedef enum {
	FB_FAHREN = 0x0,
	FB_ANKUPPELN = 0x2,
	FB_ABKUPPELN = 0x4,
	FB_HALTEN = 0x6
} Fahrbefehl;

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

typedef enum {
	FEHLER_ZUSTAND // Unmöglicher Zustand
} FehlerCode;

/* Lokale Konstanten ********************************************************/

#define MODULE_ID 0 // frs auditing und sw watchdogg
#define PRUEFSENSOR_NR 14

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
Nachricht nachricht; // fürs auditing
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

void setNachricht(FehlerCode fehlerCode);
void setZustandFuerFahranweisung();

void initLZ();
void workLZ();

/* Funktionsimplementierungen ***********************************************/

void setNachricht(FehlerCode fehlerCode) {
	switch(fehlerCode) {
	case FEHLER_ZUSTAND:
		nachricht[0] = fehlerCode;
		nachricht[1] = zustand[LOK1];
		nachricht[2] = zustand[LOK2];
		nachricht[3] = lok;
		nachricht[4] = kuppelAktion;
		nachricht[5] = wagonsGezaehlt;
		nachricht[6] = wagonsErwartet;
		break;
	default:
		break;
	}
}

void setZustandFuerAnweisung() {
	switch(fahranweisung.fahrbefehl & 0x0E) {
	/* Ausnahmsweise kein break, da auch
	 * beim Anhalten für eine beliebige
	 * Zeit zunächst in den betreffenden
	 * Gleisabschnitt eingefahren werden 
	 * muss*/
	case FB_FAHREN:
	case FB_HALTEN:
		zustand[lok] = FAHREND;
		break;
	case FB_ANKUPPELN:
		zustand[lok] = ANKUPPELND;
		break;
	case FB_ABKUPPELN:
		zustand[lok] = ABKUPPELND;
		break;
	default:
		// error msg
		break;
	}
}

void initLZ() {
	int i=0;
	wiederholen[LOK1] = FALSE;
	wiederholen[LOK2] = FALSE;
	for(i=0;i<3;++i) {
		fahranweisung[i].fahrbefehl = 0;
		fahranweisung[i].gleisabschnittNr = 0;
	}
	nextFahranweisung = 0;
	zustand[LOK1] = HOLT_FAHRANWEISUNG;
	zustand[LOK2] = HOLT_FAHRANWEISUNG;
	zeit = 0xFFFF; // höchster Wert
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
	int lok=0;
	byte gleisabschnittNr = 0;
	Fahranweisung fahranweisung = { 0, 0 }; // Fahrbefehl-Typ definieren?
	for(lok=LOK1;lok<=LOK2;++lok) {		
		switch(zustand[lok]) {
		case HOLT_FAHRANWEISUNG:
			if(wiederholen) {
				fahranweisung = getNeueFahranweisung();
			}
			else {
				fahranweisung = getAlteFahranweisung();
			}
			gleisabschnittNr = fahranweisung.gleisabschnittNr;
			if(checkBefahrbarkeit(gleisabschnittNr) == TRUE) {
				setZustandFuerFahranweisung();
			}
			else {
				zustand[lok] = ANGEHALTEN;
			}
			break;
		case FAHREND:
			// TODO Fahrgeschwindigkeit einlegen
			
			if(	(checkZielpositionErreicht() == TRUE && 
				 checkKuppelAktion == JETZT &&
				 checkAnzahlWagons() == FALSE) || 
			
				(checkZielpositionErreicht() == FALSE &&
				kuppelAktion == ANKUPPELN &&
				checkPruefSensor(PRUEFSENSOR_NR) == TRUE)) {
					
					if(kuppelAktion == ANKUPPELN) {
						kuppelAktion = KEINE;
					}
					
					wiederholen[lok] = TRUE;
					zustand[lok] = HOLT_FAHRANWEISUNG;
			}
			else {
				// TODO gerade verlassenes gleis sperren
				if(checkKuppelAktion == BEIM_NAECHSTEN_MAL) {
					checkKuppelAktion = JETZT;
				}
				else {
					checkKuppelAktion = NIE;
				}
				
				if(fahranweisung.fahrbefehl & 0x0E == FB_HALTEN) {
					zustand[lok] = WARTEND;
				}
				else {
					zustand[lok] = HOLT_FAHRANWEISUNG;
				}
			}
			break;
		case WARTEND:
			if(checkZeit() == FALSE) {
				zustand[lok] = HOLT_FAHRANWEISUNG;
			}
			break;
		case ANGEHALTEN:
			if(checkBefahrbarkeit(gleisabschnittNr) == TRUE) {
				setZustandFuerFahranweisung();
			}
			break;
		case ANKUPPELND:
			// ankuppelgeschwindigkeit einlegen
			if(checkZielpositionErreicht() == TRUE) {
				kuppelAktion = ANKUPPELN;
			}
			checkKuppelAktion = BEIM_NAECHSTEN_MAL;
			wiederholen = false;
			break;
		case ABKUPPELND:
			// abkuppelgeschwindigkeit einlegen
			if(checkZielpositionErreicht() == TRUE) {
				kuppelAktion = ABKUPPELN;
			}
			// gerade verlassenen gleisabschnitt entsperren
			checkKuppelAktion = BEIM_NAECHSTEN_MAL;
			wiederholen = false;
			break;
		default:
			setNachricht(FEHLER_ZUSTAND);
			hello(nachricht,MODULE_ID);
			break;
		}
	}
}