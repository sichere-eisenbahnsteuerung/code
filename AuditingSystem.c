/*****************************************************************************
 *
 *        Dateiname:    AuditingSystem.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Felix Theodor Blueml
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
 ****************************************************************************/

/* Includes *****************************************************************/

#include "AuditingSystem.h"
#include "Betriebsmittelverwaltung.h"


/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

static byte AS_msg_array[15][7];
/*
 *  Description: Ringpuffer zum speichern der Statusmeldungen der Module.
 *  Values     : [0-14][0]:    0 = Leitzentrale
 *                             1 = Befehlsvalidierung
 *                             2 = Ergebnisvalidierung
 *                            16 = Leitzentrale, Pufferueberlauf
 *                            17 = Befehlsvalidierung, Pufferueberlauf
 *                            18 = Ergebnisvalidierung, Pufferueberlauf
 *               (Modulnummer und ggf. Flag fuer Pufferueberlauf)
 *               
 *               [0-14][1-6]: 0-255	(Statusmeldung)
 *               (Siehe die Beschreibung des jeweiligen Moduls)
 */

static byte AS_read_next_msg;
/*
 *  Description: Lesezeiger ("OUT-­Index") fuer den Ringpuffer.
 *  Values     : 0-14	(Lesezeiger)
 */

static byte AS_fill_next_msg;
/*
 *  Description: Schreibezeiger ("IN-­Index") fuer den Ringpuffer.
 *  Values     : 0-14	(Schreibezeiger)
 */

static byte AS_msg_counter;
/*
 *  Description: Fuellstandzaehler fuer den Ringpuffer.
 *  Values     : 0-15	(Anzahl der Elemente im Ringpuffer)
 */


/* Lokale Makros ************************************************************/

#ifndef _nop_
	#define _nop_	;
#endif
/*
 * Leeranweisung zum Zeitvertreib.
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

// Uebernommene Funktionen aus der Datei I2C_SW.C //

/*
 * _I2CBitDly()
 * wait 4.7uS, or thereabouts
 * tune to xtal. This works at 11.0592MHz
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
	*               addr / 2 entspricht der Adresse des gewuenschten
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
// byte _I2CGetByte(byte lastone /*
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
 * send_msg(byte msg[6], byte module_id)
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
 * aelteste Meldung gesetzt wird. Der Fuellstandzaehler wird auf 14 gesetzt um
 * einerseits ggf. wieder in den Gueltigkeitsbereich zu gelangen. Andererseits
 * geschieht dies, da er im Folgenden wieder erhoeht wird und somit
 * nach-wie-vor den Befuellungsstand voll beibehaelt.
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
void send_msg(byte msg[6], byte module_id)
{
	byte i;
	// Enthaellt AS_fill_next_msg falschen Wert?
	if(AS_fill_next_msg >= 15)
	{
		// Sicherstellen, dass nicht wahllos in den Speicher
		// geschrieben wird
		AS_fill_next_msg = 0;
	}
	
	// Pufferueberlauf oder enthaellt AS_msg_counter falschen Wert?
	if(AS_msg_counter >= 15)
	{
		// Pufferueberlauf signalisieren
		module_id = module_id + 0x10;
		
		// Lesezeiger auf die aelteste Meldung setzen
		AS_read_next_msg = (AS_read_next_msg+1) % 15;
		
		// Fuellstand um 1 verringern
		AS_msg_counter = 14;
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
	AS_fill_next_msg = (AS_fill_next_msg+1) % 15;
}

/*
 * workAS()
 * Schnittstelle nur fuer das Modul Betriebsmittelverwaltung. Die
 * Schnittstelle dient dem Auslesen der Meldungen der Module aus dem
 * Ringpuffer und deren Versendung ueber den I2C-Bus.
 * Die Uebertragung eines komplett befuellten Puffers bedeutet, dass 105 Bytes
 * (15 Meldungen) versendet werden sollen.
 * 
 * Nach Aufruf dieser Schnittstelle wird ueberprueft, ob der Ringpuffer leer
 * ist. Wenn dem so ist, muessen keine Daten untersucht werden und der Vorgang
 * wird beendet.
 * Andernfalls wird ueberprueft, ob der Lesezeiger AS_read_next_msg auf einen
 * gueltigen Index zeigt. Ggf. wird dieser durch Nullsetzen korrigiert.
 * Dadurch soll sichergestellt werden, dass im Folgenden nicht wahllose Werte
 * aus dem Speicher gelesen werden.
 * Der Zeitmesser timer wird mit 0 initialisiert und im Folgenden bei jeder
 * I2C-Uebertragung auf den Wert 215 überprueft. Wird dieser überstiegen,
 * wird der Vorgang beendet.
 * Es wird wiederholt versucht die I2C-Bus-Verbindung zum Mikrocontroller
 * "Arduino Duemilanove" herzustellen. Dazu wird die Funktion
 * I2CSendAddr(addr, rd) mit den Parametern (2, WRITE) benutzt. Bei jedem
 * Versuch wird der Zeitmesser um 4 erhoeht. 
 * Danach wird versucht, solange der Ringpuffer nicht leer ist, die naechsten
 * 7 Daten aus dem Ringpuffer an der Stelle des Lesezeigers AS_read_next_msg
 * mit Hilfe der Funktion I2CSendByte(bt) zu versenden. Bei jedem
 * Sendeversuch wird der Zeitmesser jeweils um 2 erhoeht.
 * Nachdem jeweils die 7 Daten versendet wurden, wird der Lesezeiger auf die
 * naechste Meldung gesetzt und der Fuellstandszaehler um 1 dekrementiert.
 * Ist der Vorgang des Daten aus dem Ringpuffers versenden abgeschlossen,
 * wird die Verbindung getrennt. Dazu wird die Funktion I2CSendStop() benutzt
 * Bei jedem Versuch wird der Zeitmesser um 1 erhoeht.
 * 
 * Rueckgabe: Keine
 * 
 * Autor: Felix Blueml
 */
void workAS()
{
	// Zeitmesser; dient der Verhinderung des Ueberschreitens des
	// vorgegebenen Zeitfensters von 20 ms
	byte timer = 0;

	// Keine Elemente im Puffer zum versenden?
	if(!AS_msg_counter)
	{
		// Abbruch!
		return;
	}
	
	// Enthaellt AS_read_next_msg falschen Wert?
	if(AS_read_next_msg >= 15)
	{
		// Sicherstellen, dass nicht wahllos irgendwelche Werte aus
		// dem Speicher gelesen werden
		AS_read_next_msg = 0;
	}
	
	
	// Versuche eine schreibende Verbindung zum Arduino mit der Adresse 4
	// herzustellen
	_i2c_error = 255; // Status: Kein Fehler
	do
	{
		I2CSendAddr(2, WRITE);
		timer += 4; // Relative Zeitmessung
		
		// Gesamtzeit ueberschritten?
		if(timer > 215)
		{
			// Abbruch!
			return;
		}
	}
	while(_i2c_error != 255);	// Wiederhole, solange Fehler
					// aufgetreten
	
	// Leere den Ringpuffer und versende die Daten
	for(AS_msg_counter; AS_msg_counter>0; AS_msg_counter--)
	{
		byte i;
		for(i=0; i<7; i++)
		{
			// Versuche ein Byte aus dem Ringpuffer an den Arduino
			// zu versenden
			_i2c_error = 255; // Status: Kein Fehler
			do
			{
				I2CSendByte(
					AS_msg_array[AS_read_next_msg][i]
				);
				timer += 2; // Relative Zeitmessung
				
				// Gesamtzeit ueberschritten?
				if(timer > 215)
				{
					// Abbruch!
					return;
				}
			}
			while(_i2c_error != 255);	// Wiederhole, solange
							// Fehler aufgetreten
		}
		
		// Eine komplette Statusmeldung wurde versendet,
		// Lesezeiger auf die naechste zu sendende Meldung setzen
		AS_read_next_msg = (AS_read_next_msg+1) % 15;
	}
	
	
	// Versuche die Verbindung zu beenden
	_i2c_error = 255; // Status: Kein Fehler
	do
	{
		I2CSendStop();
		timer += 1; // Relative Zeitmessung
		
		// Gesamtzeit ueberschritten?
		if(timer > 215)
		{
			// Abbruch!
			return;
		}
	}
	while(_i2c_error != 255);	// Wiederhole, solange Fehler
					// aufgetreten
}


// Uebernommene Funktionen aus der Datei I2C_SW.C //

/*
 * _I2CBitDly()
 * wait 4.7uS, or thereabouts
 * tune to xtal. This works at 11.0592MHz
 * 
 * Rueckgabe: Keine
 * 
 * Autor: -
 */
void _I2CBitDly()
{
	_nop_;		// delay is 5.4uS, only 4.3uS without
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
