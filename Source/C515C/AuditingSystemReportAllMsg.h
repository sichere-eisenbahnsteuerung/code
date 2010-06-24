#ifndef AUDITINGSYSTEMREPORTALLMSG_H
#define AUDITINGSYSTEMREPORTALLMSG_H
/*****************************************************************************
 *
 *        Dateiname:    AuditingSystemReportAllMsg.h
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
 *        Diese Headerdatei ist fuer das Modul Not-Aus-Treiber vorgesehen. Sie
 *        enthaellt den Prototypen fuer die Schnittstelle reportAllMsg.
 *
 ****************************************************************************/




/* Globale Makrodefinitionen ************************************************/

/* Globale Typdefinitionen **************************************************/

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

/* Deklaration von globalen Funktionen **************************************/

/*
 * reportAllMsg()
 * Schnittstelle nur fuer das Modul Not-Aus-Treiber. Die Schnittstelle dient
 * dem Auslesen aller Meldungen der Module aus dem Ringpuffer und deren
 * Versendung ueber den I2C-Bus.
 * Diese Schnittstelle darf nur bei deaktiviertem Watchdog aufgerufen werden.
 * 
 * Rueckgabe: Keine
 */
void reportAllMsg(void);


#endif /* AUDITINGSYSTEMREPORTALLMSG_H */
