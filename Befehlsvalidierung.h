#ifndef BEFEHLSVALIDIERUNG_H
#define BEFEHLSVALIDIERUNG_H
/*****************************************************************************
 *
 *        Dateiname:    Befehlsvalidierung.h
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Vitali Voroth
 *
 *
 *        Modul:        Befehlsvalidierung, 1.0
 *
 *        Beschreibung:	Prüft Streckenbefehle der Leitzentrale auf Gültigkeit
 *			Leitet Sensordaten vom S88-Treiber an die Leitzentrale
 *			Macht ein Not-Aus bei kritischem Zustand auf Strecke
 *			Die Header-Datei definiert die Struktur Gleisabschnitt,
 *			die von der Leitzentrale verwendet wird.
 *
 ****************************************************************************/


/* Globale Makrodefinitionen ************************************************/

/* Globale Typdefinitionen **************************************************/
typedef struct Gleisabschnitt_struct 
{ 
	byte nr;
	byte next1;
	byte next2;
	byte prev1;
	byte prev2;
	byte nextSwitch;
	byte prevSwitch;
	byte nextSensor;
	byte prevSensor;

} Gleisabschnitt;

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

Gleisabschnitt BV_streckentopologie[];
byte BV_gleisBelegung[];
byte BV_weichenBelegung[];
byte BV_zugPosition[];

/* Deklaration von globalen Funktionen **************************************/

void initBV();
void workBV();

#endif /* BEFEHLSVALIDIERUNG_H */
