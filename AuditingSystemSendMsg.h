#ifndef AUDITINGSYSTEMSENDMSG_H
#define AUDITINGSYSTEMSENDMSG_H
/*****************************************************************************
 *
 *        Dateiname:    AuditingSystemSendMsg.h
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Felix Blueml
 *
 *
 *        Modul:        Auditing-System, Version 0.3
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
 *        enthaellt den Prototypen fuer die Schnittstelle send_msg.
 *
 ****************************************************************************/




/* Globale Makrodefinitionen ************************************************/

/* Globale Typdefinitionen **************************************************/

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

/* Deklaration von globalen Funktionen **************************************/

void send_msg(
	byte msg[6], /*
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

	byte module_id /*
	*
	*  Description: Vergebene Nummer zu einem der folgenden Module:
	*               0 = Leitzentrale
	*               1 = Befehlsvalidierung
	*               2 = Ergebnisvalidierung
	*  Direction  : in
	*  Values     : 0-2			(Modulnummer)
	*/  
);

// Uebernommene Funktionen aus der Datei I2C_SW.C
void _I2CBitDly(void);
void _I2CSCLHigh(void);
void I2CSendAddr(
	byte addr, /*
	*
	*  Description: Adresse des Geraetes, mit dem ueber I2C kommuniziert
	*               werden soll.
	*               addr / 2 entspricht der Adresse des gewuenschten Geraets.
	*  Direction  : in
	*  Values     : 0-255 (?)	(Adresse*2)
	*/  

	byte rd /*
	*
	*  Description: Legt die gewuenschte Richtung der Kommunikation fest.
	*  Direction  : in
	*  Values     : 0 = schreibend
	*               1 = lesend
	*/  
);

byte I2CSendByte(
	byte bt /*
	*
	*  Description: Byte, dass ueber I2C versendet werden soll.
	*  Direction  : in
	*  Values     : 0-255		(Byte)
	*/  
);

void I2CSendStop(void);

// byte _I2CGetByte(byte lastone /*
// 	*
// 	*  Description: Schalter zur Auswahl zwischen Funktionalitaeten.
// 	*  Direction  : in
// 	*  Values     : 0 = Funktion "Byte lesen"
// 	*               1 = Funktion "Letztes Byte lesen"
// 	*/  
// );

#endif /* AUDITINGSYSTEMSENDMSG_H */
