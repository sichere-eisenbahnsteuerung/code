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
 *        Schnittstellen initSW und hello. Ausserdem enthaellt sie die
 *        Startwerte fuer die Konfiguration von Timer0:
 *        SW_T0_STARTWERT_LO und SW_T0_STARTWERT_HI.
 *
 ****************************************************************************/




/* Globale Makrodefinitionen ************************************************/

// Zeitfensterkonfiguration des Hardwarezaehlers
#ifndef SW_ZEITFENSTER
	#define SW_ZEITFENSTER		2
#endif
/*
 * Groesse des Zeitfensters in 10 ms.
 */  

#ifndef SW_TAKTRATE_C515C
	#define SW_TAKTRATE_C515C	12000000
#endif
/*
 * Taktrate des Mikrocontrollers C515C in MHz.
 */  

#ifndef SW_TAKTZYKLEN_C515C
	#define SW_TAKTZYKLEN_C515C	(6)
#endif
/*
 * Zyklen pro Takt des Mikrocontrollers C515C.
 */  

#ifndef SW_TIMER0_STARTWERT
	#define SW_TIMER0_STARTWERT	(0xffff - ((SW_ZEITFENSTER * SW_TAKTRATE_C515C) / (SW_TAKTZYKLEN_C515C * 100)))
#endif
/*
 * Startwert fuer Timer0: 0x63bf.
 */  

#ifndef SW_T0_STARTWERT_LO
	#define SW_T0_STARTWERT_LO (SW_TIMER0_STARTWERT & 0x00ff)
#endif
/*
 * Startwert fuer Timer0 (unteres Bit): 0xbf.
 */  

#ifndef SW_T0_STARTWERT_HI
	#define SW_T0_STARTWERT_HI ((SW_TIMER0_STARTWERT & 0xff00) >> 8)
#endif
/*
 * Startwert fuer Timer0 (oberes Bit): 0x63.
 */  

/* Globale Typdefinitionen **************************************************/

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

/* Deklaration von globalen Funktionen **************************************/

void initSW(void);
void hello(void);

#endif /* SOFTWAREWATCHDOG_H */
