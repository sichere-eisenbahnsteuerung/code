#ifndef LEITZENTRALE_H
#define LEITZENTRALE_H
/*****************************************************************************
 *
 *        Dateiname:    Leitzentrale.h
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

/* Globale Makrodefinitionen ************************************************/

/* Globale Typdefinitionen **************************************************/

/* Eine Fahranweisung besteht aus 2 Byte. Der Typ Gleisabschnitt wird in 
 * Befehlsvalidierung.h deklariert und ist ein Byte groß.
 * Der Fahrbefehl enthaelt den eigentlichen Fahrbefehl und die Lok fuer die
 * er gilt.
 */
typedef struct Fahranweisung_struct {
	byte fahrbefehl; /*
	 * Description      : Der Fahrbefehl, enthält zusätzlich als 
	 *		      letztes Bit die Lok, für die er gilt.
	 *
	 * Access Rights    : r (Leitzentrale) w (Fahrprogramm)
	 *
	 * Values           : Kodierung siehe Software-Design
	 */  
	byte gleisabschnittNr; /*
	 * Description      : Die Nummer des Gleisabschnitts, für den der 
	 *		      Fahrbefehl gilt.
	 *
	 * Access Rights    : r (Leitzentrale) w (Fahrprogramm)
	 *
	 * Values           : (1 - 9)
	 *                    Kodierung siehe Befehlsvalidierung-Moduldesign
	 */  
} Fahranweisung;

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

extern Gleisabschnitt BV_streckentopologie[]; /*
 *
 * Description      : Siehe Befehlsvalidierung.h
 *
 */  

extern byte BV_gleisBelegung[]; /*
 *
 * Description      : Siehe Befehlsvalidierung.h
 *
 */

extern byte BV_weichenBelegung[]; /*
 *
 * Description      : Siehe Befehlsvalidierung.h
 *
 */

extern byte BV_zugPosition[]; /*
 *
 * Description      : Siehe Befehlsvalidierung.h
 *
 */

extern Streckenbefehl LZ_BV_streckenbefehl; /*
 *
 * Description      : Siehe Betriebsmittelverwaltung.h
 *
 */
 
extern Sensordaten BV_LZ_sensordaten; /*
 *
 * Description      : Siehe Betriebsmittelverwaltung.h
 *
 */
 
extern byte BV_LZ_bestaetigung; /*
 *
 * Description      : Siehe Betriebsmittelverwaltung.h
 *
 */

/* Deklaration von globalen Funktionen **************************************/



#endif /* LEITZENTRALE_H */
