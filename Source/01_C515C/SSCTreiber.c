/**
 * @file     SSCTreiber.c
 *
 * @author   Matthias Buss
 *   
 * @brief    Das SSC-Modul sorgt fuer die Kommunikation zwischen den
 *  	        redudanten Mikrocontrollern
 *
 * @date     13.01.2010
 */

// Includes
#include "SSCTreiber.h"
#include "Betriebsmittelverwaltung.h"

// Lokale Variablen 
byte byteToSend     = 1;
byte byteToReceive  = 1;

/**
 * Speichert den Fortschritt des Moduls bzw. den Punkt, an dem das Modul den Token an 
 * die Betriebsmittelverwaltung abgegeben hat. 
 *
 * 0  = Startpunkt/Datenfreigabe (Daten senden)
 * 10 = Das Modul wartet auf die Freigabe der Variabelen SSC_EV_streckenbefehl, um 
 *      Streckenbefehl und Daten an die Ergebnisvalidierung zu senden.
 */
byte status = 0;

// die empfangenen Byte des Streckenbefehls werden in diesem Array zwischengespeichert,
// solange der Streckenbefehl noch nicht vollstaendig empfangen und auf Gültigkeit geprueft wurde
byte temp_streckenbefehl[3];

// Prototypen fuer lokale Funktionen:
void streckenbefehlAn_EVsenden();
void datenLesen();
void kollisionVerarbeiten();


/**
 * @brief   Inkrementiert die lokale Variabel 'byteToSend'.
 *
 *          Erhöht den Wert von 'byteToSend' um eins. Dabei wechselt der Wert
 *          zwischen eins und drei. Ist der Wert von 'byteToSend' drei, wird  
 *          'byteToSend' wieder auf eins gesetzt. 
 *                  Bsp. 1 -> 2 -> 3 => 1 -> 2 -> 3 => 1 -> ...
 */
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
	
        default: ;// @TODO Hier sollte irgendetwas passieren z.B. eine Fehlermeldung;
    }
}


/**
 * @brief   Dekrementiert die lokale Variabel 'byteToSend'.
 *
 *          Verringert den Wert von 'byteToSend' um eins. Dabei wechselt der Wert
 *          zwischen drei und eins. Ist der Wert von 'byteToSend' eins, wird  
 *          'byteToSend' wieder auf drei gesetzt. 
 *                  Bsp. 3 -> 2 -> 1 => 3 -> 2 -> 1 => 3 -> ...
 */
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
		
        default: ;// @TODO Hier sollte irgendetwas passieren z.B. eine Fehlermeldung;
    }
}


/**
 * @brief   Inkrementiert die lokale Variabel 'bbyteToReceive'.
 *
 *          Erhöht den Wert von 'bbyteToReceive' um eins. Dabei wechselt der Wert
 *          zwischen eins und drei. Ist der Wert von 'byteToSend' drei, wird  
 *          'byteToSend' wieder auf eins gesetzt. 
 *                  Bsp. 1 -> 2 -> 3 => 1 -> 2 -> 3 => 1 -> ...
 */
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
		
        default: ;// @TODO Hier sollte irgendetwas passieren z.B. eine Fehlermeldung;
    }
}


/**
 * @brief   Dekrementiert die lokale Variabel 'bbyteToReceive'.
 *
 *          Verringert den Wert von 'byteToSend' um eins. Dabei wechselt der Wert
 *          zwischen drei und eins. Ist der Wert von 'bbyteToReceive' eins, wird  
 *          'bbyteToReceive' wieder auf drei gesetzt. 
 *                  Bsp. 3 -> 2 -> 1 => 3 -> 2 -> 1 => 3 -> ...
 */
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
		
        default: ;// @TODO Hier sollte irgendetwas passieren z.B. eine Fehlermeldung;
    }
}

/**
 * @brief   Prüft die globale Variable 'EV_SSC_streckenbefehl'.
 *
 *          Enthält die globale Variable 'EV_SSC_streckenbefehl' nur
 *          einsen, liegt keiner neuer Streckenbefehl an und die Funktion
 *          gibt false zurück. 
 *
 * @return  Gibt true zurück, wenn ein neuer Streckenbefehl anliegt. 
 *          Ansonsten false.
 */
boolean checkNeuerStreckenbefehl()
{	
    boolean neuerBefehl = TRUE;

    if(EV_SSC_streckenbefehl.Lok == 255 
        && EV_SSC_streckenbefehl.Weiche == 255 
        && EV_SSC_streckenbefehl.Entkoppler == 255) 
    {
        neuerBefehl = FALSE;
    }

    return (neuerBefehl);
}

/**
 * @brief   Prüft den anliegenden Strecknbefehl auf Gültigkeit.
 *
 *          Handelt es sich um den anliegenden Streckenbefehl um einen 
 *          zu lesende (gegeben durch den Übergabeparameter 'lesen'),
 *          wird der Streckenbefehl geprüft und ggf. die Ergebnisvalidierung, 
 *          funktion streckenbefehlAn_EVsenden(), aufgerufen, wenn der 
 *          Streckenbefehl gültig ist.
 *
 * @param   lesen Zu lesendener Streckenbefehlt (true) oder zu 
 *          sendenen Streckenbefehl (false).
 */
void befehlAufGueltigkeitPruefen(boolean lesen) 
{
	// Zu lesender Streckenbefehl
    if(lesen == TRUE)
    {		
		if (temp_streckenbefehl[0] == 255 
            && temp_streckenbefehl[1] == 255 
            && temp_streckenbefehl[2] == 255)
		{				
            //kritischer Fehler (128 - 255) setzen
            SSC_EV_streckenbefehl.Fehler = 255;
		}
		
        streckenbefehlAn_EVsenden();
	}
	/*  //Pruefung, ob nur Einsen vorhanden macht wenig Sinn. Fuer Erweitung erstmal nicht geloescht
	else//zu sendender Streckenbefehl
	{
		
		if (EV_SSC_streckenbefehl.Lok == 255 && EV_SSC_streckenbefehl.Weiche == 255 && EV_SSC_streckenbefehl.Entkoppler)
		{	
			EV_SSC_streckenbefehl.Fehler;
		}
	}*/
}


/**
 * @brief   Setzt das status bit. 
 *          
 *          Ist der Streckenbefehl freigegeben, wird die lokale Variable 'status'
 *          auf 0 gesetzt. Ansonsten auf 10.   
 */
void streckenbefehlAn_EVsenden() //@TODO: Name stimmt nicht mit Funktion überein.
{
    // Pruefen, ob SSC_EV_streckenbefehl zum Ueberschreiben freigegeben wurde
    if(SSC_EV_streckenbefehl.Lok == 255 
        &&  SSC_EV_streckenbefehl.Weiche == 255 
        && SSC_EV_streckenbefehl.Entkoppler == 255)
    {
        SSC_EV_streckenbefehl.Lok        = temp_streckenbefehl[0];
        SSC_EV_streckenbefehl.Weiche     = temp_streckenbefehl[1];
        SSC_EV_streckenbefehl.Entkoppler = temp_streckenbefehl[2];
        SSC_EV_streckenbefehl.Fehler     = 0;

        status = 0;
	}
    else // Wenn nicht freigegeben, Wartezustand speichern
    {
        status = 10;
    }
}

/**
 * @brief   Interrupt des SSC Treibers.
 *
 *          Wird bei einem SCC Interrupt aufgerufen. Je nach Interrupttyp 
 *          werden Daten empfangen oder eine Kollision verarbeitet. 
 */
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


/**
 * @brief   Verarbeitung einer anliegenden Interrupt Kollision.
 *
 *          Setzt nach einer Kollision die Nummer der zu empfangenden  
 *          und des zu sendenden Bytes herab, damit dieses beim nächsten 
 *          Aufruf von work() erneut gesendet bzw. empfangen werden kann 
 *
 * @param
 *
 * @return
 */
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

/**
 * @brief   Einlesen des empfangenen Bytes.
 *  
 *          Liest die die empfangenen Daten temporaer ein und setzt das 
 *          Statusbyte (inkrement 'byteToReceive'). Außerdem wird das 
 *          'Transmission Compleded-Bit' zurueck gesetzen.   
 */
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

        //@todo: Default Fall einbauen
    }
	
    //Transmission Compleded-Bit zuruecksetzen
    if (SCF & 0x02)     //Kollisionsbit (WCOL) gesetzt
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

/**
 * @brief   Senden von Daten. 
 *          
 *          Je nach Wert von byteToSend setzt die Funktion die entsprechenden
 *          globalen Daten z.B. byteToSend = 2, die Daten für die Weiche werden 
 *          gesetzt. 
 */
void datenSenden()
{
    //checkByteToSend      @todo: Vorher prüfen, ob Modul überhaupt initialisiert.
    switch(byteToSend)
    {
	    //erstes Byte
        case 1:	
            if(checkNeuerStreckenbefehl() == TRUE)
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
			
            /**
             *  Sichergehen, dass keine Kollision aufgetreten ist, die das byteToSend 
             *  wieder auf 3 gesetzt hat.
             *  Dann wurde das Byte erfolgreich Uebertragen und es kann die globale 
             *  Variable mit einem neuen Streckenbefehl Ã¼berschrieben werden.
             */
            if (byteToSend == 1)
            {
                EV_SSC_streckenbefehl.Lok        = 255;
                EV_SSC_streckenbefehl.Weiche     = 255;
                EV_SSC_streckenbefehl.Entkoppler = 255;
                EV_SSC_streckenbefehl.Fehler     = 0;
            }
            break;
				
        default:
                ;//@todo: Fehlermeldung oder gar nichts
	}
}


/**
 * @brief   Schnittstelle des Moduls, wird von außerhalb aufgerufen. 
 */
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

//@todo: SW-watchdog noch zu prüfen
//hello(4, status);		
}
	

/**
 * @brief   Initialisiert den SSC Treiber. 
 */
void initSSC()
{
	//SSC Einstellungen in Master Mode 
	/*
	SCEN = 1		SSC enabled
	TEN  = 1		Transmitter enabled
	MSTR = 1		Master Mode
	CPOL = 0		SCLK low
	CPHA = 1		shift, sample
	BRS2 = 0
	BRS1 = 1
	BRS2 = 0		1.25 Mbaud
	*/			
	SSCCON = 0xEA;
	
	//interrupt Konfiguration
	EAL = 1;
	//ESSC deaktivieren, damit SCIEN beschrieben werden darf
	IEN2 &= ~(1<<2);
	//interrupts aktivieren
	SCIEN = 0x03;
	//ESSC setzen (drittes Bit im IEN2 Register)
	IEN2 |= (1<<2);

	//Priorität festlegen, Level 2
	//setze IP0.2 auf 0
	IP0 &= (1<<2);

	//setze IP1.2 auf 1
	IP1 |= (1<<2);
}


/*void main (void)
{

}*/
