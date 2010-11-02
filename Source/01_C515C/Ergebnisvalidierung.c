/**
 * @file     Ergebnisvalidierung.c
 *
 * @author   Philip Weber
 *   
 * @brief    Ueberpruefung der Streckenbefehle auf Korrektheit.
 * 
 *		     Hierzu werden diese ueber die SSC-Schnittstelle zwischen den 
 *		     Mikrocontrollern ausgetauscht und verglichen. Unterscheiden 
 *		     sich diese voneinander, wird ein Not-Aus-Signal erzeugt und 
 *		     eine Statusmeldung mit den abweichenden Werten an das 
 *		     Auditing-System zur Protokollierung gesendet.
 *
 * @date     13.01.2010
 */

// Includes:
#include "Ergebnisvalidierung.h"
#include "Betriebsmittelverwaltung.h"
#include "AuditingSystemSendMsg.h"
#include "Notaus.h"


// Lokale Makros:
#if ndef MAXSCCCOUNTER 
#define MAXSSCCOUNTER 3
#endif 
#if ndef	MAXRS232COUNTER
#define MAXRS232COUNTER 3
#endif 
#if ndef	MAXSTRECKENBEFEHLEUNGLEICH
#define MAXSTRECKENBEFEHLEUNGLEICH 3
#endif 

// Lokale Typen:
typedef enum {
	E_STRECKENBEFEHLEUNGLEICH   = 1,
	E_SSC_COUNTER               = 2,
	E_RS232_COUNTER             = 3,
	E_SSC_ERROR                 = 4,
	E_RS232_ERROR               = 5
} Element;

typedef enum {
	A_INFO      = 1,
	A_WARNING   = 2,
	A_FEHLER    = 3
} Art;

// Lokale Variablen: 
/**
 * @brief   Speichern der alten Streckenbefehl des SCC Treibers.
 */
static Streckenbefehl oldSSC = {LEER, LEER, LEER, 0};  

/**
 * @brief   Speichern der alten Streckenbefehl des RS232 Treibers.  
 */  
static Streckenbefehl oldRS232 = {LEER, LEER, LEER, 0};

/**
 * @brief   Dient zum Zaehlen vergeblicher Schreibversuche in den 
 *          Shared Memory zum SSC-Treiber.
 */
static byte counterSSC = 0;

/**
 * @brief   Dient zum Zaehlen vergeblicher Schreibversuche in den 
 *          Shared Memory zum RS232-Treiber.
 */
static byte counterRS232 = 0;

/**
 * @brief   Dient zum Zaehlen der Streckenbefehl-Vergleich-Versuche  
 */
static byte streckenbefehleUngleich = 0;

/**
 * @brief   Dient zum Speichern des Streckenbefehls aus dem Shared 
 *          Memory von der Befehlsvalidierung. 
 */
static Streckenbefehl internerStreckenbefehl = {LEER, LEER, LEER, 0};

/**
 * @brief   Dient zum Speichern des Streckenbefehls aus dem Shared 
 *          Memory vom SSC-Treiber.
 */ 
static Streckenbefehl externerStreckenbefehl = {LEER, LEER, LEER, 0};

/**
 * @brief   Gibt an, ob ein neuer Streckenbefehl von der Befehlsvalidierung 
 *          vorhanden ist. 
 */
static boolean isBVNew = FALSE;

/**
 * @brief   Gibt an, ob ein neuer Sreckenbefehl vom SSC-Treiber vorhanden ist. 
 */
static boolean isSSCNew = FALSE;

/**
 * @brief   Nachricht, die an das Auditing System gesendet wird.  
 */
static byte EV_nachricht[6];

// Prototypen fuer lokale Funktionen:
static boolean  compareTracks(Streckenbefehl *track1, Streckenbefehl *track2);
static boolean  isStreckenbefehlResetted(Streckenbefehl *track);
static boolean  processExternalStreckenbefehl(void);
static boolean  streckenbefehleEqual(Streckenbefehl *track1, Streckenbefehl *track2);
static void     processInternalStreckenbefehl(void);
static void     resetStreckenbefehl(Streckenbefehl *track);
static void     sendNachricht(Element element, Art art, byte data1, byte data2, byte data3);

void initEV(void);
void workEV(void);

/**
 * @brief   Initialisierung aller benoetigten Variablen des Moduls.
 */
void initEV(void)
{
	counterSSC      = 0;
	counterRS232    = 0;

	streckenbefehleUngleich = 0;

	internerStreckenbefehl.Lok          = LEER;
	internerStreckenbefehl.Weiche       = LEER;
	internerStreckenbefehl.Entkoppler   = LEER;
	internerStreckenbefehl.Fehler       = 0;

	externerStreckenbefehl.Lok          = LEER;
	externerStreckenbefehl.Weiche       = LEER;
	externerStreckenbefehl.Entkoppler   = LEER;
	externerStreckenbefehl.Fehler       = 0;

	EV_nachricht[0] = 0;
	EV_nachricht[1] = 0;
	EV_nachricht[2] = 0;
	EV_nachricht[3] = 0;
	EV_nachricht[4] = 0;
	EV_nachricht[5] = 0;

	oldSSC.Lok          = LEER;
	oldSSC.Weiche       = LEER;
	oldSSC.Entkoppler   = LEER;
	oldSSC.Fehler       = 0;

	oldRS232.Lok        = LEER;
	oldRS232.Weiche     = LEER;
	oldRS232.Entkoppler = LEER;
	oldRS232.Fehler     = 0;
}

/**
 * @brief   Dient zum senden von Nachrichten an das Auditing System.  
 *
 * @param   element	Art des Typs, der den Fehler geworfen hat
 *
 * @param   art     Art der Nachricht (Info, Warnung, Fehler)
 *
 * @param   data1   Erster Datensatz z.B. Streckenbefehl.Lok, Zaehlerwert.
 *
 * @param   data2	Zweiter Datensatz z.B. Streckenbefehl.Weiche, Streckenbefehl.Fehler.
 *
 * @param   data3   Dritter Datensatz z.B. Streckenbefehl.Entkoppler.
 */
static void sendNachricht(Element element, Art art, byte data1, byte data2, byte data3)      //@todo: Besser const Referenzen (&) verwenden.
{
	EV_nachricht[0] = element;
	EV_nachricht[1] = art;
	EV_nachricht[2] = data1;
	EV_nachricht[3] = data2;
	EV_nachricht[4] = data3;
	EV_nachricht[5] = 0;

	// Nachricht an das Auditing System uebermitteln
	sendMsg(MODUL_EV, EV_nachricht);
}


/**
 * @brief   Ueberprueft, ob der uebergebene Streckenbefehl komplett
 *          zurueckgesetzt ist.
 *
 * @param   track Zu ueberpruefender Streckenbefehl.
 *
 * @return  Gibt true zurueck, wenn der Streckenbefehl zurueck gesetzt
 *          ist. Andernfalls false.
 */
static boolean isStreckenbefehlResetted(Streckenbefehl *track)          //@todo: Besser const Referenzen verwenden.
{
	// Ueberpruefung, ob der Streckenbefehl zurueckgesetzt ist.
	// Der Fehler ist an dieser Stelle irrelevant.
	if  (track->Lok != LEER 
        || track->Weiche != LEER 
        || track->Entkoppler != LEER)
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


/**
 * @brief   Setzt den gegebenen Streckenbefehl zurueck. 
 *
 * @param   track Streckenbefehl der zurueck gesetzt wird.       
 */
static void resetStreckenbefehl(Streckenbefehl *track)      //@todo: Besser Referenzen verwenden.
{
	// Streckenbefehl zuruecksetzen
	track->Lok          = LEER;
	track->Weiche       = LEER;
	track->Entkoppler   = LEER;
	track->Fehler       = 0;
}


/**
 * @brief   Ueberprueft, ob die uebergebenen Streckenbefehle auf Gleichheit.
 *
 * @param   track1   1. Streckenbefehl
 *
 * @param   track2   2. Streckenbefehl
 *
 * @return  Gibt true zurueck, wenn die Streckenbefehl identisch sind. 
 *          Andernfalls false.
 */
static boolean streckenbefehleEqual(Streckenbefehl *track1, Streckenbefehl *track2)     //@todo: Besser const Referenzen verwenden.
{
	// Ueberpruefung, ob jeweils ein neuer Streckenbefehl von der
	// Befehlsvalidierung und dem SSC-Treiber vorhanden sind
	if  (isBVNew && isSSCNew)
	{
		// Zuruecksetzen des Zaehlers, 
		streckenbefehleUngleich = 0;
		// Streckenbefehle vergleichen
		return compareTracks(track1, track2);
	}

	// Ueberpruefung, ob die uebergebenen Streckenbefehle leer sind,
	// falls beide neu sind
	if (!isStreckenbefehlResetted(track1) && !isStreckenbefehlResetted(track2))
	{
		// Streckenbefehle vergleichen
		if  (compareTracks(track1, track2))
		{
			// Zuruecksetzen des Zaehlers
			streckenbefehleUngleich = 0;
			return TRUE;
		}
	}
	
	return FALSE;
}

/**
 * @brief   Ueberprueft, ob die uebergebenen Streckenbefehle auf Gleichheit.
 *
 * @param   track1   1. Streckenbefehl
 *
 * @param   track2   2. Streckenbefehl
 *
 * @return  Gibt true zurueck, wenn die Streckenbefehl identisch sind. 
 *          Andernfalls false.
 */
static boolean compareTracks(Streckenbefehl *track1, Streckenbefehl *track2)            //@todo: Besser const Referenzen (&) verwenden.
{
	// Vergleichen der beiden Streckenbefehle
	if  (track1->Lok == track2->Lok 
	    && track1->Weiche == track2->Weiche
	    && track1->Entkoppler == track2->Entkoppler)
	{
		// und die Funktion mit dem Wert TRUE verlassen.
		return TRUE;
	}
	
	// Sind die Streckenbefehle ungleich, wird ueberprueft, wie oft
	// die Streckenbefehle bereits voneinander abwichen.
	if  (streckenbefehleUngleich < MAXSTRECKENBEFEHLEUNGLEICH)
	{
		sendNachricht(E_STRECKENBEFEHLEUNGLEICH, A_WARNING, streckenbefehleUngleich, 0, 0);
		// Ist der Wert kleiner 3 der Zaehler inkrementiert ...
		streckenbefehleUngleich++;
		// und die Funktion mit dem Wert FALSE verlassen, um das
		// Modul fruehzeitig zu beenden.
		return FALSE;
	}
	
	// Sind die Streckenbefehle ungleich und hat der Zaehler einen Wert
	// groesser gleich 3, wird ein Fehlercode an das Auditing System 
	// uebermittelt, ...
	sendNachricht(E_STRECKENBEFEHLEUNGLEICH, A_FEHLER, MAXSTRECKENBEFEHLEUNGLEICH + 1, 0, 0);
	// ein Not-Aus eingeleitet und ...
	emergency_off();
	// die Funktion mit dem Wert FALSE verlassen, um das Modul
	// fruehzeitig zu beenden. 
	return FALSE;
}


/**
 * @brief   Überprüfung der Kommunikation mit den Treibern. 
 *
 *          Ueberprueft, ob bei der Kommunikation mit den Treibern
 *          Fehler aufgetreten sind. Ist dies der Fall soll das 
 *          Modul beendet werden.
 *
 * @return  TRUE wenn Kommunikationsfehler gefunden wurden,
 *          FALSE falls nicht.
 */
static boolean checkForCommunicationErrors(void)
{
	// Ueberpruefung, ob der Fehleranteil des Shared Memory vom
	// RS232-Treiber zurueckgesetzt ist.
	if  (RS232_EV_streckenbefehl.Fehler != 0)
	{
		// Falls nicht, wird der Fehlercode an das Auditing System
		// uebermittelt, ...
		sendNachricht(E_RS232_ERROR, A_FEHLER, 0, RS232_EV_streckenbefehl.Fehler, 0);
		// ein Not-Aus eingeleitet ...
		emergency_off();
		// und die Funktion mit dem Wert TRUE verlassen. 
		// Es wird nicht auf Fehler ueberprueft.
		return TRUE;
	} 

	// Ueberpruefung, ob der Fehleranteil des Shared Memory vom
	// SSC-Treiber zurueckgesetzt ist.
	if  (SSC_EV_streckenbefehl.Fehler != 0)
	{
		// Falls nicht, wird der Fehlercode an das Auditing System
		// uebermittelt, ...
		sendNachricht(E_SSC_ERROR, A_FEHLER, 0, SSC_EV_streckenbefehl.Fehler, 0);
		// ein Not-Aus eingeleitet ...
		emergency_off();
		// und die Funktion mit dem Wert TRUE verlassen. 
		// Es wird nicht auf Fehler ueberprueft.
		return TRUE;
	}

	// Ueberpruefung, ob beim letzten Modulaufruf der interne
	// Streckenbefehl in den Shared Memory zum RS232-Treiber
	// geschrieben wurde. Dann waere counterRS232 == 0. 	
	if (counterRS232 > 0)
	{
		// Falls nicht wird ueberprueft, ob der Shared Memory zum
		// RS232-Treiber in der Zwischenzeit zurueckgesetzt wurde.
		if (isStreckenbefehlResetted(&EV_RS232_streckenbefehl))
		{
			// Falls ja, wird der interne Streckenbefehl in 
			// diesen geschrieben, ...
//			 EV_RS232_streckenbefehl = internerStreckenbefehl;
			// der interne Streckenbefehl zurueckgesetzt ...
//			resetStreckenbefehl(&internerStreckenbefehl);
			EV_RS232_streckenbefehl = oldRS232;
			resetStreckenbefehl(&oldRS232);
			// und der Zaehler zurueckgesetzt.
			counterRS232 = 0;

		// Falls der Shared Memory zum RS232-Treiber nicht 
		// zurueckgesetzt ist, wird ueberprueft, wie oft vergeblich
		// versucht wurde in diesen zu schreiben.
		} 
        else if (counterRS232 < MAXRS232COUNTER)
		{
			sendNachricht(E_RS232_COUNTER, A_WARNING, counterRS232, 0, 0);
			// Falls die Versuche einen Wert kleiner 3 aufweisen,
			// wird dieser inkrementiert.
			counterRS232++;

		// Falls der Shared Memory zum RS232-Treiber nicht 
		// zurueckgesetzt ist und schon 3 oder mehr Male versucht
		// wurde in diesen zu schreiben, ...
		} 
        else 
		{
			// wird ein Fehlercode an das Auditing System 
			// uebermittelt, ...
			sendNachricht(E_RS232_COUNTER, A_FEHLER, MAXRS232COUNTER + 1, 0, 0);
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
	if (counterSSC > 0)
	{
		// Falls nicht wird ueberprueft, ob der Shared Memory zum
		// RS232-Treiber in der Zwischenzeit zurueckgesetzt wurde.
		if (isStreckenbefehlResetted(&EV_SSC_streckenbefehl))
		{
			// Falls ja, wird der interne Streckenbefehl in 
			// diesen geschrieben, ...
//			EV_SSC_streckenbefehl = internerStreckenbefehl;
			// der interne Streckenbefehl zurueckgesetzt ...
//			resetStreckenbefehl(&internerStreckenbefehl);
			EV_SSC_streckenbefehl = oldSSC;
			resetStreckenbefehl(&oldSSC);
			// und der Zaehler zurueckgesetzt.
			counterSSC = 0;

		// Falls der Shared Memory zum SSC-Treiber nicht 
		// zurueckgesetzt ist, wird ueberprueft, wie oft vergeblich
		// versucht wurde in diesen zu schreiben.
		} 
        else if (counterSSC < MAXSSCCOUNTER)
		{
			sendNachricht(E_SSC_COUNTER, A_WARNING, counterSSC, 0, 0);
			// Falls die Versuche einen Wert kleiner 3 aufweisen,
			// wird dieser inkrementiert.
			counterSSC++;
		
		// Falls der Shared Memory zum SSC-Treiber nicht 
		// zurueckgesetzt ist und schon 3 oder mehr Male versucht
		// wurde in diesen zu schreiben, ...
		} 
        else 
		{
			// wird ein Fehlercode an das Auditing System 
			// uebermittelt, ...
			sendNachricht(E_SSC_COUNTER, A_FEHLER, MAXSSCCOUNTER + 1, 0, 0);
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


/**
 * @brief   Ueberprueft den Shared Memory zur Befehlsvalidierung.   
 *
 *          Falls dieser neu ist, wird er in den Shared Memory
 *          zum SSC-Treiber geschrieben.     
 */
static void processInternalStreckenbefehl(void)
{
	isBVNew = FALSE;

	// Neuer Streckenbefehl im Shared Memory von der Befehlsvalidierung?
	if (isStreckenbefehlResetted(&BV_EV_streckenbefehl))
	{
		// Falls kein neuer Streckenbefehl vorhanden ist,
		// wird die Funktion verlassen
		return;
	}

	// Shared Memory von der Befehlsvalidierung in lokaler Variable 
	// fuer spaeter Ueberpruefungen speichern
	internerStreckenbefehl = BV_EV_streckenbefehl;
	
	// Shared Memory von der Befehlsvalidierung zuruecksetzen
	resetStreckenbefehl(&BV_EV_streckenbefehl);
	
	// Signalisieren, dass ein neuer Streckenbefehl vorhanden ist
	isBVNew = TRUE;

	// Ueberpruefung ob ein alter Streckenbefehl nicht gesendet werden konnte
//	if (counterSSC == 0)
	if (isStreckenbefehlResetted(&oldSSC))
	{
		// Ueberpruefung, ob der Shared Memory zum SSC-Treiber 
		// zurueckgesetzt ist.
		if (isStreckenbefehlResetted(&EV_SSC_streckenbefehl))
		{
			sendNachricht(
				E_SSC_COUNTER, 
				A_INFO, 
				internerStreckenbefehl.Lok, 
				internerStreckenbefehl.Weiche, 
				internerStreckenbefehl.Entkoppler
				);
			// Falls ja kann der interne Streckenbefehl in diesen 
			// geschrieben werden.
			EV_SSC_streckenbefehl = internerStreckenbefehl;
		} 
        else 
		{
			oldSSC = internerStreckenbefehl;
			sendNachricht(E_SSC_COUNTER, A_WARNING, counterSSC, 0, 0);
			// Falls nicht, wird der Wert der Zaehlvariable erhoeht.
			counterSSC = 1;
		}
	} 
    else 
	{
		// Fehler da ein alter Streckenbefehl nicht gesendet wurde
		sendNachricht(
    			E_SSC_COUNTER, 
    			A_FEHLER, 
    			internerStreckenbefehl.Lok, 
    			internerStreckenbefehl.Weiche, 
    			internerStreckenbefehl.Entkoppler
			);
		
		// Not-Aus einleiten
		emergency_off();
	}

}


/**
 * @brief   Ueberprueft, ob ein neuer Streckenbefehl im Shared Memory 
 *		    vorhanden ist. 
 *
 *          Falls ja wird dieser in eine interne Variable und setzt 
 *          den Shared Memory zurueck.
 *
 * @return  TRUE falls der externe und/oder der interne Streckenbefehl 
 *		    neu sind.
 *		    FALSE falls weder der externe noch der interne 
 *		  	Streckenbefehl neu sind. 
 *
 */
static boolean processExternalStreckenbefehl(void)
{
	isSSCNew = FALSE;

	// Neuer Streckenbefehl im Shared Memory vom SSC-Treiber?
	if (!isStreckenbefehlResetted(&SSC_EV_streckenbefehl))
	{
		// Falls ja Shared Memory vom SSC-Treiber in lokaler 
		// Variable fuer spaeter Ueberpruefungen speichern
		externerStreckenbefehl = SSC_EV_streckenbefehl;

		// Shared Memory vom SSC-Treiber zuruecksetzen
		resetStreckenbefehl(&SSC_EV_streckenbefehl);

		// Signalisieren, dass ein neuer Streckenbefehl vorhanden ist
		isSSCNew = TRUE;

		// Funktion mit Wert TRUE verlassen
		return TRUE;
	}

	// Falls sich kein neuer Streckenbefehl im Shared Memory befindet,
	// wird ueberprueft, ob der interne Streckenbefehl neu ist.
	if (isBVNew)
	{
		// Falls ja wird die Funktion mit dem Wert TRUE verlassen
		return TRUE;
	}

	// Falls weder der externe noch der interne Streckenbefehl neu sind,
	// wird die Funktion mit dem Wert FALSE verlassen.
	return FALSE;
}


/**
 * @brief   Schnittstelle zum Modul bz. Startet die Ergebnisvalidierung.
 *
 *          Diese Funktion wird von der Betriebsmittelsteuerung 
 *          aufgerufen. Sie stellt die gesamte Funktionalitaet des
 *          Moduls Ergebnisvalidierung bereit. 
 */
void workEV(void)
{
/* 
* Verarbeitungsschritt 1:
* Auf Kommunikationsprobleme pruefen 
*/
	// Als erstes wird ueberprueft, ob bei der Kommunikation mit den
	// Treibern Fehler oder Probleme aufgetreten sind.
	if (checkForCommunicationErrors())
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
	if (!processExternalStreckenbefehl())
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
	if (!streckenbefehleEqual(&internerStreckenbefehl, &externerStreckenbefehl))
	{
		// Sind diese nicht gleich, wird das Modul verlassen.
		return;
	}

/*
* Verarbeitungsschritt 5: 
* Senden des internen Streckenbefehls an den RS232-Treiber 
*/
	// Ueberpruefung ob ein alter Streckenbefehl nicht gesendet werden konnte
//	if (counterRS232 == 0)
	if (isStreckenbefehlResetted(&oldRS232))
	{
		// Ueberpruefung, ob der Shared Memory zum RS232-Treiber 
		// zurueckgesetzt ist.
		if (isStreckenbefehlResetted(&EV_RS232_streckenbefehl))
		{
			sendNachricht(
    				E_RS232_COUNTER, 
    				A_INFO, 
    				internerStreckenbefehl.Lok, 
    				internerStreckenbefehl.Weiche, 
    				internerStreckenbefehl.Entkoppler
				);
			// Falls ja kann der interne Streckenbefehl in diesen 
			// geschrieben werden.
			EV_RS232_streckenbefehl = internerStreckenbefehl;
		} 
        else 
		{
			oldRS232 = internerStreckenbefehl;

			sendNachricht(E_RS232_COUNTER, A_WARNING, counterRS232, 0, 0);
			// Falls nicht, wird der Wert der Zaehlvariable erhoeht ...
			counterRS232 = 1;
			// und das Modul verlassen.
			return;
		}
	} 
    else 
	{
		// Fehler senden, da ein alter Streckenbefehl nicht gesendet werden konnte
		sendNachricht(
    			E_RS232_COUNTER, 
    			A_FEHLER, 
    			internerStreckenbefehl.Lok, 
    			internerStreckenbefehl.Weiche, 
    			internerStreckenbefehl.Entkoppler
			);
		
		// Not-Aus einleiten
		emergency_off();
	}

/*
* Verarbeitungsschritt 6:
* Interne Variablen zuruecksetzen
*/
	initEV();
}
