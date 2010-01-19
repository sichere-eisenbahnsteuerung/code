/*****************************************************************************
 *
 *        Dateiname:    SSCTreiber.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Matthias Buss	
 *        Datum:        13.01.2010
 *
 *        Modul:        SSC, 0.8
 *
 *        Beschreibung:
 *        Das SSC-Modul sorgt fuer die Kommunikation zwischen den
 *  	  redudanten Mikrocontrollern
 *
 ****************************************************************************/

/* Includes *****************************************************************/
#include "SSCTreiber.h"
#include "Betriebsmittelverwaltung.h"


/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/
byte byteToSend = 1;
byte byteToReceive = 1;

/// die Variable status speichert den Fortschritt des Moduls (zu erweitern) bzw den Punkt, 
/// an dem das Modul den Token an die Betriebsmittelverwaltung abgegeben hat,
/// um an dieser Stelle wieder weiter zu arbeiten
/// 0: 	startpunkt
/// 10:	das Modul wartet auf die Freigabe der Variablen SSC_EV_streckenbefehl, um einen Streckenbefehl zu schreiben
byte status = 0;

/// die empfangenen Byte des Streckenbefehls werden in diesem Array zwischengespeichert,
/// solange der Streckenbefehl noch nicht vollständig empfangen und auf Gültigkeit geprüft wurde
byte temp_streckenbefehl[3];

/* Prototypen fuer lokale Funktionen ****************************************/
void streckenbefehlAn_EVsenden();
void datenLesen();
void kollisionVerarbeiten();

/* Funktionsimplementierungen ***********************************************/

/// Erhöht das byteToSend, wobei byteToSend nur die Werte 1,2 und 3 annehmen darf
void byteToSendInkrementieren()
{
	switch (byteToSend)
	{
		case 1:
			byteToSend = 2;
			break;
		
		case 2:
			byteToSend = 3;
			break;
			
		case 3:
			byteToSend = 1;
			break;
		
		default: ;
	}
}

/// Verringert das byteToSend, wobei byteToSend nur die Werte 1,2 und 3 annehmen darf
void byteToSendDekrementieren()
{
	switch (byteToSend)
	{
		case 1:
			byteToSend = 3;
			break;
		
		case 2:
			byteToSend = 1;
			break;
			
		case 3:
			byteToSend = 2;
			break;
		
		default: ;
	}
}

/// Erhöht das byteToReceive, wobei byteToReceive nur die Werte 1,2 und 3 annehmen darf
void byteToReceiveInkrementieren()
{
	switch (byteToReceive)
	{
		case 1:
			byteToReceive = 2;
			break;
		
		case 2:
			byteToReceive = 3;
			break;
			
		case 3:
			byteToReceive = 1;
			break;
		
		default: ;
	}
}

/// Verringert das byteToReceive, wobei byteToReceive nur die Werte 1,2 und 3 annehmen darf
void byteToReceiveDekrementieren()
{
	switch (byteToReceive)
	{
		case 1:
			byteToReceive = 3;
			break;
		
		case 2:
			byteToReceive = 1;
			break;
			
		case 3:
			byteToReceive = 2;
			break;
		
		default: ;
	}
}

/// Streckenbefehl (globale Variable "EV_SSC_streckenbefehl") prüfen, 
/// wenn lauter Einsen, dann kein neuer Streckenbefehl sonst ja
boolean checkNeuerStreckenbefehl()
{	
	if(EV_SSC_streckenbefehl.Lok == 255 && EV_SSC_streckenbefehl.Weiche == 255 && EV_SSC_streckenbefehl.Entkoppler == 255) 
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/// bei gelesenem Streckenbefehl wird true uebergeben, bei zu sendendem false
void befehlAufGueltigkeitPruefen(boolean lesen) 
{
	//zu lesender Streckenbefehl
	if(lesen)
	{		
		if (temp_streckenbefehl[0] == 255 && temp_streckenbefehl[1] == 255 && temp_streckenbefehl[2] == 255)
		{				
			//kritischer Fehler (128 - 255) setzen
			SSC_EV_streckenbefehl.Fehler = 255;
		}
		
		streckenbefehlAn_EVsenden();
	
	}
	/*  //Pruefung, ob nur Einsen vorhanden macht wenig Sinn. Fuer Erweitung erstmal nicht gelöscht
	else//zu sendender Streckenbefehl
	{
		
		if (EV_SSC_streckenbefehl.Lok == 255 && EV_SSC_streckenbefehl.Weiche == 255 && EV_SSC_streckenbefehl.Entkoppler)
		{	
			EV_SSC_streckenbefehl.Fehler;
		}
	}*/

}

void streckenbefehlAn_EVsenden()
{
	//pruefen, ob SSC_EV_streckenbefehl zum Ueberschreiben freigegeben wurde
	if(SSC_EV_streckenbefehl.Lok == 255 &&  SSC_EV_streckenbefehl.Weiche == 255 && SSC_EV_streckenbefehl.Entkoppler == 255)
	{
		SSC_EV_streckenbefehl.Lok = temp_streckenbefehl[0];
		SSC_EV_streckenbefehl.Weiche = temp_streckenbefehl[1];
		SSC_EV_streckenbefehl.Entkoppler = temp_streckenbefehl[2];
		SSC_EV_streckenbefehl.Fehler = 0;
		status = 0;
	}
	
	//wenn nicht freigegeben, Wartezustand speichern
	else
	{
		status = 10;
	}
}

/// wird bei einem SSC interrupt aufgerufen,
/// wobei entweder Daten empfangen oder eine Kollision festgestellt wurde
SSCinterrupt() interrupt 18
{	
	//Ueberpruefen, welcher Interrupt vorliegt und danach entsprechnete Funktion aufrufen

	//bitweise UND des TC (LSB) und 00000001
	if (SCF & 0x01 )
	{		
		datenLesen();
	}

	//bitweise UND des WCOL und 00000010
	if (SCF & 0x02)
	{
		kollisionVerarbeiten();		
	}
}

/// setzt nach einer Kollision die Nummer der zu empfangenden und des zu sendenden Bytes herab, 
/// damit dieses beim nächsten Aufruf von work() erneut gesendet bzw. empfangen werden kann
void kollisionVerarbeiten()
{	
	byteToSendDekrementieren();
	byteToReceiveDekrementieren();
	
	//Kollisionsbit (WCOL) zuruecksetzen
	if (SCF & 0x01)	//TC gesetzt
	{
		SCF = 0x01;
	}
	else
	{
		SCF = 0x00;
	}
	
}

/// liest das empfangene Byte ein
void datenLesen()
{
	
	switch(byteToReceive)
	{
		case 1:			
			temp_streckenbefehl[0] = SRB;
			byteToReceiveInkrementieren();			
			break;
		
		case 2:			
			temp_streckenbefehl[1] = SRB;
			byteToReceiveInkrementieren();				
			break;
		
		case 3: 			
			temp_streckenbefehl[2] = SRB;
			byteToReceiveInkrementieren();	
			befehlAufGueltigkeitPruefen(TRUE);							
			break;
	}
	
	//Transmission Compleded-Bit zuruecksetzen
	if (SCF & 0x02) //Kollisionsbit (WCOL) gesetzt
	{
		SCF = 0x02;
	}
	else
	{
		SCF = 0x00;
	}
		
}

/*boolen byteAufGueltigkeitPruefen(Byte a)
{
	//a enthält nur Einsen
	if(a == 255)
	{
		return false;
	}
	else
	{
		return true;
	}
	
}*/


void datenSenden()
{
	//checkByteToSend
	switch(byteToSend)
	{
		//erstes Byte
		case 1:	
			if(checkNeuerStreckenbefehl())
			{										  								
				//erstes Byte senden
				STB = EV_SSC_streckenbefehl.Lok;
				byteToSendInkrementieren();
			}
			break;
		
		//zweites Byte
		case 2:	
			//zweites Byte senden
			STB = EV_SSC_streckenbefehl.Weiche;
			byteToSendInkrementieren();
			break;
		
		//drittes Byte
		case 3:	
			//drittes Byte Senden
			STB = EV_SSC_streckenbefehl.Entkoppler;
			byteToSendInkrementieren();
			
			//Sichergehen, dass keine Kollision aufgetreten ist, die das byteToSend wieder auf 3 gesetzt hat.
			//Dann wurde das Byte erfolgreich übertragen und es kann die globale Variable mit einem neuen Streckenbefehl überschrieben werden
			if (byteToSend == 1)
			{
				EV_SSC_streckenbefehl.Lok = 255;
				EV_SSC_streckenbefehl.Weiche = 255;
				EV_SSC_streckenbefehl.Entkoppler = 255;
				EV_SSC_streckenbefehl.Fehler = 0;
			}
			break;
				
		default:
			;//Fehlermeldung oder gar nichts
	}
}

/// Hauptmethode des Moduls,
/// wid als Schnittstelle von außerhalb aufgerufen
void workSSC()
{
	switch (status)
	{
		case 0:
			datenSenden();
			break;
		
		case 10:
			streckenbefehlAn_EVsenden();
			break;
			
		default:
			datenSenden();
			break;
	}

	//SW-watchdog noch zu prüfen
	//hello(4, status);	
			
}
	
void initSSC()
{
	//SSC Einstellungen in Master Mode 
	/*
	SCEN = 1		SSC enabled
	TEN  = 1		Transmitter enabled
	MSTR = 1		Master Mode
	CPOL = 0		SCLK low
	CPHA = 1		shift, ...
	BRS2 = 0
	BRS1 = 0
	BRS2 = 1		2.5 Mbaud
	*/			
	SSCCON = 0xE9;
	
	//interrupts aktivieren
	SCIEN = 0x03;
}


/*void main (void)
{

}*/
