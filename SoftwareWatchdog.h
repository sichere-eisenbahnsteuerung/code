#ifndef SOFTWAREWATCHDOG_H
#define SOFTWAREWATCHDOG_H
/*****************************************************************************
 *
 *        Dateiname:    SoftwareWatchdog.h
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
 *        Diese Headerdatei ist fuer das Modul Betriebsmittelverwaltung
 *        vorgesehen. Sie enthaellt die Prototypen fuer die
 *        Schnittstellen initSW und hello.
 *
 ****************************************************************************/




/* Globale Makrodefinitionen ************************************************/

// Zeitfensterkonfiguration des Hardwarezaehlers
#define SW_ZEITFENSTER		2
/*
 * Groesse des Zeitfensters in 10 ms.
 */  

#define SW_TAKTRATE_C515C	12000000
/*
 * Taktrate des Mikrocontrollers C515C in MHz.
 */  

#define SW_TAKTZYKLEN_C515C	(6)
/*
 * Zyklen pro Takt des Mikrocontrollers C515C.
 */  

#define SW_TIMER0_STARTWERT	(0xffff - ((SW_ZEITFENSTER * SW_TAKTRATE_C515C) / (SW_TAKTZYKLEN_C515C * 100)))
/*
 * Startwert fuer Timer0: 0x63bf.
 */  

#define SW_T0_STARTWERT_LO (SW_TIMER0_STARTWERT & 0x00ff)
/*
 * Startwert fuer Timer0 (unteres Bit): 0xbf.
 */  

#define SW_T0_STARTWERT_HI ((SW_TIMER0_STARTWERT & 0xff00) >> 8)
/*
 * Startwert fuer Timer0 (obrers Bit): 0x63.
 */  

/* Globale Typdefinitionen **************************************************/

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

/* Deklaration von globalen Funktionen **************************************/

void initSW();
void hello();

#endif /* SOFTWAREWATCHDOG_H */
