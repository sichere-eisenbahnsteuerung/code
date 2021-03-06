#ifndef AUDITINGSYSTEMSENDMSG_H
#define AUDITINGSYSTEMSENDMSG_H
/*****************************************************************************
 *
 *        Dateiname:    AuditingSystemSendMsg.h
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Felix Theodor Blueml
 *
 *
 *        Modul:        Auditing-System, Version 0.6
 *
 *        Beschreibung:
 *        Das Auditing-System uebermittelt alle gelieferten
 *        Statusmeldungen, ausgewaehlter Module, chronologisch per
 *        I2C-Buss an den Mikrocontroller "Arduino Duemilanove".
 *        Statusmeldungen werden vor ihrer Versendung in einem Ringpuffer
 *        zwischengespeichert. Bei zugeteilter Zeitscheibe durch die
 *        Betriebsmittelverwaltung werden max. vier gesammelte Meldungen
 *        verschickt. Bei erkanntem Versagen des Systems, gibt das Modul
 *        Not-Aus-Treiber das Kommando alle gesammelten Meldungen zu
 *        verschicken.
 *
 *        Diese Headerdatei ist fuer die Module Leitzentrale,
 *        Befehlsvalidierung und Ergebnisvalidierung vorgesehen. Sie
 *        enthaellt den Prototypen fuer die Schnittstelle sendMsg.
 *
 ****************************************************************************/




/* Globale Makrodefinitionen ************************************************/
#ifndef MODUL_LZ
#define MODUL_LZ 0
#endif

#ifndef MODUL_BV
#define MODUL_BV 1
#endif

#ifndef MODUL_EV
#define MODUL_EV 2
#endif	

/* Globale Typdefinitionen **************************************************/

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

/* Deklaration von globalen Funktionen **************************************/

/*
 * sendMsg(byte module_id, const byte* msg)
 * Schnittstelle mit Statusmeldungen fuer Module mit zugeordneter Modulnummer.
 * Die Schnittstelle dient der Aufnahme aktueller Statusmeldungen der Module.
 * 
 * Rueckgabe: Keine
 */
void sendMsg(
	byte module_id, /*
	*
	*  Description: Vergebene Nummer zu einem der folgenden Module:
	*               0 = Leitzentrale
	*               1 = Befehlsvalidierung
	*               2 = Ergebnisvalidierung
	*  Direction  : in
	*  Values     : 0-2		(Modulnummer)
	*/

	const byte* msg /*
	*
	*  Description: Array der Groesse 6, mit Statusmeldung eines Moduls.
	*  Direction  : in
	*  Values     : [0]: 0-255,
	*               [1]: 0-255,
	*               [2]: 0-255,
	*               [3]: 0-255,
	*               [4]: 0-255,
	*               [5]: 0-255 (Statusmeldung eines Moduls)
	*/  
);


#endif /* AUDITINGSYSTEMSENDMSG_H */
