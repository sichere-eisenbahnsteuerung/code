/**
 * @file     Ergebnisvalidierung.h
 *
 * @author   Philip Weber
 *   
 * @brief    Ueberpruefung der Streckenbefehle auf Korrektheit.
 * 
 *		     Hierzu werden diese ueber die SSC-Schnittstelle zwischen den 
 *		     Mikrocontrollern ausgetauscht und verglichen. Unterscheiden 
 *		     sich diese voneinander, wird ein Not-Aus-Signal erzeugt und 
 *		     eine Statusmeldung mit den abweichenden Werten an das 
 *		     Auditing-System zur Protokollierung gesendet.
 *
 * @date     13.01.2010
 */


#ifndef ERGEBNISVALIDIERUNG_H
#define ERGEBNISVALIDIERUNG_H


/**
 * @brief   Initialisierung aller benoetigten Variablen des Moduls.
 *          @note: Vor der Funktion workEV() aufrufen.
 */
void initEV(void);

/**
 * @brief   Schnittstelle zum Modul bz. Startet die Ergebnisvalidierung.
 *
 *          Diese Funktion wird von der Betriebsmittelsteuerung 
 *          aufgerufen. Sie stellt die gesamte Funktionalitaet des
 *          Moduls Ergebnisvalidierung bereit. 
 */
void workEV(void);


#endif /* ERGEBNISVALIDIERUNG_H */
