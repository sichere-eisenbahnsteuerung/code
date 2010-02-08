/*****************************************************************************
 *
 *        Dateiname:    AuditingSystem.c
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
 *        Betriebsmittelverwaltung werden max. vier gesammelte Meldungen
 *        verschickt. Bei erkanntem Versagen des Systems, gibt das Modul
 *        Not-Aus-Treiber das Kommando alle gesammelten Meldungen zu
 *        verschicken.
 *
 ****************************************************************************/

/* Includes *****************************************************************/

#include <intrins.h>	// _nop_();
#include "AuditingSystem.h"
#include "Betriebsmittelverwaltung.h"


/* Definition globaler Konstanten *******************************************/

#ifndef MAX_MELDUNGEN
	#define MAX_MELDUNGEN	30
#endif
/*
 * Kapazitat des Ringpuffers. Maximale Anzahl der zu puffernden Meldungen.
 */


#ifndef WRITE
	#define WRITE	0
#endif
/*
 * Funktionsparameter fuer I2CSendAddr.
 */

#ifndef READ
	#define READ	1
#endif
/*
 * Funktionsparameter fuer I2CSendAddr.
 */

#ifndef SCL
	#define SCL	AS_PORT_I2C_SCL
#endif
/*
 * I2C-SCL-Anschluss des C515C
 */

#ifndef SDA
	#define SDA	AS_PORT_I2C_SDA
#endif
/*
 * I2C-SDA-Anschluss des C515C
 */


/* Definition globaler Variablen ********************************************/

static byte AS_msg_array[MAX_MELDUNGEN][7];
/*
 *  Description: Ringpuffer zum speichern der Statusmeldungen der Module.
 *  Values     : [0-(MAX_MELDUNGEN-1)][0]:
 *                0 = Leitzentrale
 *                1 = Befehlsvalidierung
 *                2 = Ergebnisvalidierung
 *               16 = Leitzentrale, Pufferueberlauf
 *               17 = Befehlsvalidierung, Pufferueberlauf
 *               18 = Ergebnisvalidierung, Pufferueberlauf
 *               (Modulnummer und ggf. Flag fuer Pufferueberlauf)
 *               
 *               [0-(MAX_MELDUNGEN-1)][1-6]: 0-255	(Statusmeldung)
 *               (Siehe die Beschreibung des jeweiligen Moduls)
 */

static byte AS_read_next_msg;
/*
 *  Description: Lesezeiger ("OUT-Index") fer den Ringpuffer.
 *  Values     : 0-(MAX_MELDUNGEN-1)	(Lesezeiger)
 */

static byte AS_fill_next_msg;
/*
 *  Description: Schreibezeiger ("IN-Index") fuer den Ringpuffer.
 *  Values     : 0-(MAX_MELDUNGEN-1)	(Schreibezeiger)
 */

static byte AS_msg_counter;
/*
 *  Description: Fuellstandzaehler fuer den Ringpuffer.
 *  Values     : 0-MAX_MELDUNGEN	(Anzahl der Elemente im Ringpuffer)
 */


/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/

byte _i2c_error;
/*
 *  Description: Bitfeld mit Fehlerarten fuer I2C-Verbindung.
 *  Values     : 1   = Keine Bestaetigung empfangen
 *               2   = SCL konnte nicht gesetzt werden
 *               255 = Kein Fehler
 *               (Fehlerarten)
 */


/* Prototypen fuer lokale Funktionen ****************************************/

/*
 * warten()
 * 1,568 ms verstreichen lassen.
 * 
 * Rueckgabe: Keine
 */
void warten(void);

// Uebernommene Funktionen aus der Datei I2C_SW.C //

/*
 * _I2CBitDly()
 * wait 4.7uS, or thereabouts
 * tune to xtal. This works at 10MHz
 * 
 * Rueckgabe: Keine
 */
void _I2CBitDly(void);

/*
 * _I2CSCLHigh()
 * set SCL high, and wait for it to go high
 * 
 * Rueckgabe: Keine
 */
void _I2CSCLHigh(void);
 
/*
 * I2CSendAddr(byte addr, byte rd)
 * 
 * 
 * Rueckgabe: Keine
 */
void I2CSendAddr(
	byte addr, /*
	*
	*  Description: Adresse des Geraetes, mit dem ueber I2C kommuniziert
	*               werden soll.
	*               addr * 2 entspricht der Adresse des gewuenschten
	*		Geraets.
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

/*
 * I2CSendByte(byte bt)
 * 
 * 
 * Rueckgabe: Keine
 */
void I2CSendByte(
	byte bt /*
	*
	*  Description: Byte, dass ueber I2C versendet werden soll.
	*  Direction  : in
	*  Values     : 0-255		(Byte)
	*/  
);

// /*
//  * _I2CGetByte(byte lastone)
//  * 
//  * 
//  * Rueckgabe: byte
//  */
// byte _I2CGetByte(
// 	byte lastone /*
// 	*
// 	*  Description: Schalter zur Auswahl zwischen Funktionalitaeten.
// 	*  Direction  : in
// 	*  Values     : 0 = Funktion "Byte lesen"
// 	*               1 = Funktion "Letztes Byte lesen"
// 	*/  
// );

/*
 * I2CSendStop()
 * 
 * 
 * Rueckgabe: Keine
 */
void I2CSendStop(void);


/* Funktionsimplementierungen ***********************************************/

/*
 * initAS()
 * Schnittstelle nur fuer das Modul Betriebsmittelverwaltung. Die 
 * Schnittstelle dient der Initialisierung der Steuerungsvariablen fuer den 
 * Ringpuffer.
 * 
 * Nach Aufruf dieser Schnittstelle werden die Bytes AS_read_next_msg, 
 * AS_fill_next_msg und AS_msg_counter jeweils auf 00h gesetzt.
 * 
 * Rueckgabe: Keine
 * 
 * Autor: Felix Blueml
 */
void initAS()
{
	// Lesezeiger auf Position 0 im Ringpuffer setzen
	AS_read_next_msg = 0;

	// Schreibezeiger auf Position 0 im Ringpuffer setzen
	AS_fill_next_msg = 0;
	
	// Bekanntgabe: Ringpuffer ist leer
	AS_msg_counter   = 0;
}

/*
 * sendMsg(byte module_id, byte msg[6])
 * Schnittstelle mit Statusmeldungen fuer Module mit zugeordneter Modulnummer.
 * (Siehe dazu den Prototypen-Kommentar in der Header-Datei
 * AuditingSystemSendMsg.h)
 * Die Schnittstelle dient der Aufnahme aktueller Statusmeldungen der Module.
 * 
 * Nach Aufruf dieser Schnittstelle wird ueberprueft, ob der Schreibezeiger
 * AS_fill_next_msg auf einen gueltigen Index zeigt. Ggf. wird dieser durch
 * Nullsetzen korrigiert. Dadurch soll sichergestellt werden, dass im
 * Folgenden nicht wahllos in den Speicher geschrieben werden kann.
 * Danach wird geprueft, ob sich der Fuellstandzaehler AS_msg_counter
 * ausserhalb des gueltigen Bereichs befindet. Wenn dem so ist, so wird die
 * Modul-ID module_id um 16 erhoeht, sodass sie die Form 1Xh erhaelt. Der
 * Lesezeiger AS_read_next_msg wird korrigiert, in dem er auf die zweit
 * aelteste Meldung gesetzt wird. Der Fuellstandzaehler wird auf
 * (MAX_MELDUNGEN-1) gesetzt um einerseits ggf. wieder in den
 * Gueltigkeitsbereich zu gelangen. Andererseits geschieht dies, da er im
 * Folgenden wieder erhoeht wird und somit nach-wie-vor den Befuellungsstand
 * voll beibehaelt.
 * Im Anschluss werden die per Parameter uebergebenen Informationen in den
 * Ringpuffer AS_msg_array an die Stelle des Schreibezeigers in der folgenden
 * Reihenfolge geschrieben:
 * Modul-ID, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]
 * Schliesslich wird der Fuellstandzaehler erhoeht und der Schreibezeiger auf
 * die naechste Meldung gesetzt.
 * 
 * Rueckgabe: Keine
 * 
 * Autor: Felix Blueml
 */
void sendMsg(byte module_id, byte msg[6])
{
	byte i;
	// Enthaellt AS_fill_next_msg falschen Wert?
	if(AS_fill_next_msg >= MAX_MELDUNGEN)
	{
		// Sicherstellen, dass nicht wahllos in den Speicher
		// geschrieben wird
		AS_fill_next_msg = 0;
	}
	
	// Pufferueberlauf oder enthaellt AS_msg_counter falschen Wert?
	if(AS_msg_counter >= MAX_MELDUNGEN)
	{
		// Pufferueberlauf signalisieren
		module_id = module_id + 0x10;
		
		// Lesezeiger auf die aelteste Meldung setzen
		AS_read_next_msg = (AS_read_next_msg+1) % MAX_MELDUNGEN;
		
		// Fuellstand um 1 verringern
		AS_msg_counter = MAX_MELDUNGEN-1;
	}
	
	// Modulnummer und Statusmeldung zusammen im Ringpuffer speichern
	// speichere Modulnummer
	AS_msg_array[AS_fill_next_msg][0] = module_id;
	
	for(i=1; i<7; i++)
	{
		// speichere Statusmeldung
		AS_msg_array[AS_fill_next_msg][i] = msg[i-1];
	}
	
	// Fuellstand um 1 erhoehen
	AS_msg_counter++;
	
	// Schreibezeiger auf die naechste zu ueberschreibende
	// Meldung setzen
	AS_fill_next_msg = (AS_fill_next_msg+1) % MAX_MELDUNGEN;
}

/*
 * workAS()
 * Schnittstelle nur fuer das Modul Betriebsmittelverwaltung. Die
 * Schnittstelle dient dem Auslesen maximal vier Meldungen der Module aus dem
 * Ringpuffer und deren Versendung ueber den I2C-Bus.
 * Fuer diese Uebertragung werden 28 Bytes versendet.
 * Vor jedem erneuten Aufruf dieser Schnittstelle muessen mindestens 1,102 ms
 * verstrichen sein.
 * 
 * Nach Aufruf dieser Schnittstelle wird ueberprueft, ob der Ringpuffer leer
 * ist. Wenn dem so ist, muessen keine Daten untersucht werden und der Vorgang
 * wird beendet.
 * Andernfalls wird ueberprueft, ob der Lesezeiger AS_read_next_msg auf einen
 * gueltigen Index zeigt. Ggf. wird dieser durch Nullsetzen korrigiert.
 * Dadurch soll sichergestellt werden, dass im Folgenden nicht wahllose Werte
 * aus dem Speicher gelesen werden.
 * Es wird versucht die I2C-Bus-Verbindung zum AD herzustellen. Dazu wird die
 * Funktion I2CSendAddr(addr, rd) mit den Parametern (8, WRITE) benutzt.
 * Schlaegt die Verbindung fehl, wird der Vorgang beendet.
 * Danach wird versucht maximal vier Meldungen zu versenden. Dazu werden
 * jeweils die naechsten 7 Daten aus dem Ringpuffer an der Stelle des
 * Lesezeigers AS_read_next_msg gelesen und mit Hilfe der Funktion
 * I2CSendByte(bt) wird sucht, sie zu versenden. Schlaegt die Verbindung fehl,
 * wird erst versucht die Verbindung zu beenden und danach wird der Vorgang
 * beendet.
 * Ist der Vorgang des Daten aus dem Ringpuffers versenden abgeschlossen, wird
 * versucht die Verbindung zu beenden. Dazu wird die Funktion I2CSendStop()
 * benutzt.
 * 
 * Rueckgabe: Keine
 * 
 * Autor: Felix Blueml
 */
void workAS()
{
	byte max;	// Maximale Anzahl Mitteilungen, die versendet werden

	byte i, j;	// Zaehler fuer die beiden for-Schleifen, fuer die
			// Versendung von Mitteilungen

	// Keine Elemente im Puffer zum versenden?
	if(!AS_msg_counter)
	{
		// Abbruch!
		return;
	}
	
	// Enthaellt AS_read_next_msg falschen Wert?
	if(AS_read_next_msg >= MAX_MELDUNGEN)
	{
		// Sicherstellen, dass nicht wahllos irgendwelche Werte aus
		// dem Speicher gelesen werden
		AS_read_next_msg = 0;
	}
	

	// Versuche eine schreibende Verbindung zum Arduino mit der
	// Adresse 4 herzustellen

	_i2c_error = 255; // Status: Kein Fehler

	I2CSendAddr(8, WRITE);

	if(_i2c_error != 255) // Fehler erkannt?
	{
		// Abbruch!
		return;
	}


	// Lese maximal 4 Mitteilungen aus dem Ringpuffer und versende sie
	if(AS_msg_counter<4)
	{
		max = AS_msg_counter;
	}
	else
	{
		max = 4;
	}

	for(i=0; i<max; i++)
	{
		for(j=0; j<7; j++)
		{
			// Versuche ein Byte aus dem Ringpuffer an den Arduino
			// zu versenden

			_i2c_error = 255; // Status: Kein Fehler

			I2CSendByte(AS_msg_array[AS_read_next_msg][j]);
			
			if(_i2c_error != 255) // Fehler erkannt?
			{
				// Versuche die Verbindung zu beenden
				I2CSendStop();

				// Abbruch!
				return;
			}
		}
		

		// Eine komplette Statusmeldung wurde versendet,
		// Lesezeiger auf die naechste zu sendende Meldung setzen
		AS_read_next_msg = (AS_read_next_msg+1) % MAX_MELDUNGEN;

		// Fuellstand um 1 verringern
		AS_msg_counter--;
	}

	// Versuche die Verbindung zu beenden
	I2CSendStop();
}

/*
 * reportAllMsg()
 * Schnittstelle nur fuer das Modul Not-Aus-Treiber. Die Schnittstelle dient
 * dem Auslesen aller Meldungen der Module aus dem Ringpuffer und deren
 * Versendung ueber den I2C-Bus.
 * Diese Schnittstelle darf nur bei deaktiviertem Watchdog aufgerufen werden.
 * 
 * Nach Aufruf dieser Schnittstelle wird zehn mal abwechselnd 1,568 ms
 * gewartet und die Schnittstelle workAS() aufgerufen.
 * 
 * Rueckgabe: Keine
 * 
 * Autor: Felix Blueml
 */
void reportAllMsg()
{
	byte i;
  	for(i=0; i<10; i++)
	{
		warten();
		workAS();
	}

}

/*
 * warten()
 * 1,568 ms verstreichen lassen.
 * 
 * Rueckgabe: Keine
 * 
 * Autor: Felix Blueml
 */
void warten()
{
	byte i;
  	for(i=0; i<200; i++)
	{
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
	}

}

// Uebernommene Funktionen aus der Datei I2C_SW.C //

/*
 * _I2CBitDly()
 * wait 4.7uS, or thereabouts
 * tune to xtal. This works at 10MHz
 * 
 * Rueckgabe: Keine
 * 
 * Autor: -
 */
void _I2CBitDly()
{
	// 8 mal einen NOP (Leerbefehl) ausfuehren um 4,8 us zu warten
	_nop_(); // je 600 ns warten
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	return;
}

/*
 * _I2CSCLHigh()
 * set SCL high, and wait for it to go high
 * 
 * Rueckgabe: Keine
 * 
 * Autor: -
 */
void _I2CSCLHigh()
{
	register int err;
	SCL = 1;
	while(!SCL)
	{
		err++;
		if(!err)
		{
			// SCL stuck, something's holding it down
			_i2c_error &= 0x02;
			return;
		}
	}
}

/*
 * I2CSendAddr(byte addr, byte rd)
 * 
 * 
 * Rueckgabe: Keine
 * 
 * Autor: -
 */
void I2CSendAddr(byte addr, byte rd)
{
	SCL = 1;
	_I2CBitDly();
	SDA = 0;		// generate start
	_I2CBitDly();
	SCL = 0;
	_I2CBitDly();
	I2CSendByte(addr+rd);	// send address byte
}

/*
 * I2CSendByte(byte bt)
 * 
 * 
 * Rueckgabe: Keine
 * 
 * Autor: -
 */
void I2CSendByte(byte bt)
{
	register unsigned char i;
	for(i=0; i<8; i++)
	{
		if(bt & 0x80)
		{
			// send each bit, MSB first
			SDA = 1;
		}
		else
		{
			SDA = 0;
		}
		_I2CSCLHigh();
		_I2CBitDly();
		SCL = 0;
		_I2CBitDly();
		bt = bt << 1;
	}
	SDA = 1;		// listen for ACK
	_I2CSCLHigh();
	_I2CBitDly();
	if(SDA)
	{
		// ack didn't happen, may be nothing out there
		_i2c_error &= 0x01;
	}
	SCL = 0;
	_I2CBitDly();
}

// /*
//  * _I2CGetByte(byte lastone)
//  * 
//  * 
//  * Rueckgabe: byte
//  * 
//  * Autor: -
//  */
// byte _I2CGetByte(byte lastone)
// {
// 	register unsigned char i, res;
// 	res = 0;
// 	for(i=0;i<8;i++)	// each bit at a time, MSB first
// 	{
// 		_I2CSCLHigh();
// 		_I2CBitDly();
// 		res *= 2;
// 		if(SDA)
// 		{
// 			res++;
// 		}
// 		SCL = 0;
// 		_I2CBitDly();
// 	}
// 	SDA = lastone;		// send ACK according to 'lastone'
// 	_I2CSCLHigh();
// 	_I2CBitDly();
// 	SCL = 0;
// 	SDA = 1;
// 	_I2CBitDly();
// 	return(res);
// }

/*
 * I2CSendStop()
 * 
 * 
 * Rueckgabe: Keine
 * 
 * Autor: -
 */
void I2CSendStop()
{
	SDA = 0;
	_I2CBitDly();
	_I2CSCLHigh();
	_I2CBitDly();
	SDA = 1;
	_I2CBitDly();
}
