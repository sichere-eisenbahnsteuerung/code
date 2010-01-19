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
 *        Modul:        Befehlsvalidierung, 1.1
 *
 *        Beschreibung:	Prüft Streckenbefehle der Leitzentrale auf Gültigkeit
 *			Leitet Sensordaten vom S88-Treiber an die Leitzentrale
 *			Macht ein Not-Aus bei kritischem Zustand auf Strecke
 *			Die Header-Datei definiert die Struktur Gleisabschnitt,
 *			die von der Leitzentrale verwendet wird.
 *
 ****************************************************************************/


/* Globale Makrodefinitionen ************************************************/

#ifndef BV_ANZAHL_GLEISABSCHNITTE
#define BV_ANZAHL_GLEISABSCHNITTE 9
#endif
#ifndef BV_ANZAHL_ENTKOPPLER
#define BV_ANZAHL_ENTKOPPLER 2
#endif
#ifndef BV_ANZAHL_WEICHEN
#define BV_ANZAHL_WEICHEN 3
#endif
#ifndef BV_ANZAHL_ZUEGE
#define BV_ANZAHL_ZUEGE 2
#endif

// Geschwindigkeiten.. noch festzulegen und vielleicht wo anders definieren?
#ifndef BV_V_VOLLGAS
#define BV_V_VOLLGAS 62
#endif
#ifndef BV_V_ABKUPPELN
#define BV_V_ABKUPPELN 10
#endif
#ifndef BV_V_ANKUPPELN
#define BV_V_ANKUPPELN 20
#endif

#ifndef BV_MAX_WAGGONS
#define BV_MAX_WAGGONS 4
#endif
#ifndef BV_MAX_KRITISCH
#define BV_MAX_KRITISCH 5
#endif


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

extern Gleisabschnitt BV_streckentopologie[BV_ANZAHL_GLEISABSCHNITTE + 1];
extern byte BV_gleisBelegung[BV_ANZAHL_GLEISABSCHNITTE + 1];
extern byte BV_weichenBelegung[BV_ANZAHL_WEICHEN + 1];
extern byte BV_zugPosition[BV_ANZAHL_ZUEGE];

/* Deklaration von globalen Funktionen **************************************/

void initBV();
void workBV();

#endif /* BEFEHLSVALIDIERUNG_H */
