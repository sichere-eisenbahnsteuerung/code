/*****************************************************************************
 *
 *        Dateiname:    RS232Treiber.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Oliver Gatti
 *
 *
 *        Modul:        <RS232-Treiber>, <Version des Moduldesigns>
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
#include<stdio.h>
#include<RS232Treiber.h>
#include<Betriebsmittelverwaltung.h>

/* Definition globaler Konstanten *******************************************/
void konvertLok();
void konvertWeiche();
void konvertEntkoppler();

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/
static byte sbuf_buffer;
static byte receive_buffer[3];
static byte called_without_sent_answer = 0x00;
static byte bytes_to_receive = 0x00;
static byte li101_header_received = 0x00;
static byte ready_to_send = 0x01;
static byte cts_pin_false = 0x00;
static byte sende_counter = 0x00;
static byte sende_buffer[6];
static byte maxfailures = 0x05; // !!!!! Noch zu ergaezen im Moduldesign
static byte rs232_enabled; // !!!!! Noch zu ergaenzen im Moduldesign
// 1 Entkupplungsdekoder ist aktiv
// 0 Entkupplungsdekoder ist nicht aktiv 
static byte entkupplerActive = 0x01;  // !!!!! Noch zu ergaenzen im Moduldesign

/* Prototypen fuer lokale Funktionen ****************************************/

/* Funktionsimplementierungen ***********************************************/

DataReceivedInterrupt () interrupt 4 //Interruptfunktion
{
	if(RI == 1) 
	{
		//Es wurde zuvor ein Antwort-Header-Byte vom LI101F  empfangen
		if(li101_header_received == 0x01) 
		{
			receive_buffer[3 - bytes_to_receive] = SBUF;
			bytes_to_receive--;
			if(bytes_to_receive == 0x00) 
			{
				//Sendebestaetigung
				if(receive_buffer[1] == 0x04 && receive_buffer[2] == 0x05)
				{
					ready_to_send = 0x01;	
					li101_header_received = 0x00;
				}
				//Keine Sendebestaetigung
				else 
				{
					EV_RS232_streckenbefehl.Fehler = receive_buffer[1];		
				}
			}
		} 
		//Es wurde zuvor KEIN Antwort-Header-Byte vom L101F empfangen
		else 
		{
			sbuf_buffer = SBUF;
			//Li101 Header identifiziert
			if(sbuf_buffer == 0x01) 
			{
				receive_buffer[0] = sbuf_buffer;
				bytes_to_receive = 0x02;
				li101_header_received = 0x01;
			}
			else
			{
				//Keine Datenbytes mehr auszulesen => Neuer Header
				if(bytes_to_receive == 0x00) {
					bytes_to_receive = (0x1F & sbuf_buffer) + 1;
				}
				//Es sind noch Datenbytes zu empfangen
				else {
					bytes_to_receive--;
				}
			}
		}
		RI = 0;
	}
}

void workRS232() 
{
	//CTS Pin = 0x00 = LI101F ist empfangsbereit
	if(rs232_enabled == 0x00) 
	{
		EV_RS232_streckenbefehl.Lok = 0xFF;
		EV_RS232_streckenbefehl.Weiche = 0xFF;
		EV_RS232_streckenbefehl.Entkoppler = 0xFF;
		return;
	}
	
	//Prueft wie oft das Modul aufgerufen worden ist, ohne dass eine Sende-Bestaetigung des LI101F empfangen worden ist.
	if(ready_to_send == 0x00)
	{
		called_without_sent_answer++;
		if( called_without_sent_answer == maxfailures)
		{
			EV_RS232_streckenbefehl.Fehler = 0x04;
		}
		return;
	}
	
	called_without_sent_answer = 0x00;
	
	if(RS232TREIBER_CTSPIN == 0) 
	{
		cts_pin_false = 0x00;
		
		//Keine Daten mehr zu versenden
		if(sende_counter == 0x00) 
		{
			//Empfangsbestaetigung des letzten Befehls vom LI101F erhalten
			if(ready_to_send == 0x01) 
			{
				byte commandposition = 0xFF;
				//Lok Befehl
				if(EV_RS232_streckenbefehl.Lok != 0xFF) {
					commandposition = 0x00;
				}
				else
				{
					//Weichen-Befehl
					if(EV_RS232_streckenbefehl.Weiche != 0xFF) 
					{
						commandposition = 0x01;
					}
					else
					{	
						//Entkupplungs-Befehl
						if(EV_RS232_streckenbefehl.Entkoppler != 0xFF) 
						{
							commandposition = 0x02;
						}
					}
				}
				
				switch(commandposition)
				{
					//Lok Befehl
					case 0x00: 	konvertLok();
								break;
					
					//Weichen Befehl
					case 0x01: 	konvertWeiche();
								break;
					
					//Entkupplungs Befehl
					case 0x02: 	konvertEntkoppler();
								break;
					
					//Kein Befehl vorhanden
					case 0xFF: return;
				}	
			}
		}
		
		while(sende_counter > 0x00)
		{
			//CTS bereit
			if(RS232TREIBER_CTSPIN == 0) 
			{
				SBUF = sende_buffer[sende_counter-1];
				while(TI == 0) {
				}
				sende_counter--;
				TI = 0;
			}
			//CTS nicht bereit
			else 
			{
				int verzoegerung = 0;
				while(verzoegerung  < 2000)
				{
					if(RS232TREIBER_CTSPIN == 1) 
					{
						cts_pin_false++;
						//Maximaler Fehleranzahl erreicht?
						if(cts_pin_false == maxfailures)
						{
							EV_RS232_streckenbefehl.Fehler = 0x04;	
						}
						return;
					}
					verzoegerung++;
				}
			}
		}
		//Befehl abgeschickt, entsprechenden Eintrag im Shared-Memory auf 0xFF setzen
		if(EV_RS232_streckenbefehl.Lok != 0xFF) 
		{
			EV_RS232_streckenbefehl.Lok = 0xFF;
		}
		else
		{
			//Weichen-Befehl
			if(EV_RS232_streckenbefehl.Weiche != 0xFF) 
			{
				EV_RS232_streckenbefehl.Weiche = 0xFF;
			}
			else
			{	
				//Entkupplungs-Befehl
				if(EV_RS232_streckenbefehl.Entkoppler != 0xFF) 
				{
					if(entkupplerActive == 0x01)
					{
						entkupplerActive = 0x00;
					}
					else 
					{
						EV_RS232_streckenbefehl.Entkoppler = 0xFF;
					}
				}
			}
		}
		
	}
	//LI101F ist nicht empfangsbereit
	else {
		cts_pin_false++;
		//Maximaler Fehleranzahl erreicht?
		if(cts_pin_false == maxfailures)
		{
			EV_RS232_streckenbefehl.Fehler = 0x04;	
		}
		
	}
}

//Konvertiert Streckenbefehl aus dem Shared-Memory ins XpressNet-Format
void konvertLok()
{
	byte v;

	//Lok soll gestoppt werden 0000 00XX
	if((EV_RS232_streckenbefehl.Lok >> 2) == 0x00)
	{
		sende_counter = 0x04;
		sende_buffer[3] = 0x92; //Header
		sende_buffer[2] = 0x00;//Daten 1
		
		if((0x01 & EV_RS232_streckenbefehl.Lok) == 0x01)
		{
			sende_buffer[1] = Lok1_address;//Daten 2
		}
		else
		{
			sende_buffer[1] = Lok2_address;//Daten 2
		}
		
		sende_buffer[0] = (sende_buffer[3] ^ sende_buffer[2]) ^ sende_buffer[1];
		return;
	}

	sende_counter = 0x06;
	sende_buffer[5] = 0xE4;
	sende_buffer[4] = 0x13;
	sende_buffer[3] = 0x00;

	//Lok1 mit Adresse 0x00
	if((0x01 & EV_RS232_streckenbefehl.Lok) == 0x00) 
	{
		sende_buffer[2] = Lok1_address;		  	
		
	}  //Lok2 mit Adresse 0x01
	else if((0x01 & EV_RS232_streckenbefehl.Lok) == 0x01) 
	{
		sende_buffer[2] = Lok2_address;	
	}
	
	//Geschwindigkeit ermitteln
	v = 0x03 & (EV_RS232_streckenbefehl.Lok  >> 2);
	
	if(v == 0x01) {
		sende_buffer[1] = ((0x02 & EV_RS232_streckenbefehl.Lok) << 6 ) | V_Abkuppeln;
	}
	else {
		if (v == 0x02) {
			sende_buffer[1] = ((0x02 & EV_RS232_streckenbefehl.Lok) << 6 ) | V_Ankuppeln;
		}
		else {
			sende_buffer[1] = ((0x02 & EV_RS232_streckenbefehl.Lok) << 6 ) | V_Fahrt;
		}
	}
	
	sende_buffer[0] = ((((sende_buffer[5] ^ sende_buffer[4]) ^ sende_buffer[3]) ^ sende_buffer[2]) ^ sende_buffer[1]);
}

void konvertWeiche() //getestet
{
	byte BB;
	
	sende_counter = 0x04;
	sende_buffer[3] = 0x52; //Header
		
	//Weiche bestimmen
	
	if(EV_RS232_streckenbefehl.Weiche >> 1 == 0x01)
	{
		sende_buffer[2] = W1_address >> 2;	
		BB = W1_address % 4;	
	}
	else if(EV_RS232_streckenbefehl.Weiche >> 1 == 0x02)
	{
		sende_buffer[2] = W2_address >> 2;
		BB = W2_address % 4;
	}
	else if(EV_RS232_streckenbefehl.Weiche >> 1 == 0x03)
	{
		sende_buffer[2] = W3_address >> 2;
		BB = W3_address % 4;
	}			   
	BB = BB << 1;

	//Abbiegen
	if((0x01 & EV_RS232_streckenbefehl.Weiche) == 0x01)
	{
		sende_buffer[1] = 0x80 + (0x09 | BB);//Daten 2
	}
	//Gerade aus
	else
	{
		sende_buffer[1] = 0x80 + (0x08 | BB);//Daten 2
	}
	
	sende_buffer[0] = (sende_buffer[3] ^ sende_buffer[2]) ^ sende_buffer[1];	  
}

void konvertEntkoppler()
{	 
	byte BB;
	
	sende_counter = 0x04;
	sende_buffer[3] = 0x52; //Header

	//Entkuppler bestimmen
	
	if(EV_RS232_streckenbefehl.Weiche >> 1 == 0x01)
	{
		sende_buffer[2] = EK1_address >> 2;	
		BB = EK1_address % 4;	
	}
	else if(EV_RS232_streckenbefehl.Weiche >> 1 == 0x02)
	{
		sende_buffer[2] = EK2_address >> 2;
		BB = EK2_address % 4;
	}			   
	BB = BB << 1;

	//entkupplerActive = 0x01 -> Erst deaktivieren
	if(entkupplerActive == 0x01)
	{
		sende_buffer[1] = 0x80 + BB;//Daten 2
	}
	//entkupplerActive = 0x00 -> "Heben" Befehl erzeugen
	else
	{
		sende_buffer[1] = 0x80 + (0x08 | BB);//Daten 2
	}
	
	sende_buffer[0] = (sende_buffer[3] ^ sende_buffer[2]) ^ sende_buffer[1];	  
}


void initRS232() 
{
	if(RS232TREIBER_CTSPIN == 0)
	{
		rs232_enabled = 0x01;
	}
	else
	{
		rs232_enabled = 0x00;
	}
	
	EV_RS232_streckenbefehl.Fehler = 0x00;
	SM0 = 0; 
	SM1 = 1;	
	RI = 0;
	TI = 0;	
	SM2 = 1;
	SRELH = 0x03;
	SRELH = 0xDF;
	ES = 1;
	REN = 1;
	BD = 1;
}
/*
void main(void)
{
   int i = 50000;

   initRS232();

   EV_RS232_streckenbefehl.Lok = 0x02; 
	
   workRS232();
   
   while(i > 0)
   	i--;
   i = 50000;

   workRS232();
   workRS232();
   workRS232();

   while(1);
}	*/



