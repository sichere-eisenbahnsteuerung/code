/*****************************************************************************
 *
 *        Dateiname:    ASTF1.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Felix Theodor Blueml
 *
 *
 *        Testobjekt:   Auditing-System, Version 0.4
 *
 *        Beschreibung:
 *        Diese Datei ist Teil des Testfalls 1 fuer den Programmcode zum Modul
 *        Auditing-System in der Version 0.4.
 *        Testname: Uebertragungstest
 *        Szenario: Test auf korrekte Uebertragung.
 *        Testspezifikationsversion: noch keine vorhanden
 *
 ****************************************************************************/

/* Includes *****************************************************************/
#include <reg515c.h>
#include <intrins.h>	// _nop_();
#include "ASTF1.h"
#include "AuditingSystem.h"
#include "AuditingSystemSendMsg.h"
#include "AuditingSystemReportAllMsg.h"


/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/

/* Prototypen fuer lokale Funktionen ****************************************/

/* Funktionsimplementierungen ***********************************************/

void main (void) 
{
	byte msgHallo[6];
	byte msgTest[6];

	EAL = 1; //enable all interrupts
	
	initAS();

	msgHallo[0] = 'H';
	msgHallo[1] = 'a';
	msgHallo[2] = 'l';
	msgHallo[3] = 'l';
	msgHallo[4] = 'o';
	msgHallo[5] = '!';
	
	msgTest[0] = 'T';
	msgTest[1] = 'E';
	msgTest[2] = 'S';
	msgTest[3] = 'T';
	msgTest[4] = '4';
	msgTest[5] = '2';
	
	// Fuellstand sollte jetzt 0 sein (leer)

	sendMsg(0,msgHallo);
	sendMsg(1,msgHallo);
	sendMsg(2,msgHallo);
	sendMsg(3,msgHallo);
	sendMsg(4,msgHallo);
	sendMsg(5,msgHallo);
	sendMsg(6,msgHallo);
	sendMsg(7,msgHallo);
	sendMsg(8,msgHallo);
	sendMsg(9,msgHallo);
	sendMsg(10,msgHallo);
	sendMsg(11,msgHallo);
	sendMsg(12,msgHallo);
	sendMsg(13,msgHallo);
	sendMsg(14,msgHallo);
	sendMsg(15,msgHallo);
	sendMsg(16,msgHallo);
	sendMsg(17,msgTest);
	sendMsg(18,msgTest);
	sendMsg(19,msgTest);
	sendMsg(20,msgTest);
	sendMsg(21,msgTest);
	sendMsg(22,msgTest);
	sendMsg(23,msgTest);
	sendMsg(24,msgTest);
	sendMsg(25,msgTest);
	sendMsg(26,msgTest);
	sendMsg(27,msgTest);
	sendMsg(28,msgTest);

	// Fuellstand sollte jetzt 29 sein
	sendMsg(29,msgTest);
	// Fuellstand sollte jetzt 30 sein (voll)

	sendMsg(30,msgTest);
	// Fuellstand sollte jetzt 30 sein (voll)
	// Pufferueberlauf; ueberschreiben der aeltesten Meldung (0)

	sendMsg(31,msgTest);
	// Fuellstand sollte jetzt 30 sein (voll)
	// Pufferueberlauf; ueberschreiben der aeltesten Meldung (1)

	sendMsg(32,msgTest);
	// Fuellstand sollte jetzt 30 sein (voll)
	// Pufferueberlauf; ueberschreiben der aeltesten Meldung (2)

	reportAllMsg();
/*
	workAS();
	// Fuellstand sollte jetzt 26 sein
  	for(i=0; i<255; i++)
	{
		_nop_();
		_nop_();
		_nop_();
	}

	workAS();
	// Fuellstand sollte jetzt 22 sein
  	for(i=0; i<255; i++)
	{
		_nop_();
		_nop_();
		_nop_();
	}
	workAS();
	// Fuellstand sollte jetzt 18 sein
  	for(i=0; i<255; i++)
	{
		_nop_();
		_nop_();
		_nop_();
	}

	workAS();
	// Fuellstand sollte jetzt 14 sein
  	for(i=0; i<255; i++)
	{
		_nop_();
		_nop_();
		_nop_();
	}

	workAS();
	// Fuellstand sollte jetzt 10 sein
  	for(i=0; i<255; i++)
	{
		_nop_();
		_nop_();
		_nop_();
	}

	workAS();
	// Fuellstand sollte jetzt 6 sein
  	for(i=0; i<255; i++)
	{
		_nop_();
		_nop_();
		_nop_();
	}

	workAS();
	// Fuellstand sollte jetzt 2 sein
  	for(i=0; i<255; i++)
	{
		_nop_();
		_nop_();
		_nop_();
	}

	workAS();
	// Fuellstand sollte jetzt 0 sein (leer)
  	for(i=0; i<255; i++)
	{
		_nop_();
		_nop_();
		_nop_();
	}
	
	workAS();
	// Fuellstand sollte jetzt 0 sein (leer)
*/
	for(;;)	//Endlosschleife
	{
	}
}
