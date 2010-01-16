#ifndef SOFTWAREWATCHDOGSTOP_H
#define SOFTWAREWATCHDOGSTOP_H
/*****************************************************************************
 *
 *        Dateiname:    SoftwareWatchdogStop.h
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Felix Blueml
 *
 *
 *        Modul:        Software Watchdog, Version 1.2
 *
 *        Beschreibung:
 *        Der Software-Watchdog erkennt Deadlocks im Programmablauf. Bei
 *        Erkennung wird mit Hilfe des Not-Aus-Treibers das System
 *        angehalten.
 *        Deadlocks werden erkannt durch:
 *        - Zu viel verstrichene Zeit zwischen Rueckmeldungen
 *        - Pruefung auf gleiche Statusmeldungen von Modulen
 *
 ****************************************************************************/




/* Globale Makrodefinitionen ************************************************/

/* Globale Typdefinitionen **************************************************/

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

/* Deklaration von globalen Funktionen **************************************/

void stopSW();


#endif /* SOFTWAREWATCHDOGSTOP_H */
