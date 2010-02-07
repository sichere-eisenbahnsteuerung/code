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
 *        Modul:        Auditing-System, Version 0.4
 *
 *        Beschreibung:
 *        Das Auditing-System uebermittelt alle gelieferten
 *        Statusmeldungen, ausgewaehlter Module, chronologisch per
 *        I2C-Buss an den Mikrocontroller "Arduino Duemilanove".
 *        Statusmeldungen werden vor ihrer Versendung in einem Ringpuffer
 *        zwischengespeichert. Bei zugeteilter Zeitscheibe durch die
 *        Betriebsmittelverwaltung werden alle gesammelten Meldungen
 *        verschickt.
 *
 *        Diese Headerdatei ist fuer die Module Leitzentrale,
 *        Befehlsvalidierung und Ergebnisvalidierung vorgesehen. Sie
 *        enthaellt den Prototypen fuer die Schnittstelle sendMsg.
 *
 ****************************************************************************/




/* Globale Makrodefinitionen ************************************************/

/* Globale Typdefinitionen **************************************************/

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

/* Deklaration von globalen Funktionen **************************************/

/*
 * sendMsg(byte module_id, byte msg[6])
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

	byte msg[6] /*
	*
	*  Description: Statusmeldung eines Moduls.
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
