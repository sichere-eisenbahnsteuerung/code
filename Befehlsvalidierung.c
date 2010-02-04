/*****************************************************************************
 *
 *        Dateiname:    Befehlsvalidierung.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Vitali Voroth
 *
 *        Modul:        Befehlsvalidierung, 1.1
 *
 *        Beschreibung:	Prueft Streckenbefehle der Leitzentrale auf Gueltigkeit.
 *			Leitet Sensordaten vom S88-Treiber an die Leitzentrale.
 *			Macht ein Not-Aus bei kritischem Zustand auf Strecke.
 *
 ****************************************************************************/

/* Includes *****************************************************************/

#include "Betriebsmittelverwaltung.h"
#include "Befehlsvalidierung.h"
#include "SoftwareWatchdogHelloModul.h"
#include "AuditingSystemSendMsg.h"
#include "Notaus.h"
//#include <stdio.h>	// only for tests with printf

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

Gleisabschnitt BV_streckentopologie[BV_ANZAHL_GLEISABSCHNITTE + 1];
byte BV_gleisBelegung[BV_ANZAHL_GLEISABSCHNITTE + 1];
byte BV_weichenBelegung[BV_ANZAHL_WEICHEN + 1];
byte BV_zugPosition[BV_ANZAHL_ZUEGE];

/* Lokale Makros ************************************************************/
#ifndef BV_MODULE_ID
#define BV_MODULE_ID MODUL_BV
#endif
#ifndef BV_MAX_WAGGONS
#define BV_MAX_WAGGONS 4
#endif
#ifndef BV_MAX_KRITISCH
	// Nicht hoeher als 30 !
#define BV_MAX_KRITISCH 5
#endif

/* Lokale Typen *************************************************************/

typedef enum {	// Zustaende, die ans Auditing-System uebermittelt werden
	Z_RETURN_FROM_WORK = 0,
		// Lokale Funktionen, die Nachrichten ans AS schicken
	Z_FKT_CHECK_SENSOR_DATEN = 1,
	Z_FKT_SEND_SENSOR_DATEN = 2,
	Z_FKT_CHECK_STRECKEN_BEFEHL = 3,
	Z_FKT_SENSOR_NACHBARN = 4,
	Z_FKT_CHECK_KRITISCHER_ZUSTAND = 5
} Zustand;

typedef enum {	// Fehler, die ans Auditing-System uebermittelt werden
	F_KEIN_FEHLER = 0,
		// Allgemeine Fehler
	F_SENSORDATEN_FEHLERHAFT = 1,
	F_KRITISCHER_ZUSTAND_ZU_OFT = 2,
	F_STRECKENTOPOLOGIE_MANIPULIERT = 3,
	F_UNBEKANNTER_INTERNER_ZUSTAND = 4,
	
		// Sensordaten-Fehler
	F_SD_FEHLERBYTE_GESETZT = 8,
	F_SD_KEIN_ZUG_NEBEN_SENSOR = 9,
	F_SD_VON_LZ_NICHT_VERARBEITET = 10,
	F_SD_KEINE_ABSCHNITTE_DRAN = 11,
	
		// Streckenbefehl-Fehler
	F_SB_ENTKOPPLER_NR_FALSCH = 16,
	F_SB_WEICHEN_NR_FALSCH = 17,
	F_SB_ENTKOPPELN_BEI_VOLLGAS = 18,
	F_SB_BELEGTE_WEICHE_STELLEN = 19,
	F_SB_ANGEFAHRENE_WEICHE_STELLEN = 20,
		// Lokbefehl-Fehler
	F_SB_LB_MIT_VOLLGAS_AUF_BELEGTES_GLEIS = 21,
	F_SB_LB_WEICHE_ZUM_ZIEL_BELEGT = 22,
	F_SB_LB_WEICHE_ZUM_ZIEL_FALSCH = 23,
	
		// Kritischer-Zustand-Fehler
	F_KZ_VOLLGAS_RICHTUNG_BELEGTEN_ABSCHNITT = 32,
	F_KZ_ZUG_RICHTUNG_BENACHBARTEN_ZUG = 33,
	F_KZ_ZUG_RICHTUNG_FALSCH_GESTELLTER_WEICHE = 34,
	F_KZ_ZU_VIELE_WAGGONS_LOKS_AUF_ABSCHNITT = 35
} Fehler;

typedef enum {	// Detailliertheit der Debug-Ausgaben
	V_ERRORS = 1,
	V_NORMAL = 2,
	V_ALL = 3
} Verbosity;

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/

static byte BV_next_state = 0;
static byte BV_criticalStateCounter = 0;
static byte BV_nachricht[6] = {0, 0, 0, 0, 0, 0};
static byte BV_verbosity = V_ERRORS;

	/* Gleis-Topologie und -Zaehler */
static Gleisabschnitt streckentopologie[BV_ANZAHL_GLEISABSCHNITTE + 1];

static byte gleisBelegung[BV_ANZAHL_GLEISABSCHNITTE + 1] =
	{	0,0,
		3,	// Abschn. 2: Drei Gueterwaggons
		0,0,0,	
		3,	// Abschn. 7: Lok1 inkl. zwei Passagierwaggons
		1,	// Abschn. 8: Rangierlokomotive
		0
	};

static byte weichenBelegung[BV_ANZAHL_WEICHEN + 1] =
	{
		0,0,0,0	// Alle Weichen im Initialzustand frei
	};	

static byte weichenStellung[BV_ANZAHL_WEICHEN + 1] =
	{
		0,0,0,0	// Alle Weichen im Initialzustand nach links
	};	

static byte zugPosition[BV_ANZAHL_ZUEGE] = // #0 = Lok1, #1 = Lok2
	{
		7,8
	};
static byte zugGeschwindigkeit[BV_ANZAHL_ZUEGE] =
	{
		0,0
	};
	
static byte zugRichtung[BV_ANZAHL_ZUEGE] =
	{
		1,1
	};

/* Prototypen fuer lokale Funktionen ****************************************/

/* checkStreckenTopologie()
 * Vergleicht die originale Streckentopologie mit der globalen Kopie,
 * die der Leitzentrale zur Verfügung gestellt wird.
 * Gibt TRUE zurück, wenn Kopie nicht vom Original abweicht, sonst FALSE.
 */
static boolean checkStreckenTopologie(void);

/* checkSensorDaten()
 * Überprüft die Sensordaten des S88-Treibers und aktualisiert
 * das Streckenabbild entsprechend, wenn sie richtig sind.
 * Sensordaten müssen vorher im Shared Memory zwischen S88-Treiber und BV sein.
 * Gibt TRUE zurück, wenn Sensordaten gültig und konsistent sind, sonst FALSE.
 */
static boolean checkSensorDaten(void);

/* sendSensorDaten()
 * Leitet die S88-Sensordaten an die Leitzentrale weiter (schreibt sie
 * in den Shared Memory zwischen Befehlsvalidierung und Leitzentrale).
 * Gültige Sensordaten müssen vorher im SM zwischen S88-Treiber und BV sein.
 * Gibt TRUE zurück, wenn Sensordaten im Shared Memory
 * zwischen Befehlsvalidierung und Leitzentrale leer waren, sonst FALSE.
 * Der Shared Memory zwischen S88-Treiber und Befehlsvalidierung wird geleert.
 */
static boolean sendSensorDaten(void);

/* checkStreckenBefehl()
 * Überprüft den Streckenbefehl der Leitzentrale zunächst auf syntaktische
 * Korrektheit und dann darauf, dass er keinen unsicheren Zustand auslöst.
 * Streckenbefehl muss vorher im Shared Memory zwischen LZ und BV sein.
 * Gibt TRUE zurück, wenn der Streckenbefehl gültig und sicher ist, sonst FALSE.
 */
static boolean checkStreckenBefehl(void);

/* sendStreckenBefehl()
 * Sendet den Streckenbefehl der Leitzentrale an die Ergebnisvalidierung.
 * Ein gültiger und sicherer Streckenbefehl muss vorher im Shared Memory
 * zwischen Leitzentrale und Befehlsvalidierung sein
 * Der Streckenbefehl im Shared Memory zwischen LZ und BV wird geleert
 * und die Bestätigung = 1 gesetzt.
 * Der Streckenbefehl wird in den Shared Memory zwischen Befehlsvalidierung
 * und Ergebnisvalidierung geschrieben, vorhandene alte werden überschrieben.
 */
static void sendStreckenBefehl(void);

// Hilfsfunktionen
static void copyStreckenBelegung(void);
static void defineStreckenTopologie(void);
static boolean zugNebenSensor(byte sensorNr, byte *zugNr, byte *richtung);
static boolean sensorNachbarn(byte sensorNr, byte *nextAbs, byte *prevAbs,
					byte *nSwitch, byte *pSwitch);
static boolean checkKritischerZustand(void);
static boolean weicheRichtig(byte zugPos, byte richtung, byte ziel, byte weiche);
static boolean zugFaehrtAufWeicheZu(byte weicheNr);
static void zielGleisUndWeiche(byte zugPos, byte richtung, byte *ziel, byte *weiche);
static void sendNachricht(Zustand zustand, Fehler fehler);

/* Funktionsimplementierungen ***********************************************/

/* Globale Funktionen .. */
void initBV(void)
{
	defineStreckenTopologie();	// intern
	copyStreckenBelegung();		// fuer die LZ
	
	// Das ist zum Testen, wenn echte Daten kommen, kommt das weg.
	S88_BV_sensordaten.Byte0 = 0;
	S88_BV_sensordaten.Byte1 = 12;
}

void workBV(void)
{
	// Verknuepft next_state und critical_state_counter
	byte concatState = 0; 
	
	switch (BV_next_state)
	{
	case 0:
		// Wenn keine neuen Sensordaten eingetroffen, gibt's nix zu tun
		if (	(S88_BV_sensordaten.Byte0) == LEER && 
			(S88_BV_sensordaten.Byte1) == LEER)
		{
			BV_next_state = 2;	// Streckenbefehl holen
			break;
		}
		
		// Wenn Sensordaten nicht in Orndung, Notaus generieren
		if (	(checkSensorDaten() == FALSE) || 
			(sendSensorDaten() == FALSE) )
		{
			sendNachricht(Z_RETURN_FROM_WORK, 
					F_SENSORDATEN_FEHLERHAFT);
			emergency_off();
			break;
		}
		
		// Wenn neue Sensordaten in Ordndung
		else 
		{
			BV_next_state = 1;	// Gleisbild pruefen
			break;
		}
		
	case 1:
		// Gleisbild auf kritische Zustaende pruefen
		if (checkKritischerZustand() == FALSE)
		{
			if (++BV_criticalStateCounter > BV_MAX_KRITISCH)
			{
				sendNachricht(Z_RETURN_FROM_WORK, 
						F_KRITISCHER_ZUSTAND_ZU_OFT);
				emergency_off();
			}
			break;
		}
		else
		{
			// keine kritischen Zustaende erkannt
			BV_criticalStateCounter = 0;
			BV_next_state = 2;	// Streckenbefehl holen
			break;
		}
				
	case 2:
		// Streckenbefehl ueberpruefen und (wenn OK) an EV senden
		if (checkStreckenBefehl() == TRUE)
		{
			byte zugNr = LZ_BV_streckenbefehl.Lok & 1;
			byte richtung = (LZ_BV_streckenbefehl.Lok & 2) >> 1;
			byte geschw = (LZ_BV_streckenbefehl.Lok & 252) >> 2;
			
			zugRichtung[zugNr] = richtung;
			zugGeschwindigkeit[zugNr] = geschw;
			sendStreckenBefehl();
		}
		
		BV_next_state = 0;		// Sensordaten holen
		break;
		
	case 3:
		// Wenn Kopien der Streckentopologie manipuliert wurden
		if (checkStreckenTopologie() == FALSE)
		{
			sendNachricht(Z_RETURN_FROM_WORK,
					F_STRECKENTOPOLOGIE_MANIPULIERT);
			emergency_off();
			break;
		}
		else
		{
			BV_next_state = 2;	// Streckenbefehl holen
			break;
		}
	default: 
		sendNachricht(Z_RETURN_FROM_WORK,
				F_UNBEKANNTER_INTERNER_ZUSTAND);
		emergency_off();	// anderen Zustand darf es nicht geben.
	}
	
	if (BV_verbosity >= V_NORMAL)
	{
		sendNachricht(Z_RETURN_FROM_WORK, F_KEIN_FEHLER);
	}

	concatState = (BV_next_state) & (BV_criticalStateCounter << 3);
	helloModul(BV_MODULE_ID, concatState);
}

/* Im Moduldesign beschriebene lokale Funktionen .. */
static boolean checkStreckenTopologie(void)
{
	byte z = 1;
	bit ret = TRUE;
	
	// Variablennamen kuerzen ... :/
	Gleisabschnitt *my = streckentopologie;
	Gleisabschnitt *his = BV_streckentopologie;
	
	for (z = 1; z < BV_ANZAHL_GLEISABSCHNITTE; z++)
	{
		ret &= (his[z].nr == my[z].nr);
		ret &= (his[z].next1 == my[z].next1);
		ret &= (his[z].next2 == my[z].next2);
		ret &= (his[z].prev1 == my[z].prev1);
		ret &= (his[z].prev2 == my[z].prev2);
		ret &= (his[z].nextSwitch == my[z].nextSwitch);
		ret &= (his[z].prevSwitch == my[z].prevSwitch);
		ret &= (his[z].nextSensor == my[z].nextSensor);
		ret &= (his[z].prevSensor == my[z].prevSensor);
		if (ret == FALSE) return FALSE;
	}
	
	for (z = 1; z < BV_ANZAHL_GLEISABSCHNITTE; z++)
	{
		ret &= (BV_gleisBelegung[z] == gleisBelegung[z]);
	}
	if (ret == FALSE) return FALSE;
	
	for (z = 1; z < BV_ANZAHL_WEICHEN; z++)
	{
		ret &= (BV_weichenBelegung[z] == weichenBelegung[z]);
	}
	if (ret == FALSE) return FALSE;
	
	for (z = 1; z < BV_ANZAHL_ZUEGE; z++)
	{
		ret &= (BV_zugPosition[z] == zugPosition[z]);
	}
	return ret;
}

static boolean checkSensorDaten(void)
{
	byte z, inkr, mask;
	byte sensoren[16];
	
	// wenn Fehler-Byte gesetzt ist, FALSE zurueckgeben
	if (S88_BV_sensordaten.Fehler != 0)
	{
		sendNachricht(Z_FKT_CHECK_SENSOR_DATEN,
				F_SD_FEHLERBYTE_GESETZT);
		return FALSE;
	}
	
	// wenn keine neuen Sensordaten vorhanden, Funktion verlassen
	if (S88_BV_sensordaten.Byte0 == LEER && S88_BV_sensordaten.Byte1 == LEER)
	{
		return TRUE;
	}
	
	// sonst Streckenabbild aktualisieren
	
		// einzelne Sensoren aus den beiden Bytes des Sensors holen
	mask = 1;
	for (z = 0; z < 8; z++)
	{
		sensoren[z] 	= ((S88_BV_sensordaten.Byte0 & mask) != 0);
		sensoren[z+8]	= ((S88_BV_sensordaten.Byte1 & mask) != 0);
		mask = mask << 1;
	}
	
		// Nun die Einzelnen Sensoren anschauen
	for (z = 1; z <= 16; z++)
	{
		byte nextAbs, prevAbs, nSwitch, pSwitch, zugNr, richtung, pos;
		
		// Wenn Sensor nicht belegt, naechsten Schleifendurchlauf.
		if (sensoren[z-1] == 0) continue; // weniger Verschachtelung
		
		// sonst: Sensor belegt, es gibt was zu tun..

		// Ist eigentlich ein Zug bei diesem Sensor? Welcher? Richtung?
		if (zugNebenSensor(z, &zugNr, &richtung) == FALSE)
		{
			sendNachricht(Z_FKT_CHECK_SENSOR_DATEN,
					F_SD_KEIN_ZUG_NEBEN_SENSOR);
			return FALSE;
		}
		
		// Suche die Nachbar-Abschnitte und -Weichen des Sensors
		sensorNachbarn(z, &nextAbs, &prevAbs, &nSwitch, &pSwitch);
		
		// Fahrtrichtung des Zuges? 0 = rueckwaerts, 1 = vorwaerts
		inkr = (richtung == 1) ? 1 : -1;
		
		if (nextAbs != 0) gleisBelegung[nextAbs] += inkr;
		if (prevAbs != 0) gleisBelegung[prevAbs] -= inkr;
		if (nSwitch != 0) weichenBelegung[nSwitch] += inkr;
		if (pSwitch != 0) weichenBelegung[pSwitch] -= inkr;
		
		// Wenn Sensor neben einer Weiche, folgenden Abschnitt bestimmen
		if (nextAbs == 0) 
		{
			nextAbs = streckentopologie[prevAbs].next1;
			
			// Mehr als ein Nachfolger? Weiche anschauen!
			if (	(streckentopologie[prevAbs].next2 != 0) &&
				(weichenStellung[nSwitch] == 1)	)
			{
				nextAbs = streckentopologie[prevAbs].next2;
			}
		}

		if (prevAbs == 0) 
		{
			prevAbs = streckentopologie[nextAbs].prev1;
			
			// Mehr als ein Nachfolger? Weiche anschauen!
			if (	(streckentopologie[nextAbs].prev2 != 0) &&
				(weichenStellung[pSwitch] == 1)	)
			{
				prevAbs = streckentopologie[nextAbs].prev2;
			}
		}
				
		// Position des Zuges aendern?
		pos = zugPosition[zugNr];			// alte Position
		if ( (pos == prevAbs) && (richtung == 1) )	// vorwaerts
		{
			zugPosition[zugNr] = nextAbs;
		}
		else if ( ( pos == nextAbs) && (richtung == 0) )// rueckwaerts
		{
			zugPosition[zugNr] = prevAbs;
		}
	}
	
	return TRUE;
}

static boolean sendSensorDaten(void)
{
	// wenn keine neuen Sensordaten vorhanden, nichts zu tun
	if (S88_BV_sensordaten.Byte0 == LEER && S88_BV_sensordaten.Byte1 == LEER)
	{
		return TRUE;
	}
	
	// wenn die LZ die alten Sensordaten noch nicht verarbeitet hat, Fehler
	if (BV_LZ_sensordaten.Byte0 != LEER || BV_LZ_sensordaten.Byte1 != LEER)
	{
		sendNachricht(Z_FKT_SEND_SENSOR_DATEN,
				F_SD_VON_LZ_NICHT_VERARBEITET);
		return FALSE;
	}
	
	// sonst Sensordaten an die Leitzentrale weiterleiten
	BV_LZ_sensordaten = S88_BV_sensordaten;
	return TRUE;
}

static boolean checkStreckenBefehl(void)
{
	byte weicheNr, entkopplerNr;
	// Wenn Befehl leer ist, nichts weiter pruefen
	if (	(LZ_BV_streckenbefehl.Entkoppler == LEER) &&
		(LZ_BV_streckenbefehl.Weiche == LEER) &&
		(LZ_BV_streckenbefehl.Lok == LEER) )
	{			
		return FALSE;
	}

	// --------------- Syntax-Checks --------------- 
	
	// beim Lok-Befehl ist jede Byte-Belegung gueltig.
		
	// beim Entkoppler-Befehl pruefen, ob die Entkoppler-Nr gueltig ist.
	if (LZ_BV_streckenbefehl.Entkoppler != LEER)
	{
		entkopplerNr = (LZ_BV_streckenbefehl.Entkoppler & 254) >> 1;
		if (entkopplerNr > BV_ANZAHL_ENTKOPPLER)
		{
			sendNachricht(Z_FKT_CHECK_STRECKEN_BEFEHL,
					F_SB_ENTKOPPLER_NR_FALSCH);
			return FALSE;
		}
	}
	
	// beim Weichen-Befehl pruefen, ob die WeichenNr gueltig ist.
	if (LZ_BV_streckenbefehl.Weiche != LEER)
	{
		weicheNr = (LZ_BV_streckenbefehl.Weiche & 254) >> 1;
		if (weicheNr > BV_ANZAHL_WEICHEN)
		{
			sendNachricht(Z_FKT_CHECK_STRECKEN_BEFEHL,
					F_SB_WEICHEN_NR_FALSCH);
			return FALSE;
		}
	}
	
	// --- Pruefung, ob unsicherer Zustand eintreten wuerde ---
	
	// Entkoppeln nur bei entsprechender Geschwindigkeit erlaubt
	if (LZ_BV_streckenbefehl.Entkoppler != LEER)
	{
		boolean fehler = FALSE;
		byte v1 = zugGeschwindigkeit[0];
		byte v2 = zugGeschwindigkeit[1];
		byte zug1 = zugPosition[0];
		byte zug2 = zugPosition[1];
		
		switch (entkopplerNr)
		{
		case 1:
			if ( (zug1 == 2 || zug1 == 3) && (v1 != BV_V_ABKUPPELN) )
			{
				fehler = TRUE;
			}
			if ( (zug2 == 2 || zug2 == 3) && (v2 != BV_V_ABKUPPELN) )
			{
				fehler = TRUE;
			}
			break;
		case 2:
			if ( (zug1 == 8 || zug1 == 9) && (v1 != BV_V_ABKUPPELN) )
			{
				fehler = TRUE;
			}
			if ( (zug2 == 8 || zug2 == 9) && (v2 != BV_V_ABKUPPELN) )
			{
				fehler = TRUE;
			}
			break;
		}
		
		if (fehler == TRUE)
		{
			sendNachricht(Z_FKT_CHECK_STRECKEN_BEFEHL,
					F_SB_ENTKOPPELN_BEI_VOLLGAS);
			return FALSE;
		}
		
		return TRUE;
	}
	
	if (LZ_BV_streckenbefehl.Weiche != LEER)
	{
		byte wStell = (LZ_BV_streckenbefehl.Weiche & 1);
		
		// Weiche darf nur gestellt werden, wenn sie nicht belegt ist
		if ( weichenBelegung[weicheNr] != 0 )
		{
			sendNachricht(Z_FKT_CHECK_STRECKEN_BEFEHL,
					F_SB_BELEGTE_WEICHE_STELLEN);
			return FALSE;
		}
		
		// Nur stellen, wenn kein anderer Zug auf diese Weiche zufaehrt
		if (zugFaehrtAufWeicheZu(weicheNr) 
			&& weichenBelegung[weicheNr] != wStell)
		{
			sendNachricht(Z_FKT_CHECK_STRECKEN_BEFEHL,
					F_SB_ANGEFAHRENE_WEICHE_STELLEN);
			return FALSE;
		}
	}
	
	// Pruefen ob der Lok-Befehl einen unsicheren Zustand hervorruft
	if (LZ_BV_streckenbefehl.Lok != LEER)
	{
		byte zugNr = LZ_BV_streckenbefehl.Lok & 1;
		byte richtung = (LZ_BV_streckenbefehl.Lok & 2) >> 1;
		byte geschw = (LZ_BV_streckenbefehl.Lok & 252) >> 2;
		byte zugPos = zugPosition[zugNr];
		byte ziel, weiche;
		
		// Zielgleis und etwaige Weiche auf dem Weg dahin bestimmen
		zielGleisUndWeiche(zugPos, richtung, &ziel, &weiche);
		
		// Wenn Zielgleis belegt, darf man nur lansgsam reinfahren
		if ( (gleisBelegung[ziel] != 0) && (geschw != BV_V_ANKUPPELN) )
		{
			sendNachricht(Z_FKT_CHECK_STRECKEN_BEFEHL,
					F_SB_LB_MIT_VOLLGAS_AUF_BELEGTES_GLEIS);
			return FALSE;
		}
		
		// Egal ob Zielgleis belegt: die Weiche dahin muss frei sein!
		if ( (weiche != 0) && (weichenBelegung[weiche] != 0) )
		{
			sendNachricht(Z_FKT_CHECK_STRECKEN_BEFEHL,
					F_SB_LB_WEICHE_ZUM_ZIEL_BELEGT);
			return FALSE;
		}
		
		// Weichenstellung pruefen, wenn man von hinten kommt.
		if (weicheRichtig(zugPos, richtung, ziel, weiche))
		{
			sendNachricht(Z_FKT_CHECK_STRECKEN_BEFEHL,
					F_SB_LB_WEICHE_ZUM_ZIEL_FALSCH);
			return FALSE;
		}
	}

	return TRUE;
}

static void sendStreckenBefehl(void)
{
	// wenn neuer Streckenbefehl vorhanden ist, (Shared Memory nicht leer)
	if (	(LZ_BV_streckenbefehl.Entkoppler != LEER) ||
		(LZ_BV_streckenbefehl.Weiche != LEER) ||
		(LZ_BV_streckenbefehl.Lok != LEER) )
	{
		// Streckenbefehl an Ergebnisvalidierung weiterleiten
		BV_EV_streckenbefehl = LZ_BV_streckenbefehl;
		
		// Shared Memory von der Leitzentrale leeren
		LZ_BV_streckenbefehl.Entkoppler = LEER;
		LZ_BV_streckenbefehl.Weiche = LEER;
		LZ_BV_streckenbefehl.Lok = LEER;
		BV_LZ_bestaetigung = 1;
	}
}


/* Weitere Hilfsfunktionen .. */
static void copyStreckenBelegung(void)
{
	byte z = 0;
	
	for (z = 1; z < BV_ANZAHL_GLEISABSCHNITTE; z++)
	{
		BV_gleisBelegung[z] = gleisBelegung[z];
	}
	
	for (z = 1; z < BV_ANZAHL_WEICHEN; z++)
	{
		BV_weichenBelegung[z] = weichenBelegung[z];
	}
	
	for (z = 1; z < BV_ANZAHL_ZUEGE; z++)
	{
		BV_zugPosition[z] = zugPosition[z];
	}
}

static void defineStreckenTopologie(void)
{
	byte z;
	
	// TODO: Let somebody review this!
	// Gleisabschnitt 1
	streckentopologie[1].nr = 1;
	streckentopologie[1].next1 = 2;
	streckentopologie[1].next2 = 7;
	streckentopologie[1].prev1 = 6;
	streckentopologie[1].prev2 = 8;
	streckentopologie[1].nextSwitch = 1;
	streckentopologie[1].prevSwitch = 3;
	streckentopologie[1].nextSensor = 1;
	streckentopologie[1].prevSensor = 9;
	
	// Gleisabschnitt 2
	streckentopologie[2].nr = 2;
	streckentopologie[2].next1 = 3;
	streckentopologie[2].next2 = 0;
	streckentopologie[2].prev1 = 1;
	streckentopologie[2].prev2 = 0;
	streckentopologie[2].nextSwitch = 0;
	streckentopologie[2].prevSwitch = 1;
	streckentopologie[2].nextSensor = 3;
	streckentopologie[2].prevSensor = 2;
	
	// Gleisabschnitt 3
	streckentopologie[3].nr = 3;
	streckentopologie[3].next1 = 4;
	streckentopologie[3].next2 = 0;
	streckentopologie[3].prev1 = 2;
	streckentopologie[3].prev2 = 0;
	streckentopologie[3].nextSwitch = 2;
	streckentopologie[3].prevSwitch = 0;
	streckentopologie[3].nextSensor = 4;
	streckentopologie[3].prevSensor = 3;
	
	// Gleisabschnitt 4
	streckentopologie[4].nr = 4;
	streckentopologie[4].next1 = 5;
	streckentopologie[4].next2 = 0;
	streckentopologie[4].prev1 = 7;
	streckentopologie[4].prev2 = 3;
	streckentopologie[4].nextSwitch = 0;
	streckentopologie[4].prevSwitch = 2;
	streckentopologie[4].nextSensor = 6;
	streckentopologie[4].prevSensor = 5;
	
	// Gleisabschnitt 5
	streckentopologie[5].nr = 5;
	streckentopologie[5].next1 = 6;
	streckentopologie[5].next2 = 0;
	streckentopologie[5].prev1 = 4;
	streckentopologie[5].prev2 = 0;
	streckentopologie[5].nextSwitch = 0;
	streckentopologie[5].prevSwitch = 0;
	streckentopologie[5].nextSensor = 7;
	streckentopologie[5].prevSensor = 6;
	
	// Gleisabschnitt 6
	streckentopologie[6].nr = 6;
	streckentopologie[6].next1 = 1;
	streckentopologie[6].next2 = 0;
	streckentopologie[6].prev1 = 5;
	streckentopologie[6].prev2 = 0;
	streckentopologie[6].nextSwitch = 3;
	streckentopologie[6].prevSwitch = 0;
	streckentopologie[6].nextSensor = 8;
	streckentopologie[6].prevSensor = 7;
	
	// Gleisabschnitt 7
	streckentopologie[7].nr = 7;
	streckentopologie[7].next1 = 4;
	streckentopologie[7].next2 = 0;
	streckentopologie[7].prev1 = 1;
	streckentopologie[7].prev2 = 0;
	streckentopologie[7].nextSwitch = 2;
	streckentopologie[7].prevSwitch = 1;
	streckentopologie[7].nextSensor = 11;
	streckentopologie[7].prevSensor = 10;
	
	// Gleisabschnitt 8
	streckentopologie[8].nr = 8;
	streckentopologie[8].next1 = 1;
	streckentopologie[8].next2 = 0;
	streckentopologie[8].prev1 = 9;
	streckentopologie[8].prev2 = 0;
	streckentopologie[8].nextSwitch = 3;
	streckentopologie[8].prevSwitch = 0;
	streckentopologie[8].nextSensor = 12;
	streckentopologie[8].prevSensor = 13;
	
	// Gleisabschnitt 9
	streckentopologie[9].nr = 9;
	streckentopologie[9].next1 = 8;
	streckentopologie[9].next2 = 0;
	streckentopologie[9].prev1 = 0;
	streckentopologie[9].prev2 = 0;
	streckentopologie[9].nextSwitch = 0;
	streckentopologie[9].prevSwitch = 0;
	streckentopologie[9].nextSensor = 13;
	streckentopologie[9].prevSensor = 14;
	
	// Kopie für die LZ erstellen
	for (z = 1; z < BV_ANZAHL_GLEISABSCHNITTE; z++)
	{
		BV_streckentopologie[z] = streckentopologie[z];
	}
}

static boolean zugNebenSensor(byte sensorNr, byte *zugNr, byte *richtung)
{
	byte zug1 = zugPosition[0];
	byte zug2 = zugPosition[1];
	
	// Maximal drei Nachbarabschnitte eines Sensors
	byte pos1 = 0;
	byte pos2 = 0;
	byte pos3 = 0;
	
	switch(sensorNr) 
	{
	case 1:
		pos1 = 1;	pos2 = 2;	pos3 = 7;	break;
	case 2:
		pos1 = 1;	pos2 = 2;	pos3 = 0;	break;
	case 3:
		pos1 = 2;	pos2 = 3;	pos3 = 0;	break;
	case 4:
		pos1 = 3;	pos2 = 4;	pos3 = 0;	break;
	case 5:
		pos1 = 3;	pos2 = 4;	pos3 = 7;	break;
	case 6:
		pos1 = 4;	pos2 = 5;	pos3 = 0;	break;
	case 7:
		pos1 = 5;	pos2 = 6;	pos3 = 0;	break;
	case 8:
		pos1 = 6;	pos2 = 1;	pos3 = 0;	break;
	case 9:	
		pos1 = 6;	pos2 = 1;	pos3 = 8;	break;
	case 10:
		pos1 = 1;	pos2 = 7;	pos3 = 0;	break;
	case 11:
		pos1 = 7;	pos2 = 4;	pos3 = 0;	break;
	case 12:
		pos1 = 8;	pos2 = 1;	pos3 = 0;	break;
	case 13:	
		pos1 = 9;	pos2 = 8;	pos3 = 0;	break;
	case 14:	
		pos1 = 9;	pos2 = 0;	pos3 = 0;	break;
	default:
		pos1 = 0;	pos2 = 0;	pos3 = 0;	break;
	};
	
	//TODO: Was, wenn Zuege auf benachbarten Abschnitten (z.B. 1 und 2)
	if ( (zug1 == pos1) || (zug1 == pos2) || (zug1 == pos3) )
	{
		*zugNr = 0;
		*richtung = zugRichtung[0];
		return TRUE;
	}
	else if ( (zug2 == pos1) || (zug2 == pos2) || (zug2 == pos3) )
	{
		*zugNr = 1;
		*richtung = zugRichtung[1];
		return TRUE;
	}
	else
	{
		return FALSE;
	};
}

static boolean sensorNachbarn(byte sensorNr, byte *nextAbs, byte *prevAbs,
				byte *nSwitch, byte *pSwitch)
{
	byte z;
	*nextAbs = 0; *prevAbs = 0; *nSwitch = 0; *pSwitch = 0;
	
	// Streckentopologie nach dem Sensor durchsuchen..
	for (z = 1; z < BV_ANZAHL_GLEISABSCHNITTE; z++) {
		if (streckentopologie[z].nextSensor == sensorNr)
		{
			*prevAbs = z;
		}
		
		if (streckentopologie[z].prevSensor == sensorNr)
		{
			*nextAbs = z;
		}
	}
	
	// das sollte eigentlich nicht passieren
	if (*nextAbs == 0 && *prevAbs == 0)
	{
		sendNachricht(Z_FKT_SENSOR_NACHBARN,
				F_SD_KEINE_ABSCHNITTE_DRAN);
		return FALSE;
	}
	
	// wenn next oder prev gleich 0, liegt der Sensor neben einer Weiche
	if (*nextAbs == 0)
	{
		*nSwitch = streckentopologie[*prevAbs].nextSwitch;
	}
	
	if (*prevAbs == 0)
	{
		*pSwitch = streckentopologie[*nextAbs].prevSwitch;
	}
	
	return TRUE;
}

static boolean checkKritischerZustand(void)
{
	byte z;
	boolean kritisch = FALSE;
	
	for (z = 0; z < BV_ANZAHL_ZUEGE; z++)
	{
		byte zugPos = zugPosition[z];
		byte geschw = zugGeschwindigkeit[z];
		byte richtung = zugRichtung[z];
		byte ziel, weiche;
		
		// Zielgleis und Weiche dahin bestimmen
		zielGleisUndWeiche(zugPos, richtung, &ziel, &weiche);
		
		// Ein Zug faehrt mit Vollgas richtung eines belegten Abschnitts
		if ( (geschw > BV_V_ANKUPPELN) && (gleisBelegung[ziel] != 0) )
		{
			sendNachricht(Z_FKT_CHECK_KRITISCHER_ZUSTAND,
				F_KZ_VOLLGAS_RICHTUNG_BELEGTEN_ABSCHNITT);
			kritisch = TRUE;
			break;
		}
		
		// Zuege in benachbarten Abschnitten bewegen sich aufeinander zu
		if (ziel == zugPosition[ 1 - zugPos ])
		{
			// Ziel gleich der Position des anderen Zuges!
			sendNachricht(Z_FKT_CHECK_KRITISCHER_ZUSTAND,
				F_KZ_ZUG_RICHTUNG_BENACHBARTEN_ZUG);
			kritisch = TRUE;
			break;
		}
		
		// Ein Zug faehrt auf eine falsch gestellte Weiche zu
		if (weicheRichtig(zugPos, richtung, ziel, weiche))
		{
			sendNachricht(Z_FKT_CHECK_KRITISCHER_ZUSTAND,
				F_KZ_ZUG_RICHTUNG_FALSCH_GESTELLTER_WEICHE);
			kritisch = TRUE;
		}
	}
	
	// Zu viele Waggons und Loks auf einem Abschnitt
	for (z = 1; z < BV_ANZAHL_GLEISABSCHNITTE; z++) {
		if (gleisBelegung[z] > BV_MAX_WAGGONS)
		{
			sendNachricht(Z_FKT_CHECK_KRITISCHER_ZUSTAND,
				F_KZ_ZU_VIELE_WAGGONS_LOKS_AUF_ABSCHNITT);
			kritisch = TRUE;
			break;
		}
	}
		
	return kritisch;
}

static boolean weicheRichtig(byte zugPos, byte richtung, byte ziel, byte weiche)
{
	Gleisabschnitt gleis, zielGleis;
	byte wStell = weichenStellung[weiche];
	gleis = streckentopologie[zugPos];
	zielGleis = streckentopologie[ziel];
	
	// Wenn da gar keine Weiche ist, Pruefung abbrechen
	if (weiche == 0)
	{
		return TRUE;
	}
		
	if ( (richtung == 1) && (gleis.next2 == 0) )
	{
		if ( (zugPos == zielGleis.prev1) && (wStell == 1) )
		{
			return FALSE;
		}
		else if ( (zugPos == zielGleis.prev2) && (wStell == 0) )
		{
			return FALSE;
		}
	}
	else if ( (richtung == 0) && (gleis.prev2 == 0))
	{
		if ( (zugPos == zielGleis.next1) && (wStell == 1) )
		{
			return FALSE;
		}
		else if ( (zugPos == zielGleis.next2) && (wStell == 0) )
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

static boolean zugFaehrtAufWeicheZu(byte weicheNr)
{
	byte z, zug;
	for (zug = 0; z < BV_ANZAHL_ZUEGE; z++)
	{
		byte ziel, weiche;
		byte zugPos = zugPosition[zug];
		byte richtung = zugRichtung[zug];
		
		if (zugGeschwindigkeit[zug] == BV_V_STAND)
		{
			// Nur Zuege anschauen, die nicht stehen.
			continue;
		}

		zielGleisUndWeiche(zugPos, richtung, &ziel, &weiche);
		if ( weicheRichtig(zugPos, richtung, ziel, weiche)
			&& (weiche == weicheNr) )
		{
			return TRUE;
		}
	}
	return FALSE;
}

static void zielGleisUndWeiche(byte zugPos, byte richtung, byte *ziel, byte *weiche)
{
	Gleisabschnitt gleis;
	gleis = streckentopologie[zugPos];
	if (richtung == 1)
	{
		*ziel = gleis.next1;
		*weiche = gleis.nextSwitch;
		if ((gleis.next2 != 0) && (weichenStellung[*weiche] == 1))
		{
			*ziel = gleis.next2;
		}
	}

	if (richtung == 0)
	{
		*ziel = gleis.prev1;
		*weiche = gleis.prevSwitch;
		if ((gleis.prev2 != 0) && (weichenStellung[*weiche] == 1))
		{
			*ziel = gleis.prev2;
		}
	}
}

static void sendNachricht(Zustand zustand, Fehler fehler)
{
	BV_nachricht[0] = zustand;
	BV_nachricht[1] = fehler;
	BV_nachricht[2] = BV_next_state;
	BV_nachricht[3] = BV_criticalStateCounter;
	BV_nachricht[4] = zugPosition[0];
	BV_nachricht[5] = zugPosition[1];
	
	sendMsg(BV_MODULE_ID, BV_nachricht);
}