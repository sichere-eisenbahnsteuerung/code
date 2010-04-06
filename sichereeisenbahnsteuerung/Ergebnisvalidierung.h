#ifndef ERGEBNISVALIDIERUNG_H
#define ERGEBNISVALIDIERUNG_H
/*****************************************************************************
 *
 *        Dateiname:    Ergebnisvalidierung.h
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Philip Weber
 *
 *
 *        Modul:        Ergebnisvalidierung, 1.0
 *
 *        Beschreibung:
 *		Das Modul Ergebnisvalidierung dient der berprfung der von 
 *		der Anwendung erzeugten Streckenbefehle auf Korrektheit. 
 *		Hierzu werden diese ber die SSC-Schnittstelle zwischen den 
 *		Mikrocontrollern ausgetauscht und verglichen. Unterscheiden 
 *		sich diese voneinander, wird ein Not-Aus-Signal erzeugt und 
 *		eine Statusmeldung mit den abweichenden Werten an das 
 *		Auditing-System zur Protokollierung gesendet.
 *
 *
 ****************************************************************************/




/* Globale Makrodefinitionen ************************************************/

/* Globale Typdefinitionen **************************************************/

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

/* Deklaration von globalen Funktionen **************************************/
void initEV(void);
void workEV(void);


#endif /* ERGEBNISVALIDIERUNG_H */
