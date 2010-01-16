#ifndef SOFTWAREWATCHDOGHELLOMODUL_H
#define SOFTWAREWATCHDOGHELLOMODUL_H
/*****************************************************************************
 *
 *        Dateiname:    SoftwareWatchdogHelloModul.h
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

void helloModul(
	byte module_id, /*
	*
	*  Description: Vergebene Nummer zu einem der folgenden Module:
	*               0 = Leitzentrale
	*               1 = Befehlsvalidierung
	*               2 = Ergebnisvalidierung
	*               3 = S88-Treiber 
	*               4 = SSC-Treiber
	*               5 = RS232-Treiber
	*               6 = Auditing System
	*  Direction  : in
	*  Values     : 0-6			(Modulnummer)
	*/  

	byte status /*
	*
	*  Description: Aktueller Status eines Moduls.
	*  Direction  : in
	*  Values     : 0-255		(Statusmeldung eines Moduls)
	* 				Kein Modul darf 255 als erste Statusmeldung zur Laufzeit
	*               melden.
	*/  
);



#endif /* SOFTWAREWATCHDOGHELLOMODUL_H */
