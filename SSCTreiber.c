/*****************************************************************************
 *
 *        Dateiname:    SSC.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Matthias Buß	
 *		  Datum:		11.01.2010
 *
 *        Modul:        SSC, 0.7
 *
 *        Beschreibung:
 *        Das SSC-Modul sorgt für die Kommunikation zwischen den
 *  	  redudanten Mikrocontrollern
 *
 ****************************************************************************/

/* Includes *****************************************************************/
#include <reg515C.h>
#include <stdio.h>

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/
Byte byteToSend = 1;
Byte byteToReceive = 1;

/// die Variable status speichert den Fortschritt des Moduls bzw den Punkt, 
///an dem das Modul den Token an die Betriebsmittelverwaltung abgegeben hat,
///um an dieser Stelle wieder weiter zu arbeiten
/// 0: 	startpunkt
/// 10:	das Modul wartet auf die Freigabe der Variablen SSC_EV_streckenbefehl, um einen Streckenbefehl zu schreiben
int status = 0;

///die empfangenen Byte des Streckenbefehls werden in diesem Array zwischengespeichert,
///solange der Streckenbefehl noch nicht vollständig empfangen und auf Gültigkeit geprüft wurde
Byte temp_streckenbefehl[3];

/* Prototypen fuer lokale Funktionen ****************************************/

/* Funktionsimplementierungen ***********************************************/

///Erhöht das byteToSend, wobei byteToSend nur die Werte 1,2 und 3 annehmen darf
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

///Verringert das byteToSend, wobei byteToSend nur die Werte 1,2 und 3 annehmen darf
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

///Erhöht das byteToReceive, wobei byteToReceive nur die Werte 1,2 und 3 annehmen darf
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

///Verringert das byteToReceive, wobei byteToReceive nur die Werte 1,2 und 3 annehmen darf
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

///Streckenbefehl (globale Variable "EV_SSC_streckenbefehl") prüfen, 
///wenn lauter Einsen, dann kein neuer Streckenbefehl sonst ja
boolean checkNeuerStreckenbefehl()
{
	if(EV_SSC_streckenbefehl == 11111111 11111111 11111111) 
	{
		return false;
	}
	else
	{
		return true;
	}
}

///bei gelesenem Streckenbefehl wird true übergeben, bei zu sendendem false
void befehlAufGueltigkeitPruefen(boolean lesen) 
{
	//zu lesender Streckenbefehl
	if(lesen)
	{
		//pseudocode implementieren
		if (temp_streckenbefehl[] == gültig)
		{				
			streckenbefehlAN_EVsenden()
		}
		else
		{
			//pseudocode
			<kritischeFehler> setzen;
		}
	
	}
	else//zu sendender Streckenbefehl
	{
		//pseudocode implementieren
		if (EV_SSC_streckenbefehl == ungültig)
		{	
			<kritischeFehler> setzen;
		}
	}

}

void streckenbefehlAn_EVsenden()
{
	//prüfen, ob SSC_EV_streckenbefehl zum Überschreiben freigegeben wurde
	if(SSC_EV_streckenbefehl == 11111111 11111111 1111111)
	{
		SSC_EV_streckenbefehl = temp_streckenbefehl[];
		status = 0;
	}
	else
	{
		status = 10;
	}
}

///wird bei einem SSC interrupt aufgerufen,
///wobei entweder Daten empfangen oder eine Kollision festgestellt wurde
SSCinterrupt() interrupt 18
{
	//überprüfen, welcher Interrupt vorliegt und danach entsprechnete Funktion aufrufen
	if (<TC> = 1)
	{		
		datenLesen();
	}
	if (<WCOL> = 1)
	{
		kollisionVerarbeiten();		
	}
}

///setzt nach einer Kollision die Nummer der zu empfangenden und des zu sendenden Bytes herab, 
///damit dieses beim nächsten Aufruf von work() erneut gesendet bzw. empfangen werden kann
void kollisionVerarbeiten()
{
	//evtl WCOL zurücksetzen
	byteToSendDekrementieren();
	byteToReceiveDekrementieren();
}

///liest das empfangene Byte ein
void datenLesen()
{
	//evtl TC zurücksetzten ?!
	switch(byteToReceive)
	{
		case 1:
			if(byteAufGueltigkeitPruefen(SRB))
			{
				temp_steckenbefehl[0] = SRB;
				byteToReceiveInkrementieren();
			}
			break;
		
		case 2:
			if(byteAufGueltigkeitPruefen(SRB))
			{
				temp_steckenbefehl[1] = SRB;
				byteToReceiveInkrementieren();	
			}
			else
			{
				//pseudocode korrekt umsetzen
				SSC_EV_streckenbefehl = temp_streckenbefehl[];
				<kritischeFehler> setzen
				byteToReceiveDekrementieren();
			}
			break;
		
		case 3: 
			if(byteAufGueltigkeitPruefen(SRB))
			{
				temp_steckenbefehl[2] = SRB;
				byteToReceiveInkrementieren();	
				befehlAufGueltigkeitPruefen(true);				
			}
			else
			{
				//pseudocode korrekt umsetzen
				
				SSC_EV_streckenbefehl = temp_streckenbefehl[];
				<kritischeFehler> setzen
				
				//byteToReceive auf 1 setzen; deshalb zweimal:
				byteToReceiveDekrementieren();
				byteToReceiveDekrementieren();
			}
				break;
	}
}

boolen byteAufGueltigkeitPruefen(Byte a)
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
	
}


void datenSenden()
{
	//checkByteToSend
	switch(byteToSend)
	{
		//erstes Byte
		case 1:	
			if(checkNeuerStreckenbefehl())
			{
				if(befehlAufGueltigkeitPruefen(false))
				{
					//erstes Byte senden
					STB = <EV_SSC_streckenbefehl_ByteEins>;
					byteToSendInkrementieren();
				}
			}
			break;
		
		//zweites Byte
		case 2:	
			//zweites Byte senden
			STB = <EV_SSC_streckenbefehl_ByteZwei>;
			byteToSendInkrementieren();
			break;
		
		//drittes Byte
		case 3:	
			//drittes Byte Senden
			STB = <EV_SSC_streckenbefehl_ByteDrei>;
			byteToSendInkrementieren();
			
			//Sichergehen, dass keine Kollision aufgetreten ist, die das byteToSend wieder auf 3 gesetzt hat.
			//Dann wurde das Byte erfolgreich übertragen und es kann die globale Variable mit einem neuen Streckenbefehl überschrieben werden
			if (byteToSend == 1)
			{
				EV_SSC_streckenbefehl = 11111111 11111111 11111111;
			}
			break;
				
		default://Fehlermeldung oder gar nichts
}

///Hauptmethode des Moduls,
///wid als Schnittstelle von außerhalb aufgerufen
void work()
{
	switch (status)
	{
		case 0:
			datenSenden();
			break;
		
		case 10:
			streckenbefehlAN_EVsenden();
			break;
			
		default:
			datenSenden();
			break;
	}

	hello(4, status);	
			
}
	
	
	

}
/*void main (void)
{

}*/
