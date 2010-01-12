/*****************************************************************************
 *
 *        Dateiname:    Befehlsvalidierung.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Vitali Voroth
 *
 *        Modul:        Befehlsvalidierung, 1.0
 *
 *        Beschreibung:	Prüft Streckenbefehle der Leitzentrale auf Gültigkeit
 *			Leitet Sensordaten vom S88-Treiber an die Leitzentrale
 *			Macht ein Not-Aus bei kritischem Zustand auf Strecke
 *
 ****************************************************************************/

/* Includes *****************************************************************/

#include "Betriebsmittelverwaltung.h"
#include "Befehlsvalidierung.h"
#include <stdio.h>

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

Gleisabschnitt BV_streckentopologie[BV_ANZAHL_GLEISABSCHNITTE + 1];
byte BV_gleisBelegung[BV_ANZAHL_GLEISABSCHNITTE + 1];
byte BV_weichenBelegung[BV_ANZAHL_WEICHEN + 1];
byte BV_zugPosition[BV_ANZAHL_ZUEGE];

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/

	/* Gleis-Topologie und -Zaehler */
static Gleisabschnitt streckentopologie[BV_ANZAHL_GLEISABSCHNITTE + 1];

static byte gleisBelegung[BV_ANZAHL_GLEISABSCHNITTE + 1] =
	{	0,0,
		3,	// Abschn. 2: Drei Güterwaggons
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

static byte zugPosition[BV_ANZAHL_ZUEGE +1] = 
	{
		0,7,8
	};
	
/* Prototypen fuer lokale Funktionen ****************************************/

// TODO write some docu for those
bit checkStreckenTopologie(void);
void copyStreckenTopologie(void);
void defineStreckenTopologie(void);

/* Funktionsimplementierungen ***********************************************/

byte b1, b2, b3;

void initBV(void)
{
	//TODO: remove temp test
	b1 = 10;
	b2 = 20;	
	
	defineStreckenTopologie();
	copyStreckenTopologie();
}

void workBV(void)
{
	b1 = ++gleisBelegung[1];
	b2 = --gleisBelegung[2];
	b3 = b1 + b2;
	printf("b3 = %d \n", (int)b3);
	checkStreckenTopologie();
}

bit checkStreckenTopologie(void)
{
	byte z = 1;
	bit ret = TRUE;
	
	// Variablen kürzen ... :/
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
	return TRUE;
}

void copyStreckenTopologie(void)
{
	byte z = 1;
	
	for (z = 1; z < BV_ANZAHL_GLEISABSCHNITTE; z++)
	{
		BV_streckentopologie[z].nr  = streckentopologie[z].nr;
		BV_streckentopologie[z].next1 = streckentopologie[z].next1;
		BV_streckentopologie[z].next2 = streckentopologie[z].next2;
		BV_streckentopologie[z].prev1 = streckentopologie[z].prev1;
		BV_streckentopologie[z].prev2 = streckentopologie[z].prev2;
		BV_streckentopologie[z].nextSwitch = streckentopologie[z].nextSwitch;
		BV_streckentopologie[z].prevSwitch = streckentopologie[z].prevSwitch;
		BV_streckentopologie[z].nextSensor = streckentopologie[z].nextSensor;
		BV_streckentopologie[z].prevSensor = streckentopologie[z].prevSensor;
	}
	
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
void defineStreckenTopologie(void)
{
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
}