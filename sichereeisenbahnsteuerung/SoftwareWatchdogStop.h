#ifndef SOFTWAREWATCHDOGSTOP_H
#define SOFTWAREWATCHDOGSTOP_H
/*****************************************************************************
 *
 *        Dateiname:    SoftwareWatchdogStop.h
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Felix Theodor Blueml
 *
 *
 *        Modul:        Software Watchdog, Version 1.6
 *
 *        Beschreibung:
 *        Der Software-Watchdog erkennt Deadlocks im Programmablauf. Bei
 *        Erkennung wird mit Hilfe des Not-Aus-Treibers das System
 *        angehalten.
 *        Deadlocks werden erkannt durch:
 *        - Zu viel verstrichene Zeit zwischen Rueckmeldungen
 *        - Pruefung auf gleiche Statusmeldungen von Modulen
 *
 *        Diese Headerdatei ist fuer das Modul Not-Aus-Treiber
 *        vorgesehen. Sie enthaellt den Prototypen fuer die
 *        Schnittstelle stopSW.
 *
 ****************************************************************************/




/* Globale Makrodefinitionen ************************************************/

/* Globale Typdefinitionen **************************************************/

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

/* Deklaration von globalen Funktionen **************************************/

/*
 * stopSW()
 * Schnittstelle nur fuer das Modul Not-Aus-Treiber. Die Schnittstelle dient 
 * zum abschalten des Hardwarezaehlers.
 *
 * Rueckgabe: Keine
 */
void stopSW(void);


#endif /* SOFTWAREWATCHDOGSTOP_H */
