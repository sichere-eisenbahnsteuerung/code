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
	STAND = 0x0,
	FAHRT_GESCHW = 0x04,
	ANKUPPEL_GESCHW = 0x08,
	ABKUPPEL_GESCHW = 0x0C
} Geschwindigkeit;

typedef enum {
	RUECKWAERTS = 0x0,
	VORWAERTS = 0x02
} Richtung

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
	GERADEAUS,
	ABBIEGEN
} Weichenstellung

typedef enum {
	FEHLER_ZUSTAND // Unmöglicher Zustand
} FehlerCode;

/* Lokale Konstanten ********************************************************/

#define MODULE_ID 0 // fürs auditing und sw watchdog
#define PRUEFSENSOR_NR 14

/* Lokale Variablen *********************************************************/

bit wiederholen[2];
Fahranweisung fahranweisungRingpuffer[3];
byte nextFahranweisung;
Fahranweisung fahranweisung;
Zustand zustand[2];
int zeit;
byte wagonsErwartet;
byte wagonsGezaehlt;
KuppelAktion kuppelAktion;
CheckKuppelAktion checkKuppelAktion;
Lok gleisSperrung[9];
//Lok weichenSperrung[3];
Nachricht nachricht; // fürs auditing
Lok lok;

/* Prototypen fuer lokale Funktionen ****************************************/

Fahranweisung getNeueFahranweisung();
Fahranweisung getAlteFahranweisung();
Sensordaten getSensordaten();
Richtung getRichtung(byte zielNr);
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

byte getNextWeiche(byte zielNr);

void initLZ();
void workLZ();

/* Funktionsimplementierungen ***********************************************/

Weichenstellung getWeichenStellung(byte zielNr) {
	/*
	 * TODO (a better version)
	 * dirty hack, funktioniert mit aktueller topologie
	 * lösung unabhängig von der topologie nicht möglich
	 * mit aktueller streckentopologie-definition (?),
	 * xpressnet lässt nur geradeaus und abbiegen zu,
	 * nicht aber links und rechts als weichenstellung
	 */
	return zielNr + BV_zugposition[lok] < 8 ? GERADEAUS : ABBIEGEN;
}

bit checkGesperrt(byte gleisabschnittNr) {
	return (gleisSperrung[gleisabschnittNr] == KEINE) ? FALSE : TRUE;
}

Weichenstellung getWeichenStellung(byte zielNr) {
	
}

byte getNextWeiche(byte zielNr) {
	Richtung richtung = getRichtung(gleisabschnittNr);
	byte switchNr;
	if(richtung == VORWAERTS) {
		switchNr = BV_streckentopologie[gleisabschnittNr].prevSwitch;
	}
	else {
		switchNr = BV_streckentopologie[gleisabschnittNr].nextSwitch;
	}
	return switchNr;
}

bit checkBelegt(byte gleisabschnittNr) {
	byte switchNr;
	if(BV_gleisbelegung[gleisabschnittNr] == 0) {
		switchNr = getNextWeiche(gleisabschnittNr);
		if(switchNr == 0 || BV_weichenbelegung[switchNr] == 0) {
			return FALSE;
		}
		else {
			return TRUE;
		}
	}
	else {
		return TRUE;
	}
}

bit checkBefahrbarkeit(byte gleisabschnittNr) {
	byte fahrbefehl;
	byte switchNr;
	Streckenbefehl streckenbefehl;
	if(checkBelegt(gleisabschnittNr) == FALSE && checkGesperrt(gleisabschnittNr) == FALSE) {
		setGleisabschnittGesperrt(gleisabschnittNr, TRUE);
	}
	else {
		fahrbefehl = fahranweisung.fahrbefehl & 0x0E;
		if(fahrbefehl == FB_FAHREN || fahrbefehl == FB_HALTEN) {
			return FALSE;
		}
		else {
			/*
			 * Es wird davon ausgegangen, dass Lok1 nicht
			 * rangiert und deswegen die Lok ist, die ein Einfahren
			 * in den Gleisabschnitt verhindern könnte.
			 */
			if(BV_zugposition[LOK1] == gleisabschnittNr) {
				return FALSE;
			}
		}
	}
	switchNr = getNextWeiche(gleisabschnittNr);
	if(switchNr != 0) {
		streckenbefehl.Fehler = LEER;
		streckenbefehl.Lok = LEER;
		streckenbefehl.Entkoppler = LEER;
		streckenbefehl.Weiche = (switchNr << 1) + getWeichenstellung(gleisabschnittNr);
		setStreckenbefehl(streckenbefehl);
	}
	return TRUE;
}

void setGleisabschnittGesperrt(byte gleisabschnittNr, bit gesperrt) {
	gleisGesperrt[gleisabschnittNr] = gesperrt ? lok : KEINE;
}

Richtung getRichtung(byte zielNr) {
	if( BV_streckentopologie[BV_zugposition[lok]].next1 == zielNr ||
	BV_streckentopologie[BV_zugposition[lok]].next2 == zielNr) {
		return VORWAERTS;
	}
	else {
		return RUECKWAERTS;
	}
}

void setStreckenbefehl(Streckenbefehl streckenbefehl) {
	LZ_BV_streckenbefehl = streckenbefehl;
}

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
	case FEHLER_FAHRBEFEHL:
		nachricht[0] = fehlerCode;
		nachricht[1] = zustand[LOK1];
		nachricht[2] = zustand[LOK2];
		nachricht[3] = lok;
		nachricht[4] = fahranweisung.fahrbefehl;
		nachricht[5] = BV_zugposition[LOK1];
		nachricht[6] = BV_zugposition[LOK2];
		break;
	default:
		break;
	}
}

void setZustandFuerAnweisung() {
	Streckenbefehl streckenbefehl;
	switch(fahranweisung.fahrbefehl & 0x0E) {
	/* Ausnahmsweise kein break, da auch
	 * beim Anhalten für eine beliebige
	 * Zeit zunächst in den betreffenden
	 * Gleisabschnitt eingefahren werden 
	 * muss*/
	case FB_FAHREN:
		// Fahrgeschwindigkeit einlegen
		streckenbefehl.Fehler = LEER;
		streckenbefehl.Entkoppler = LEER;
		streckenbefehl.Weiche = LEER;
		streckenbefehl.Lok = FAHR_GESCHW || getRichtung(lok,gleisabschnittNr) || lok;
		setStreckenbefehl(streckenbefehl);
	case FB_HALTEN:
		zustand[lok] = FAHREND;
		break;
	case FB_ANKUPPELN:
		// Ankuppelgeschwindigkeit einlegen
		streckenbefehl.Entkoppler = LEER;
		streckenbefehl.Fehler = LEER;
		streckenbefehl.Weiche = LEER;
		streckenbefehl.Lok = ANKUPPEL_GESCHW || getRichtung(lok,gleisabschnittNr) || lok;
		setStreckenbefehl(streckenbefehl);
		zustand[lok] = ANKUPPELND;
		break;
	case FB_ABKUPPELN:
		// Abkuppelgeschwindigkeit einlegen
		streckenbefehl.Entkoppler = LEER;
		streckenbefehl.Fehler = LEER;
		streckenbefehl.Weiche = LEER;
		streckenbefehl.Lok = ABKUPPEL_GESCHW || getRichtung(lok,gleisabschnittNr) || lok;
		setStreckenbefehl(streckenbefehl);
		zustand[lok] = ABKUPPELND;
		break;
	default:
		setNachricht(FEHLER_ZUSTAND);
		hello(nachricht,MODULE_ID);
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
	for(i=0;i<6;++i) {
		nachricht[i] = 0;
	}
}

void workLZ() {
	int lok=0;
	byte verlassenerGleisabschnitt = 0;
	byte gleisabschnittNr = 0;
	Streckenbefehl streckenbefehl;
	for(lok=LOK1;lok<=LOK2;++lok) {		
		switch(zustand[lok]) {
		case HOLT_FAHRANWEISUNG:
			verlassenerGleisabschnitt = gleisabschnittNr;
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
				// Zug anhalten (Evtl. in checkBefahrbarkeit verschieben?)
				streckenbefehl.Entkoppler = LEER;
				streckenbefehl.Fehler = LEER;
				streckenbefehl.Weiche = LEER;
				streckenbefehl.Lok = STAND|| getRichtung(lok,gleisabschnittNr) || lok;
				setStreckenbefehl(streckenbefehl);
				zustand[lok] = ANGEHALTEN;
			}
			break;
		case FAHREND:
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
				setGleisabschnittGesperrt(verlassenerGleisabschnitt,FALSE);
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
			if(checkZielpositionErreicht() == TRUE) {
				kuppelAktion = ANKUPPELN;
				setGleisabschnittGesperrt(verlassenerGleisabschnitt,FALSE);
				checkKuppelAktion = BEIM_NAECHSTEN_MAL;
				wiederholen = false;
			}
			break;
		case ABKUPPELND:
			if(checkZielpositionErreicht() == TRUE) {
				// Abkuppeln
				streckenbefehl.Entkoppler = LEER;
				streckenbefehl.Fehler = LEER;
				streckenbefehl.Weiche = LEER;
				streckenbefehl.Lok = LEER;
				kuppelAktion = ABKUPPELN;
				setGleisabschnittGesperrt(verlassenerGleisabschnitt,FALSE);
				checkKuppelAktion = BEIM_NAECHSTEN_MAL;
				wiederholen = false;
			}
			break;
		default:
			setNachricht(FEHLER_ZUSTAND);
			hello(nachricht,MODULE_ID);
			break;
		}
	}
}