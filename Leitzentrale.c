/*****************************************************************************
 *
 *        Dateiname:    Leitzentrale.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Jan Kremer
 *
 *
 *        Modul:        Leitzentrale, Version 1.2
 *
 *        Beschreibung:
 *        Die Leitzentrale ist fuer die Steuerung der beiden Loks zu-
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
#include "AuditingSystemSendMsg.h"
#include "SoftwareWatchdogHelloModul.h"
#include "Fahrprogramm.h"
#include "Leitzentrale.h"

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

// Eine Nachricht besteht aus 6 frei belegbaren Bytes
typedef byte Nachricht[6];

typedef enum {
	FB_FAHREN = 0x0,
	FB_ANKUPPELN = 0x2,
	FB_ABKUPPELN = 0x4,
	FB_HALTEN = 0x6
} Fahrbefehl;

typedef enum {
	LOK1,
	LOK2,
	KEINE_LOK
} Lok;

typedef enum {
	RUECKWAERTS = 0x0,
	VORWAERTS = 0x02
} Richtung;

// Die verschiedenen Zustaende, in denen sich die Leitzentrale befinden kann
typedef enum {
	FAHREND,
	WARTEND,
	ANGEHALTEN,
	ANKUPPELND,
	ABKUPPELND, 
	HOLT_FAHRANWEISUNG
} Zustand;

// Eine Variable vom Typ CheckKuppelAktion legt fest, wann geprueft werden
// soll, ob die Kuppel-Aktion erfolgreich war
typedef enum {
	JETZT,
	BEIM_NAECHSTEN_MAL,
	NIE
} CheckKuppelAktion;

typedef enum { 
	ANKUPPELN, 
	ABKUPPELN,
	KEINE_AKTION
} KuppelAktion;

typedef enum {
	GERADEAUS,
	ABBIEGEN
} Weichenstellung;
	
typedef enum {
	SENSOR_FREI,
	SENSOR_BELEGT
} Sensorbelegung;

// Fehlercodes fuer die Meldung ans Auditingsystem
typedef enum {
	FEHLER_ZUSTAND, 	// Unmoeglicher Zustand
	FEHLER_FAHRBEFEHL, 	// Nicht definierter Fahrbefehl
	FEHLER_WEICHENSTELLUNG,	// Weichenstellung laesst sich nicht bestimmen
	ZIEL_ERREICHT, 		// Angestrebte Zielposition erreicht
	NICHT_BEFAHRBAR, 	// Gleisabschnitt ist nicht befahrbar
	KUPPELN_FEHLGESCHLAGEN, // Kuppeln ist fehlgeschlagen
	ANKUPPEL_VERSUCH, 	// Ein Ankuppelversuch wird gestartet
	ABKUPPEL_VERSUCH 	// Ein Abkuppelversuch wird gestartet
} FehlerCode;

/* Lokale Konstanten ********************************************************/

// Modul-ID, die an den SW Watchdog und an das Auditingsystem ueberliefert
// werden
#define MODULE_ID MODUL_LZ

// Die Sensor-Nummer des Kuppelsensors
#define KUPPELSENSOR_NR 14

// Die Anzahl der Loks auf der Strecke.
#define ANZAHL_LOKS 2

// Die Anzahl der Gleisabschnitte + 1, 
// da die Zaehlung bei 1 beginnt, Weichen ausgenommen
#define ANZAHL_GLEISE 11

// Gibt an, um wieviel die Pseudo-Wartezeit pro Durchlauf verringert werden
// soll.
#define ZEIT_DEKREMENT 1

/* Lokale Variablen *********************************************************/

// Speichert fuer jede Lok, ob eine Wiederholung der letzten Fahrsequenz
// noetig ist. Muesste eigentlich boolean sein, der C-Compiler unterstuetzt
// leider keine bit-Arrays.
byte wiederholen[ANZAHL_LOKS];

// Der Ringpuffer fuer die Fahranweisungen.
Fahranweisung fahranweisungRingpuffer[ANZAHL_LOKS][3];

// Die aktuelle Fahranweisung fuer jede Lok
Fahranweisung fahranweisung[ANZAHL_LOKS];

// Der aktuelle Zustand fuer jede Lok
Zustand zustand[ANZAHL_LOKS];

// Die (Pseudo-)Wartezeit, falls eine Lok eine bestimmte Zeit warten muss.
int zeit = 0;

// Die gerade gewaehlte Kuppelaktion
KuppelAktion kuppelAktion = KEINE_AKTION;

// Gibt an, wann der Erfolg der Kuppelaktion ueberprueft werden soll
CheckKuppelAktion checkKuppelAktion = NIE;

// Speichert fuer jedes Gleis, ob es gesperrt ist.
Lok gleisSperrung[ANZAHL_GLEISE] = {
	0,
	KEINE_LOK, 	// Gleisabschnitt 1
 	KEINE_LOK, 	// Gleisabschnitt 2
	KEINE_LOK, 	// Gleisabschnitt 3
	KEINE_LOK, 	// Gleisabschnitt 4
 	KEINE_LOK, 	// Gleisabschnitt 5
	KEINE_LOK,	// Gleisabschnitt 6
	LOK1,		// Gleisabschnitt 7
	LOK2,		// Gleisabschnitt 8
	KEINE_LOK	// Gleisabschnitt 9
};

//Die aktuell ausgewaehlte Lok
Lok lok = LOK1;

// Der kuppelSensor ueberprueft, ob sich ein Wagon noch auf dem Abstellgleis
// befindet. Dies ist wichtig, um sicherzustellen, dass das Ankuppeln geklappt
// hat.
byte kuppelSensor = 0;

// Speichert den aktuellen Status, um ihn an den SW Watchdog zu melden.
byte wdStatus = 0;

/* Prototypen fuer lokale Funktionen ****************************************/

/* 
 * Gibt fuer ein Zielgleis, die passende Weichenstellung aus, in die die
 * Weiche gebracht werden muss, um das Gleis zu erreichen.
 */
static Weichenstellung getWeichenStellung(byte zielNr);

/*
 * Gibt die naechste Fahranweisung zurueck. Muss eine Sequenz von
 * Fahranweisungen wiederholt werden, so werden alte Fahranweisungen aus dem
 * Ringpuffer geholt, sonst neue hineingeschrieben und zurueckgegeben.
 */
static Fahranweisung getFahranweisung();

/*
 * Gibt die Richtung zurueck in die sich die aktuelle Lok bewegen muss,
 * um zum angegeben Zielgleis zu gelangen.
 */
static Richtung getRichtung(byte zielNr);

/*
 * Liefert die Sensordaten zurueck, die sich gerade im Shared Memory befinden.
 * Nach dem Zugriff wird das Shared Memory fuer die Sensordaten geleert.
 */
static Sensordaten getSensordaten();

/*
 * Liefert die naechste Weiche zurueck, die sich zwischen Zielgleis und
 * aktuellem Gleis befindet. 0, wenn sich dort keine Weiche befindet.
 */
static byte getNextWeiche(byte zielNr);

/*
 * Prueft, ob das angegebene Gleis befahrbar ist und stellt, falls es befahr-
 * bar ist, ggf. die zwischen aktuellem und Zielgleis liegende Weiche
 * in die richtige Stellung. Gibt TRUE zurueck, wenn das Gleis befahrbar ist,
 * sonst FALSE.
 */
static boolean checkBefahrbarkeit(byte gleisabschnittNr);

/*
 * Prueft, ob das angegebene Gleis belegt ist, oder nicht. Gibt TRUE zurueck,
 * wenn das Gleis belegt ist, sonst FALSE.
 */
static boolean checkBelegt(byte gleisabschnittNr);

/*
 * Prueft, ob das angegebene Gleis gesperrt ist. Gibt TRUE zurueck, wenn
 * das Gleis gesperrt ist, sonst FALSE.
 */
static boolean checkGesperrt(byte gleisabschnittNr);

/*
 * Prueft, ob das angegebene Gleis erreicht ist. Gibt TRUE zurueck, wenn
 * es erreicht wurde, sonst FALSE.
 */
static boolean checkZielpositionErreicht(byte zielNr);

/*
 * Prueft, ob die Wartezeit bereits vergangen ist. Zaehlt die Zeit runter, und
 * gibt TRUE zurueck, wenn noch Zeit ist, sonst FALSE.
 */
static boolean checkZeit();

/*
 * Prueft, ob die Anzahl der Wagons auf dem Gleis, auf dem sich die aktuelle 
 * Lok gerade befindet, der erwarteten Anzahl entspricht. Diese Hilfsfunktion
 * wird fuer den Abkuppelvorgang benoetigt. Gibt TRUE zurueck, wenn die
 * Anzahl der Erwartung entspricht, sonst FALSE.
 */
static boolean checkAnzahlWagons();

/*
 * Sperrt oder entsperrt den angegebenen Gleisabschnitt fuer die andere Lok.
 */
static void setGleisabschnittGesperrt(byte gleisabschnittNr, boolean gesperrt);

/*
 * Schreibt den angegebenen Streckenbefehl ins Shared Memory.
 */
static void setStreckenbefehl(Streckenbefehl streckenbefehl);

/*
 * Generiert eine Nachricht fuer den angegebenen Fehlercode. Diese Nachricht
 * wird dann ans Auditingsystem gesendet werden.
 */
static void sendNachricht(FehlerCode fehlerCode);

/*
 * Setzt den Zustand fuer die aktuelle Fahranweisung und setzt den passenden
 * Streckenbefehl ins Shared Memory in Abhaengigkeit vom angegebenen
 * Gleisabschnitt.
 */
static void setZustandFuerFahranweisung(byte gleisabschnittNr);

/* Funktionsimplementierungen ***********************************************/

static boolean checkAnzahlWagons() {
	// Befinden sich ausser der Lok noch Wagons
	// auf dem Gleis, so hat das Abkuppeln nicht geklappt
	if(BV_gleisBelegung[BV_zugPosition[lok]] > 1) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}

static Sensordaten getSensordaten() {
	Sensordaten sensordaten = {LEER,LEER,LEER};
	sensordaten = BV_LZ_sensordaten;
	// Leert die Sensordaten im Shared Memory, nachdem
	// sie geholt wurden.
	BV_LZ_sensordaten.Byte0 = LEER;
	BV_LZ_sensordaten.Byte1 = LEER;
	BV_LZ_sensordaten.Fehler = LEER;
	return sensordaten;
}

static boolean checkZielpositionErreicht(byte zielNr) {
	if(BV_zugPosition[lok] == zielNr) {
		return TRUE;
	}
	else {
		sendNachricht(ZIEL_ERREICHT);
		return FALSE;
	}
}

static boolean checkZeit() {
	zeit -= ZEIT_DEKREMENT;
	if(zeit > 0) {
		return TRUE;
	}
	else {
		// Zeit zurueck auf Maximalwert setzen.
		zeit = 0;
		return FALSE;
	}
}

static Fahranweisung getFahranweisung() {
	// Verwaltet zwei Indizes, der Index zum Lesen
	// wird verwendet, wenn eine alte Fahranweisung
	// aus dem Ringpuffer geholt werden soll.
	// Der Index zum Schreiben gibt die Position an, an der
	// eine neue Fahranweisung in den Ringpuffer geschrieben
	// werden soll.
	static byte lesen[2] = { 0, 0 };
	static byte schreiben[2] = { 0, 0 };
	Fahranweisung fa;
	
	// Wenn eine Fahranweisungsequenz wiederholt werden soll,
	// dann lese solange aus dem Ringpuffer, bis die letzte
	// Schreibposition wieder erreicht wurde.
	if(wiederholen[lok] == TRUE || lesen[lok] != schreiben[lok]) {
		fa = fahranweisungRingpuffer[lok][lesen[lok]];
		lesen[lok] = (lesen[lok] + 1) % 3;
	}
	else {
		fa = get_command(lok);
		fahranweisungRingpuffer[lok][schreiben[lok]] = fa;
		schreiben[lok] = (schreiben[lok] + 1) % 3;
		lesen[lok] = schreiben[lok];
	}
	
	return fa;
}

static Weichenstellung getWeichenStellung(byte zielNr) {
	/*
	 * Diese Loesung funktioniert nur mit der von uns
	 * gewaehlten Gleistopologie, da XpressNet bei
	 * der Weichenstellung logisch nicht zwischen links
	 * und rechts, sondern zwischen geradeaus und abbiegen
	 * unterscheidet.
	 */
	if( 	(zielNr == 1 && BV_zugPosition[lok] == 8) ||
		(zielNr == 8 && BV_zugPosition[lok] == 1) ||
		(zielNr == 1 && BV_zugPosition[lok] == 7) ||
		(zielNr == 7 && BV_zugPosition[lok] == 1) ||
		(zielNr == 4 && BV_zugPosition[lok] == 7) ||
		(zielNr == 7 && BV_zugPosition[lok] == 4)) {
		
		return ABBIEGEN;
	}
	else if((zielNr == 1 && BV_zugPosition[lok] == 6) ||
		(zielNr == 6 && BV_zugPosition[lok] == 1) ||
		(zielNr == 1 && BV_zugPosition[lok] == 2) ||
		(zielNr == 2 && BV_zugPosition[lok] == 1) ||
		(zielNr == 3 && BV_zugPosition[lok] == 4) ||
		(zielNr == 4 && BV_zugPosition[lok] == 3)) {
		
		return GERADEAUS;
	}
	else {
		sendNachricht(FEHLER_WEICHENSTELLUNG);
		return -1;
	}
}

static boolean checkGesperrt(byte gleisabschnittNr) {
	return (gleisSperrung[gleisabschnittNr] == KEINE_LOK) ? FALSE : TRUE;
}

static byte getNextWeiche(byte zielNr) {
	Richtung richtung = getRichtung(zielNr);
	byte switchNr;
	if(richtung == VORWAERTS) {
		switchNr = BV_streckentopologie[zielNr].prevSwitch;
	}
	else {
		switchNr = BV_streckentopologie[zielNr].nextSwitch;
	}
	return switchNr;
}

static boolean checkBelegt(byte gleisabschnittNr) {
	byte switchNr;
	// Ein Gleisabschnitt ist belegt, wenn der Gleisabschnitt
	// selbst nicht frei ist und evtl. zwischen aktuellem
	// Gleis und dem Gleisabschnitt eine belegte Weiche liegt.
	if(BV_gleisBelegung[gleisabschnittNr] == 0) {
		switchNr = getNextWeiche(gleisabschnittNr);
		if(switchNr == 0 || BV_weichenBelegung[switchNr] == 0) {
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

static boolean checkBefahrbarkeit(byte gleisabschnittNr) {
	byte fahrbefehl;
	byte switchNr;
	Streckenbefehl streckenbefehl;
	// Wenn der zu befahrende Gleisabschnitt nicht belegt ist
	// und nicht gesperrt ist, dann sperre ihn fuer die andere
	// Lok
	if(checkBelegt(gleisabschnittNr) == FALSE && 
	   checkGesperrt(gleisabschnittNr) == FALSE) {
		setGleisabschnittGesperrt(gleisabschnittNr, TRUE);
	}
	else {
		fahrbefehl = fahranweisung[lok].fahrbefehl & 0x0E;
		// Sonst, wenn mit Vollgas in den Gleisabschnitt eingefahren
		// werden soll, gib zurueck, dass der Abschnitt
		// nicht befahrbar ist.
		if(fahrbefehl == FB_FAHREN || fahrbefehl == FB_HALTEN) {
			return FALSE;
		}
		else {
			/*
			 * Es wird davon ausgegangen, dass Lok1 nicht
			 * rangiert und deswegen die Lok ist, die ein 
			 * Einfahren in den Gleisabschnitt verhindern 
			 * koennte.
			 */
				
			// Wenn der Gleisabschnitt nicht frei ist, 
			// und dort nicht mit Vollgas hineingefahren wird
			// aber eine Lok auf dem Gleis steht, dann
			// liefere zurueck, dass der Abschnitt nicht
			// befahrbar ist.
			if(BV_zugPosition[LOK1] == gleisabschnittNr) {
				return FALSE;
			}
		}
	}
	// Wenn eine Weiche zwischen aktuellem und naechsten Gleisabschnitt
	// ist, so soll diese gestellt werden.
	switchNr = getNextWeiche(gleisabschnittNr);
	if(switchNr != 0) {
		streckenbefehl.Fehler = LEER;
		streckenbefehl.Lok = LEER;
		streckenbefehl.Entkoppler = LEER;
		streckenbefehl.Weiche = (switchNr << 1) + getWeichenStellung(gleisabschnittNr);
		setStreckenbefehl(streckenbefehl);
	}
	return TRUE;
}

static void setGleisabschnittGesperrt(byte gleisabschnittNr, boolean gesperrt) {
	gleisSperrung[gleisabschnittNr] = gesperrt ? lok : KEINE_LOK;
}

static Richtung getRichtung(byte zielNr) {
	// Bestimmt, ob das naechste Gleis im oder gegen
	// den Uhrzeigersinn anzufahren ist.
	if( BV_streckentopologie[BV_zugPosition[lok]].next1 == zielNr ||
	BV_streckentopologie[BV_zugPosition[lok]].next2 == zielNr) {
		return VORWAERTS;
	}
	else {
		return RUECKWAERTS;
	}
}

static void setStreckenbefehl(Streckenbefehl streckenbefehl) {
	// Streckenbefehl in das Shared Memory schreiben
	LZ_BV_streckenbefehl = streckenbefehl;
}

static void sendNachricht(FehlerCode fehlerCode) {
	// Eine 6-Byte grosse Nachricht, die ans 
	// Auditingsystem geschickt werden kann.
	Nachricht nachricht;
	
	nachricht[0] = fehlerCode | (lok << 4);
	nachricht[1] = zustand[LOK1];
	nachricht[2] = zustand[LOK2];
	nachricht[3] = BV_zugPosition[LOK1];
	nachricht[4] = BV_zugPosition[LOK2];
	
	switch(fehlerCode) {
	/*
	 * Hier entgegen der Coding-Richtlinien kein break,
	 * da das 6. Byte fuer diese Fehlercodes gleich
	 * kodiert werden soll.
	 */
	case FEHLER_ZUSTAND:
	case FEHLER_FAHRBEFEHL:
	case FEHLER_WEICHENSTELLUNG:
	case ZIEL_ERREICHT:
	case KUPPELN_FEHLGESCHLAGEN:
	case ANKUPPEL_VERSUCH:
	case ABKUPPEL_VERSUCH:
		nachricht[5] = fahranweisung[lok].fahrbefehl;
		break;
	case NICHT_BEFAHRBAR:
		nachricht[5] = fahranweisung[lok].gleisabschnittNr;
		break;
	default:
		break;
	}
	/* TEST sendMsg(MODULE_ID,nachricht); */
}

static void setZustandFuerFahranweisung(byte gleisabschnittNr) {
	Streckenbefehl streckenbefehl;
	// Niedrigstes Bit ignorieren, welches nur die Lok
	// enthaelt, fuer die dieser Fahrbefehl gilt
	switch(fahranweisung[lok].fahrbefehl & 0x0E) {
	/* Entgegen der Coding-Richtlinien kein 
	 * break, da auch beim Anhalten fuer eine 
	 * beliebige Zeit zunaechst in den betreffenden
	 * Gleisabschnitt eingefahren werden muss
	 */
	case FB_FAHREN:
		// Fahrgeschwindigkeit einlegen
		streckenbefehl.Fehler = LEER;
		streckenbefehl.Entkoppler = LEER;
		streckenbefehl.Weiche = LEER;
		streckenbefehl.Lok = (BV_V_VOLLGAS << 2) | getRichtung(gleisabschnittNr) | lok;
		setStreckenbefehl(streckenbefehl);
	case FB_HALTEN:
		zustand[lok] = FAHREND;
		break;
	case FB_ANKUPPELN:
		// Ankuppelgeschwindigkeit einlegen
		streckenbefehl.Entkoppler = LEER;
		streckenbefehl.Fehler = LEER;
		streckenbefehl.Weiche = LEER;
		streckenbefehl.Lok = (BV_V_ANKUPPELN << 2) | getRichtung(gleisabschnittNr) | lok;
		setStreckenbefehl(streckenbefehl);
		zustand[lok] = ANKUPPELND;
		break;
	case FB_ABKUPPELN:
		// Abkuppelgeschwindigkeit einlegen
		streckenbefehl.Entkoppler = LEER;
		streckenbefehl.Fehler = LEER;
		streckenbefehl.Weiche = LEER;
		streckenbefehl.Lok = (BV_V_ABKUPPELN << 2) | getRichtung(gleisabschnittNr) | lok;
		setStreckenbefehl(streckenbefehl);
		zustand[lok] = ABKUPPELND;
		break;
	default:
		// Ungueltiger Zustand
		sendNachricht(FEHLER_ZUSTAND);
		break;
	}
}

/* Globale Init-Funktion */
void initLZ() {
	int i=0;
	// Initialisieren der Werte, deren Groesse von der Anzahl
	// der Loks abhaengt.
	for(i=0;i<ANZAHL_LOKS;++i) {
		wiederholen[i] = FALSE;
		zustand[i] = HOLT_FAHRANWEISUNG;
		fahranweisung[i].fahrbefehl = 0;
		fahranweisung[i].gleisabschnittNr = 0;
		fahranweisungRingpuffer[i][0] = fahranweisung[i];
		fahranweisungRingpuffer[i][1] = fahranweisung[i];
		fahranweisungRingpuffer[i][2] = fahranweisung[i];
	}
}

/* Globale Work-Funktion */
void workLZ() {
	// Die GleisabschnittNr, des gerade vom Zug 
	// verlassenen Gleisabschnitts
	byte verlassenerGleisabschnitt[ANZAHL_LOKS] = { 0, 0 };
	// Aktuelle GleisabschnittNr
	byte gleisabschnittNr[ANZAHL_LOKS] = { 0, 0 };
	Streckenbefehl streckenbefehl;
	Sensordaten sensordaten = { LEER,LEER,LEER };
	// Aktuelle Sensordaten holen
	sensordaten = getSensordaten();
	// Status des Kuppelsensors speichern
	// Sensorbelegung ist als als 16-Bit Liste in zwei Bytes gespeichert
	// Je nachdem, ob die Sensor-Nr größer oder kleiner 8 ist, muss
	// das erste oder das zweite Byte angesprochen werden.
	if(KUPPELSENSOR_NR > 8) {
		kuppelSensor = (sensordaten.Byte1 >> ((KUPPELSENSOR_NR - 1) % 8)) & 0x1;
	}
	else {
		kuppelSensor = (sensordaten.Byte0 >> (KUPPELSENSOR_NR - 1)) & 0x1;
	}
	
	// Nacheinander jeweils das Fahrprogramm fuer Lok1 und Lok2 ausfuehren
	for(lok=LOK1;lok<ANZAHL_LOKS;++lok) {
		// Status fuer den Watchdog speichern
		// Hohes Byte enthaelt einen Zaehler, niederes Zustand
		// und aktuelle Lok
		wdStatus = (wdStatus & 0xF0) + 0x10;
		wdStatus |= (lok << 3) + zustand[lok];
		
		switch(zustand[lok]) {
			// Neue Fahranweisung wird geholt
		case HOLT_FAHRANWEISUNG:
			// Status fuer den Watchdog
			// Speichert Lok und Zustand, aber keinen
			// Zaehler, da dieser Zustand nicht keinen
			// Uebergang zu sich selbst machen darf.
			wdStatus = (lok << 3) | zustand[lok];
			
			// Vor Holen des neuen Gleisabschnitts, verlassenen
			// speichern
			verlassenerGleisabschnitt[lok] = gleisabschnittNr[lok];
			
			// Neue GleisabschnittsNr holen
			fahranweisung[lok] = getFahranweisung();
			gleisabschnittNr[lok] = fahranweisung[lok].gleisabschnittNr;
			
			// Wenn der Gleisabschnitt befahrbar ist,
			// in den naechsten Zustand wechseln, sonst anhalten
			if(checkBefahrbarkeit(gleisabschnittNr[lok]) == TRUE) {
				setZustandFuerFahranweisung(gleisabschnittNr[lok]);
			}
			else {
				// Nachricht an das Auditingsystem
				sendNachricht(NICHT_BEFAHRBAR);
				// Zug anhalten
				streckenbefehl.Entkoppler = LEER;
				streckenbefehl.Fehler = LEER;
				streckenbefehl.Weiche = LEER;
				streckenbefehl.Lok = (BV_V_STAND << 2) | getRichtung(gleisabschnittNr[lok]) | lok;
				setStreckenbefehl(streckenbefehl);
				zustand[lok] = ANGEHALTEN;
			}
			break;
			// Der Zug ist im Fahrbetrieb unterwegs
		case FAHREND:			
			// Prueft, ob ein An- oder Abkuppeln evtl. wiederholt
			// werden muss.
			if(
			// Zielposition wurde erreicht; es laesst sich in
			// diesem Gleisabschnitt bestimmen, ob das Kuppeln
			// geklappt hat und die Anzahl der Wagons hinter
			// oder vor dem Zug entspricht nicht der erwarteten
			(checkZielpositionErreicht(gleisabschnittNr[lok]) == TRUE && 
			 checkKuppelAktion == JETZT &&
		         checkAnzahlWagons() == FALSE) || 
			
			// Die Zielposition wurde noch nicht erreicht,
			// es wurde vorher angekuppelt und der Sensor auf dem
			// Abstellgleis zeigt allerdings immer noch eine 
			// Belegung durch einen Wagon an
			(checkZielpositionErreicht(gleisabschnittNr[lok]) == FALSE &&
			 kuppelAktion == ANKUPPELN &&
			 kuppelSensor == SENSOR_BELEGT)) {
					
					// Um zu wissen, ob der Kuppelsensor
					// abgefragt werden muss, wird mit
					// kuppelAktion gespeichert, ob
					// angekuppelt wurde. Im Gegensatz zum
					// Abkuppeln kann der Erfolg beim
					// Ankuppeln sofort im naechsten 
					// Abschnitt geprueft werden. 
					
					// Wenn das Ankuppeln fehlschlug,
					// die kuppelAktion zuruecksetzen
					if(kuppelAktion == ANKUPPELN) {
						kuppelAktion = KEINE_AKTION;
					}
					
					// Nachricht ans Auditingsystem
					sendNachricht(KUPPELN_FEHLGESCHLAGEN);
					
					// Kuppelmanoever wiederholen und 
					// Fahranweisung holen
					wiederholen[lok] = TRUE;
					zustand[lok] = HOLT_FAHRANWEISUNG;
			}
			else {
				// Wenn das Kuppelmanoever nicht wiederholt
				// werden muss, den verlassenen Gleisabschnitt
				// wieder entsperren.
				setGleisabschnittGesperrt(verlassenerGleisabschnitt[lok],FALSE);
				
				// Wenn beim naechsten Mal der Erfolg des
				// Abkuppeln geprueft werden muss, den
				// Zeitpunkt der Ueberpruefung naeherruecken
				if(checkKuppelAktion == BEIM_NAECHSTEN_MAL) {
					checkKuppelAktion = JETZT;
				}
				else {
					checkKuppelAktion = NIE;
				}
				
				// Muss in diesem Abschnitt fuer eine gewisse
				// Zeit gewartet werden?
				if(fahranweisung[lok].fahrbefehl & 0x0E == FB_HALTEN) {
					// Holt sich die (Pseudo-)Zeit, die gewartet werden soll
					zeit = (fahranweisung[lok].fahrbefehl & 0xF0);
					// Zug anhalten
					streckenbefehl.Entkoppler = LEER;
					streckenbefehl.Fehler = LEER;
					streckenbefehl.Weiche = LEER;
					streckenbefehl.Lok = (BV_V_STAND << 2) | getRichtung(gleisabschnittNr[lok]) | lok;
					setStreckenbefehl(streckenbefehl);
					zustand[lok] = WARTEND;
				}
				else {
					// Wenn nicht, dann neue Fahranweisung
					// holen
					zustand[lok] = HOLT_FAHRANWEISUNG;
				}
			}
			break;
			// Der Zug muss fuer eine gewisse Zeit warten.
		case WARTEND:
			// Ist die Zeit abgelaufen, wird eine neue
			// Fahranweisung geholt.
			if(checkZeit() == FALSE) {
				zustand[lok] = HOLT_FAHRANWEISUNG;
			}
			break;
			// Der Zug musste anhalten, weil ein Gleisabschnitt
			// nicht befahrbar war.
		case ANGEHALTEN:
			// Ist der Gleisabschnitt wieder befahrbar, mit der
			// geplanten Aktion fortfahren.
			if(checkBefahrbarkeit(gleisabschnittNr[lok]) == TRUE) {
				setZustandFuerFahranweisung(gleisabschnittNr[lok]);
			}
			break;
			// Die Lok kuppelt an
		case ANKUPPELND:
			// Wenn das Kuppelgleis wieder verlassen wurde, 
			// Gleisabschnitt entsperren und bei Erreichen des
			// naechsten Gleisabschnitts pruefen, ob der Vorgang
			// erfolgreich war.
			if(checkZielpositionErreicht(gleisabschnittNr[lok]) == TRUE) {
				kuppelAktion = ANKUPPELN;
				setGleisabschnittGesperrt(verlassenerGleisabschnitt[lok],FALSE);
				checkKuppelAktion = BEIM_NAECHSTEN_MAL;
				wiederholen[lok] = FALSE;
				sendNachricht(ANKUPPEL_VERSUCH);
			}
			break;
			// Die Lok kuppelt ab
		case ABKUPPELND:
			// Wenn die Zielposition erreicht ist, abkuppeln.
			if(checkZielpositionErreicht(gleisabschnittNr[lok]) == TRUE) {
				// Abkuppeln
				streckenbefehl.Entkoppler = (gleisabschnittNr[lok] << 1) + 1;
				streckenbefehl.Fehler = LEER;
				streckenbefehl.Weiche = LEER;
				streckenbefehl.Lok = LEER;
				kuppelAktion = ABKUPPELN;
				setGleisabschnittGesperrt(verlassenerGleisabschnitt[lok],FALSE);
				checkKuppelAktion = BEIM_NAECHSTEN_MAL;
				wiederholen[lok] = FALSE;
				sendNachricht(ABKUPPEL_VERSUCH);
			}
			break;
			// Kein gueltiger Zustand, Fehler ans Auditing melden.
		default:
			sendNachricht(FEHLER_ZUSTAND);
			break;
		}
		// Meldung beim Software-Watchdog machen
		/* TEST helloModul(MODULE_ID,wdStatus); */
	}
}