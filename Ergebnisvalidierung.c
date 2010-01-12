/*****************************************************************************
 *
 *        Dateiname:    ergebnisvalidierung.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Philip Weber
 *
 *
 *        Modul:        Ergebnisvalidierung, <Version des Moduldesigns>
 *
 *        Beschreibung:
 *        ________________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *
 ****************************************************************************/

/* Includes *****************************************************************/
#include <REG515C.H>
// TODO: includes korrigieren
#include "EV_test_header.h"
#include "Ergebnisvalidierung.h"

#include <stdio.h>
//#include <string.h>

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/
// Dient zum Zaehlen vergeblicher Schreibversuche in den Shared Memory
// zum SSC-Treiber
static byte counterSSC = 0;

// Dient zum Zaehlen vergeblicher Schreibversuche in den Shared Memory
// zum RS232-Treiber
static byte counterRS232 = 0;

// Dient zum Zaehlen der Streckenbefehl-Vergleich-Versuche
static byte streckenbefehleUngleich = 0;

// Dient zum Speichern des Streckenbefehls aus dem Shared Memory von der
// Befehlsvalidierung
static Streckenbefehl internerStreckenbefehl = {LEER, LEER, LEER, LEER};

// Dient zum Speichern des Streckenbefehls aus dem Shared Memory vom
// SSC-Treiber
static Streckenbefehl externerStreckenbefehl = {LEER, LEER, LEER, LEER};

/* Prototypen fuer lokale Funktionen ****************************************/
// TODO: Wie funktionieren die Prototypen???

/* Funktionsimplementierungen ***********************************************/
/*
* Name		: initEV
* Description	: Dient zum Initialisieren der vom Modul benoetigten Variablen.
* Author	: Philip Weber
*/
void initEV(void)
{
	counterSSC = 0;
	counterRS232 = 0;
	streckenbefehleUngleich = 0;
	internerStreckenbefehl.Lok = LEER;
	internerStreckenbefehl.Weiche = LEER;
	internerStreckenbefehl.Entkoppler = LEER;
	internerStreckenbefehl.Fehler = LEER;
	externerStreckenbefehl.Lok = LEER;
	externerStreckenbefehl.Weiche = LEER;
	externerStreckenbefehl.Entkoppler = LEER;
	externerStreckenbefehl.Fehler = LEER;
}

/*
* Name		: isStreckenbefehlResetted
* Description	: Ueberprueft, ob der uebergebene Streckenbefehl komplett
		  zurueckgesetzt ist.
* Parameter	: track	- Zu ueberpruefender Streckenbefehl.
* Return value	: TRUE wenn der Streckenbefehl zurueckgesetzt ist,
		  FALSE falls nicht
* Author	: Philip Weber
*/
boolean isStreckenbefehlResetted(Streckenbefehl *track)
{
	// Ueberpruefung, ob der Streckenbefehl zurueckgesetzt ist.
	// Der Fehler ist an dieser Stelle irrelevant.
	if(track->Lok != LEER ||
	   track->Weiche != LEER ||
	   track->Entkoppler != LEER)
	{
		// Falls einer der Werte des Streckenbefehls nicht den
		// Wert LEER besitzt, gilt dieser nicht als zurueckgesetzt
		// und die Funktion wird mit dem Wert FALSE verlassen.
		return FALSE;
	}
	// Sind alle Werte des Streckenbefehls zurueckgesetzt wird die 
	// Funktion mit dem Wert TRUE verlassen.
	return TRUE;
}

/*
* Name		: resetStreckenbefehl
* Description	: Setzt den uebergebenen Streckenbefehl zurueck.
* Parameter	: track - Streckenbefehl
* Author	: Philip Weber
*/
void resetStreckenbefehl(Streckenbefehl *track)
{
	// Streckenbefehl zuruecksetzen
	// Der Fehler ist irrelevant und wird nicht zurueckgesetzt.
	track->Lok = LEER;
	track->Weiche = LEER;
	track->Entkoppler = LEER;
}

/*
* Name		: streckenbefehleEqual
* Description	: Ueberprueft, ob die uebergebenen Streckenbefehle
		  gleich sind.
* Parameter	: track1 - 1. Streckenbefehl
		  track2 - 2. Streckenbefehl
* Return value	: TRUE wenn die Streckenbefehle identisch sind,
		  FALSE falls nicht.
* Author	: Philip Weber
*/
boolean streckenbefehleEqual(Streckenbefehl *track1, Streckenbefehl *track2)
{
	// Vergleichen der beiden Streckenbefehle
	if(track1->Lok == track2->Lok &&
	   track1->Weiche == track2->Weiche &&
	   track1->Entkoppler == track2->Entkoppler)
	{
		// Sind diese gleich, wird der Zaehler zurueckgesetzt ...
		streckenbefehleUngleich = 0;
		// und die Funktion mit dem Wert TRUE verlassen.
	   	return TRUE;
	}
	
	// Sind die Streckenbefehle ungleich, wird ueberprueft, wie oft
	// die Streckenbefehle bereits voneinander abwichen.
	if(streckenbefehleUngleich < 3)
	{
		// Ist der Wert kleiner 3 der Zaehler inkrementiert ...
		streckenbefehleUngleich++;
		// und die Funktion mit dem Wert FALSE verlassen, um das
		// Modul fruehzeitig zu beenden.
		return FALSE;
	}
	
	// Sind die Streckenbefehle ungleich und hat der Zaehler einen Wert
	// groesser gleich 3, wird ein Fehlercode an das Auditing System 
	// uebermittelt, ...
	send_msg(3,2);
	// ein Not-Aus eingeleitet ...
	emergency_off();
	// und die Funktion mit dem Wert FALSE verlassen, um das Modul
	// fruehzeitig zu beenden. 
	return FALSE;
}

/*
* Name		: checkForCommunicationErrors
* Description	: Ueberprueft, ob bei der Kommunikation mit den Treibern
		  Fehler aufgetreten sind. Ist dies der Fall soll das 
		  Modul beendet werden.
* Return value	: TRUE wenn Kommunikationsfehler gefunden wurden,
		  FALSE falls nicht.
* Author	: Philip Weber
*/
boolean checkForCommunicationErrors(void)
{
	// Ueberpruefung, ob der Fehleranteil des Shared Memory vom
	// RS232-Treiber zurueckgesetzt ist.
	if(RS232_EV_streckenbefehl.Fehler != 0x00)
	{
		// Falls nicht, wird der Fehlercode an das Auditing System
		// uebermittelt, ...
		send_msg(RS232_EV_streckenbefehl.Fehler,2);
		// ein Not-Aus eingeleitet ...
		emergency_off();
		// und die Funktion mit dem Wert TRUE verlassen. 
		// Es wird nicht auf Fehler ueberprueft.
		return TRUE;
	}

	// Ueberpruefung, ob der Fehleranteil des Shared Memory vom
	// SSC-Treiber zurueckgesetzt ist.
	if(SSC_EV_streckenbefehl.Fehler != LEER)
	{
		// Falls nicht, wird der Fehlercode an das Auditing System
		// uebermittelt, ...
		send_msg(SSC_EV_streckenbefehl.Fehler,2);
		// ein Not-Aus eingeleitet ...
		emergency_off();
		// und die Funktion mit dem Wert TRUE verlassen. 
		// Es wird nicht auf Fehler ueberprueft.
		return TRUE;
	}

	// Ueberpruefung, ob beim letzten Modulaufruf der interne
	// Streckenbefehl in den Shared Memory zum RS232-Treiber
	// geschrieben wurde. Dann waere counterRS232 == 0. 	
	if(counterRS232 > 0)
	{
		// Falls nicht wird ueberprueft, ob der Shared Memory zum
		// RS232-Treiber in der Zwischenzeit zurueckgesetzt wurde.
		if(isStreckenbefehlResetted(&EV_RS232_streckenbefehl))
		{
			// Falls ja, wird der interne Streckenbefehl in 
			// diesen geschrieben, ...
			EV_RS232_streckenbefehl = internerStreckenbefehl;
			// der interne Streckenbefehl zurueckgesetzt ...
			resetStreckenbefehl(&internerStreckenbefehl);
			// und der Zaehler zurueckgesetzt.
			counterRS232 = 0;

		// Falls der Shared Memory zum RS232-Treiber nicht 
		// zurueckgesetzt ist, wird ueberprueft, wie oft vergeblich
		// versucht wurde in diesen zu schreiben.
		} else if(counterRS232 < 3)
		{
			// Falls die Versuche einen Wert kleiner 3 aufweisen,
			// wird dieser inkrementiert.
			counterRS232++;

		// Falls der Shared Memory zum RS232-Treiber nicht 
		// zurueckgesetzt ist und schon 3 oder mehr Male versucht
		// wurde in diesen zu schreiben, ...
		} else 
		{
			// wird ein Fehlercode an das Auditing System 
			// uebermittelt, ...
			send_msg(3,2);
			// ein Not-Aus eingeleitet ...
			emergency_off();
			// und die Funktion mit dem Wert TRUE verlassen. 
			// Es wird nicht auf Fehler ueberprueft.
			return TRUE;
		}	
	}

	// Ueberpruefung, ob beim letzten Modulaufruf der interne
	// Streckenbefehl in den Shared Memory zum RS232-Treiber
	// geschrieben wurde. Dann waere counterSSC == 0.
	if(counterSSC > 0)
	{
		// Falls nicht wird ueberprueft, ob der Shared Memory zum
		// RS232-Treiber in der Zwischenzeit zurueckgesetzt wurde.
		if(isStreckenbefehlResetted(&EV_SSC_streckenbefehl))
		{
			// Falls ja, wird der interne Streckenbefehl in 
			// diesen geschrieben, ...
			EV_SSC_streckenbefehl = internerStreckenbefehl;
			// der interne Streckenbefehl zurueckgesetzt ...
			resetStreckenbefehl(&internerStreckenbefehl);
			// und der Zaehler zurueckgesetzt.
			counterSSC = 0;

		// Falls der Shared Memory zum SSC-Treiber nicht 
		// zurueckgesetzt ist, wird ueberprueft, wie oft vergeblich
		// versucht wurde in diesen zu schreiben.
		} else if(counterSSC < 3)
		{
			// Falls die Versuche einen Wert kleiner 3 aufweisen,
			// wird dieser inkrementiert.
			counterSSC++;
		
		// Falls der Shared Memory zum SSC-Treiber nicht 
		// zurueckgesetzt ist und schon 3 oder mehr Male versucht
		// wurde in diesen zu schreiben, ...
		} else 
		{
			// wird ein Fehlercode an das Auditing System 
			// uebermittelt, ...
			send_msg(3,2);
			// ein Not-Aus eingeleitet ...
			emergency_off();
			// und die Funktion mit dem Wert TRUE verlassen. 
			// Es wird nicht auf Fehler ueberprueft.
			return TRUE;
		}		
	}

	// Wird kein Fehler oder Kommunikationsproblem gefunden, wird die
	// Funktion mit dem Wert FALSE verlassen.
	return FALSE;
}

/*
* Name		: processInternalStreckenbefehl
* Description	: Ueberprueft den Shared Memory zur Befehlsvalidierung.
		  Falls dieser neu ist, wird er in den Shared Memory
		  zum SSC-Treiber geschrieben.
* Author	: Philip Weber
*/
void processInternalStreckenbefehl(void)
{
	// Neuer Streckenbefehl im Shared Memory von der Befehlsvalidierung?
	if(isStreckenbefehlResetted(&BV_EV_streckenbefehl))
	{
		// Falls nicht wird die Funktion verlassen
		return;
	}

	// Shared Memory von der Befehlsvalidierung in lokaler Variable 
	// fuer spaeter Ueberpruefungen speichern
	internerStreckenbefehl = BV_EV_streckenbefehl;
	// Shared Memory von der Befehlsvalidierung zuruecksetzen
	resetStreckenbefehl(&BV_EV_streckenbefehl);
	
	// Ueberpruefung, ob der Shared Memory zum SSC-Treiber 
	// zurueckgesetzt ist.
	if(isStreckenbefehlResetted(&EV_SSC_streckenbefehl))
	{
		// Falls ja kann der interne Streckenbefehl in diesen 
		// geschrieben werden.
		EV_SSC_streckenbefehl = internerStreckenbefehl;
		// Zaehlvariable zuruecksetzen
		counterSSC = 0;
	} else 
	{
		// Falls nicht, wird der Wert der Zaehlvariable erhoeht.
		counterSSC++;
	}
}

/*
* Name		: processExternalStreckenbefehl
* Description	: Ueberprueft, ob ein neuer Streckenbefehl im Shared Memory 
		  vorhanden ist. Falls ja wird dieser in eine interne Variable
		  und setzt den Shared Memory zurueck.
* Return value	: TRUE falls der externe und/oder der interne Streckenbefehl 
		       neu sind.
		  FALSE falls weder der externe noch der interne 
		  	Streckenbefehl neu sind 
* Author	: Philip Weber
*/
boolean processExternalStreckenbefehl(void)
{
	// Neuer Streckenbefehl im Shared Memory vom SSC-Treiber?
	if(!isStreckenbefehlResetted(&SSC_EV_streckenbefehl))
	{
		// Falls ja Shared Memory vom SSC-Treiber in lokaler 
		// Variable fuer spaeter Ueberpruefungen speichern
		externerStreckenbefehl = SSC_EV_streckenbefehl;
		// Shared Memory vom SSC-Treiber zuruecksetzen
		resetStreckenbefehl(&SSC_EV_streckenbefehl);
		// Funktion mit Wert TRUE verlassen
		return TRUE;
	}

	// Falls sich kein neuer Streckenbefehl im Shared Memory befindet,
	// wird ueberprueft, ob der interne Streckenbefehl zurueckgesetzt ist.
	if(!isStreckenbefehlResetted(&internerStreckenbefehl))
	{
		// Falls nicht wird die Funktion mit dem Wert TRUE verlassen
		return TRUE;
	}

	// Falls weder der externe noch der interne Streckenbefehl neu sind,
	// wird die Funktion mit dem Wert FALSE verlassen.
	return FALSE;
}

/*
* Name		: workEV
* Description	: Diese Funktion wird von der Betriebsmittelsteuerung 
		  aufgerufen. Sie stellt die gesamte Funktionalitaet des
		  Moduls Ergebnisvalidierung bereit.
* Author	: Philip Weber
*/
void workEV(void)
{
/* 
* Verarbeitungsschritt 1:
* Auf Kommunikationsprobleme pruefen 
*/
	// Als erstes wird ueberprueft, ob bei der Kommunikation mit den
	// Treibern Fehler oder Probleme aufgetreten sind.
	if(checkForCommunicationErrors())
	{
		// Falls ja wird das Modul verlassen.
		return;
	}
/* 
* Verarbeitungsschritt 2:
* Internen Streckenbefehl verarbeiten 
*/
	// Im Anschluss wird der interne Streckenbefehl, also der aus dem
	// Shared Memory von der Befehlsvalidierung, ueberprueft und in 
	// den Shared Memory zum SSC-Treiber geschrieben.
	processInternalStreckenbefehl();

/*
* Verarbeitungsschritt 3: 
* Externen Streckenbefehl verarbeiten 
*/
	// Danach wird der Streckenbefehl aus dem Shared Memory vom 
	// SSC-Treiber (externer Streckenbefehl) verarbeitet.
	if(!processExternalStreckenbefehl())
	{
		// Falls weder der externe noch der interne Streckenbefehl
		// neu sind, wird das Modul verlassen.
		return;
	}

/*
* Verarbeitungsschritt 4: 
* Streckenbefehle vergleichen 
*/
	// Daraufhin werden der interne und der externe Streckenbefehl 
	// miteinander verglichen.
	if(streckenbefehleEqual(&internerStreckenbefehl, &externerStreckenbefehl))
	{
		// Sind diese nicht gleich, wird das Modul verlassen.
		return;
	}

/*
* Verarbeitungsschritt 5: 
* Senden des internen Streckenbefehls an den RS232-Treiber 
*/

	// Ueberpruefung, ob der Shared Memory zum RS232-Treiber 
	// zurueckgesetzt ist.
	if(isStreckenbefehlResetted(&EV_RS232_streckenbefehl))
	{
		// Falls ja kann der interne Streckenbefehl in diesen 
		// geschrieben werden.
		EV_RS232_streckenbefehl = internerStreckenbefehl;
	} else 
	{
		// Falls nicht, wird der Wert der Zaehlvariable erhoeht ...
		counterRS232++;
		// und das Modul verlassen.
		return;
	}

/*
* Verarbeitungsschritt 6:
* Interne Variablen zuruecksetzen
*/
	initEV();
}


// Dient nur zu Testzwecken und bedarf deshalb keine weiterne Kommentare.
int main(void)
{

// TODO: entfernen

  SCON = 0x52;    /* SCON */                   /* setup serial port control */
  TMOD = 0x20;    /* TMOD */                   /* hardware (2400 BAUD @12MHZ) */
  TCON = 0x69;    /* TCON */
  TH1 =  0xf3;    /* TH1  */


	initEV();
	workEV();

	return 0;
}