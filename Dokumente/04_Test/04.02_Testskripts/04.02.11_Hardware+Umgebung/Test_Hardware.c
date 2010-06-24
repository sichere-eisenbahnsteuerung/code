/*****************************************************************************
 *
 *        Dateiname:    Test_Hardware.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Norman Nieß
 *						Kai Dziembala
 *
 *        Modul:        Testspezifikation Hardware+Umgebungskomponenten, 1.0
 *
 *        Beschreibung:
 *        Mit einem Mikrocontroller-Programm wird die Lokomotive 1 dauerhaft auf dem Hauptgleis
 *	  	  vorwärts bewegt. Gleichzeitig werden über den Arduino Debug-Ausgaben getätigt. Somit kann
 *        sichergestellt werden, dass der Mikrocontroller, die Multimaus, der XpressNet-Adapter, der
 *        DCC-Verstärker, die entsprechenden Transformatoren, die Lokomotive 1, der Arduino und die
 *        jeweilige Verkabelung funktionstüchtig sind.
 *        Dieser Test muss sowohl mit dem Mikrocontroller eins und zwei durchgeführt werden.
 *
 ****************************************************************************/

/* Includes *****************************************************************/
#include <Test_Hardware.h>
#include <intrins.h>
#include <stdio.h>

#define TRUE 1 
#define FALSE 0 
#define LEER 255

/* Globale Typdefinitionen **************************************************/

typedef unsigned char byte;
typedef bit boolean;	

typedef struct 
{ 
	byte Lok; 
	byte Weiche; 
	byte Entkoppler; 
	byte Fehler;
} Streckenbefehl;

/* Definition globaler Konstanten *******************************************/
#define TRUE 1 
#define FALSE 0 
#define LEER 255

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/
Streckenbefehl EV_RS232_streckenbefehl = {LEER,LEER,LEER,0};

static byte sbufBuffer;
char sbufBuffer_temp;
static byte receiveBuffer[3];
static byte calledWithoutSentAnswer = 0x00;
static byte bytesToReceive = 0x00;
static byte li101HeaderReceived = 0x00;
static byte readyToSend = 0x00;
static byte ctsPinFalse = 0x00;
static byte sendeCounter = 0x00;
static byte sendeBuffer[6];
static const byte maxFailures = 0x05; 
static byte rs232Enabled;
// 1 Entkupplungsdekoder ist aktiv
// 0 Entkupplungsdekoder ist nicht aktiv 
static byte entkupplerActive = 0x01;
static byte weicheActive = 0x01;  
  
/* Prototypen fuer lokale Funktionen ****************************************/
void initTestHW();
void workRS232();
void konvertLok();
void konvertWeiche();
void konvertEntkoppler();

/* Funktionsimplementierungen ***********************************************/

void main (void) {

	while(!P1^0)
	{
	}
	initTestHW();
	Testlampe = 1;
	EV_RS232_streckenbefehl.Weiche = 0x6;
	workRS232();
	EV_RS232_streckenbefehl.Weiche = 0x9;
	workRS232();
	EV_RS232_streckenbefehl.Weiche = 0xB;
	workRS232();

	EV_RS232_streckenbefehl.Lok = 0xE;
	workRS232(); 

	
	  
	/*for(;;)	{
		getSensorData();
	}
	  */
	
}

DataReceivedInterrupt () interrupt 4 //Interruptfunktion
{

	if(RI) 
	{
		//Es wurde zuvor ein Antwort-Header-Byte vom LI101F  empfangen
	/*	if(li101HeaderReceived == 0x01) 
		{
			receiveBuffer[3 - bytesToReceive] = SBUF;
			bytesToReceive--;
			if(bytesToReceive == 0x00) 
			{
				//Sendebestaetigung
				if(receiveBuffer[1] == 0x04 && receiveBuffer[2] == 0x05)
				{
					readyToSend = 0x01;	
					li101HeaderReceived = 0x00;
				}
				//Keine Sendebestaetigung
				else 
				{
					EV_RS232_streckenbefehl.Fehler = receiveBuffer[1];		
				}
			}
		} 
		//Es wurde zuvor KEIN Antwort-Header-Byte vom L101F empfangen
		else 
		{
			sbufBuffer = SBUF;
			//Li101 Header identifiziert
			if(sbufBuffer == 0x01) 
			{
				receiveBuffer[0] = sbufBuffer;
				bytesToReceive = 0x02;
				li101HeaderReceived = 0x01;
			}
			else
			{
				//Keine Datenbytes mehr auszulesen => Neuer Header
				if(bytesToReceive == 0x00) {
					bytesToReceive = (0x1F & sbufBuffer) + 1;
				}
				//Es sind noch Datenbytes zu empfangen
				else {
					bytesToReceive--;
				}
			}
		}
		*/
		sbufBuffer_temp = SBUF;
		//SBUF = 0x00;
		if(sbufBuffer_temp == 0x01) 
		{
			SBUF = 0x00;
			readyToSend = 0x01;
		}	
		RI = 0;
	}
	else 
	{
		TI = 0;
	}  
}

void workRS232() 
{
	
	if(rs232Enabled == 0x00) 
	{
		EV_RS232_streckenbefehl.Lok = 0xFF;
		EV_RS232_streckenbefehl.Weiche = 0xFF;
		EV_RS232_streckenbefehl.Entkoppler = 0xFF;
		return;
	}
	
	//Prueft wie oft das Modul aufgerufen worden ist, ohne dass eine Sende-Bestaetigung des LI101F empfangen worden ist.
	if(readyToSend == 0x00)
	{
		calledWithoutSentAnswer++;
		if( calledWithoutSentAnswer == maxFailures)
		{
			EV_RS232_streckenbefehl.Fehler = 0x04;
		}
		return;
	}
	else
	{
		calledWithoutSentAnswer = 0x00;	
	}
	
	
	
	if(RS232TREIBER_CTSPIN == CTS_readyToSend) 
	{
		ctsPinFalse = 0x00;
		
		//Keine Daten mehr zu versenden
		if(sendeCounter == 0x00) 
		{
			//Empfangsbestaetigung des letzten Befehls vom LI101F erhalten
			if(readyToSend == 0x01) 
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
		
		while(sendeCounter > 0x00)
		{
			//CTS bereit
			if(RS232TREIBER_CTSPIN == CTS_readyToSend) 
			{
				SBUF = sendeBuffer[sendeCounter-1];
				
				while(TI == 0) {
				}
				sendeCounter--;
		
				TI = 0;
			}
			//CTS nicht bereit
			/*else 
			{
				int verzoegerung = 0;
				while(verzoegerung  < 2000)
				{
					if(RS232TREIBER_CTSPIN == CTS_stop_send) 
					{
						ctsPinFalse++;
						//Maximaler Fehleranzahl erreicht?
						if(ctsPinFalse == maxFailures)
						{
							EV_RS232_streckenbefehl.Fehler = 0x04;	
						}
						return;
					}
					verzoegerung++;
				}
			} */
		}
		readyToSend = 0x00;
		//Befehl abgeschickt, entsprechenden Eintrag im Shared-Memory auf 0xFF setzen
		if(EV_RS232_streckenbefehl.Lok != 0xFF) 
		{
			EV_RS232_streckenbefehl.Lok = 0xFF;
		}
		else
		{
			//Weichen-Befehl
			if(EV_RS232_streckenbefehl.Weiche != 0xFF && weicheActive == 0x01) 
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
						entkupplerActive = 0x01;
					}
				}
			}
		}
		
	}
	//LI101F ist nicht empfangsbereit
	else 
	{
		ctsPinFalse++;
		//Maximaler Fehleranzahl erreicht?
		if(ctsPinFalse == maxFailures)
		{
			EV_RS232_streckenbefehl.Fehler = 0x04;	
		}
		
	}
}

//Konvertiert Streckenbefehl aus dem Shared-Memory ins XpressNet-Format
void konvertLok()
{
	byte v;

	sendeCounter = 0x06;
	sendeBuffer[5] = 0xE4;
	sendeBuffer[4] = 0x13;
	sendeBuffer[3] = 0x00;

	//Lok1 mit Adresse 0x00
	if((0x01 & EV_RS232_streckenbefehl.Lok) == 0x00) 
	{
		sendeBuffer[2] = Lok1_address;		  	
		
	}  //Lok2 mit Adresse 0x01
	else if((0x01 & EV_RS232_streckenbefehl.Lok) == 0x01) 
	{
		sendeBuffer[2] = Lok2_address;	
	}
	
	//Geschwindigkeit ermitteln
	v = 0x03 & (EV_RS232_streckenbefehl.Lok  >> 2);

	if(v == 0x00) {
		sendeBuffer[1] = ((0x02 & EV_RS232_streckenbefehl.Lok) << 6 ) | V_STOPP;	
	}	

	else if(v == 0x01) {
		sendeBuffer[1] = ((0x02 & EV_RS232_streckenbefehl.Lok) << 6 ) | V_Abkuppeln;
	}
	else {
		if (v == 0x02) {
			sendeBuffer[1] = ((0x02 & EV_RS232_streckenbefehl.Lok) << 6 ) | V_Ankuppeln;
		}
		else {
			sendeBuffer[1] = ((0x02 & EV_RS232_streckenbefehl.Lok) << 6 ) | V_Fahrt;
		}
	}
	
	sendeBuffer[0] = ((((sendeBuffer[5] ^ sendeBuffer[4]) ^ sendeBuffer[3]) ^ sendeBuffer[2]) ^ sendeBuffer[1]);
}

void konvertWeiche() //getestet
{
	byte BB;
	
	sendeCounter = 0x04;
	sendeBuffer[3] = 0x52; //Header
		
	//Weiche bestimmen	
	if(EV_RS232_streckenbefehl.Weiche >> 1 == 0x01)
	{
		sendeBuffer[2] = W1_address >> 2;	
		BB = W1_address % 4;	
	}
	else if(EV_RS232_streckenbefehl.Weiche >> 1 == 0x02)
	{
		sendeBuffer[2] = W2_address >> 2;
		BB = W2_address % 4;
	}
	else if(EV_RS232_streckenbefehl.Weiche >> 1 == 0x03)
	{
		sendeBuffer[2] = W3_address >> 2;
		BB = W3_address % 4;
	}			   
	BB = BB << 1;
	
	//Check ob Weiche noch deaktiviert werden muss
	if(weicheActive == 0x01)
	{ 
		//DEAKTIVIEREN DES ANDEREN AUSGANGS
		//Abbiegen
		if((0x01 & EV_RS232_streckenbefehl.Weiche) == 0x01)
		{
			sendeBuffer[1] = 0x80 + (0x01 | BB);//Daten 2
		}
		//Gerade aus
		else
		{
			sendeBuffer[1] = 0x80 + (0x00 | BB);//Daten 2 ODER nur zur Übersicht implementiert
		}
		weicheActive = 0x00;
	}
	else
	{
		//AKTIVIEREN DES ENTSPRECHENDEN AUSGANGS
		//Abbiegen
		if((0x01 & EV_RS232_streckenbefehl.Weiche) == 0x01)
		{
			sendeBuffer[1] = 0x80 + (0x08 | BB);//Daten 2
		}
		//Gerade aus
		else
		{
			sendeBuffer[1] = 0x80 + (0x09 | BB);//Daten 2
		}
		weicheActive = 0x01;
	}
	
	sendeBuffer[0] = (sendeBuffer[3] ^ sendeBuffer[2]) ^ sendeBuffer[1];	  
}

void konvertEntkoppler()
{	 
	byte BB;
	
	sendeCounter = 0x04;
	sendeBuffer[3] = 0x52; //Header

	//Entkuppler bestimmen		  	
	if(EV_RS232_streckenbefehl.Entkoppler >> 1 == 0x01)
	{
		sendeBuffer[2] = EK1_address >> 2;	
		BB = EK1_address % 4;	
	}
	else if(EV_RS232_streckenbefehl.Entkoppler >> 1 == 0x02)
	{
		sendeBuffer[2] = EK2_address >> 2;
		BB = EK2_address % 4;
	}			   
	BB = BB << 1;

	//entkupplerActive = 0x01 -> Erst deaktivieren
	if(entkupplerActive == 0x01)
	{
		sendeBuffer[1] = 0x80 + BB;//Daten 2
	}
	//entkupplerActive = 0x00 -> "Heben" Befehl erzeugen
	else
	{
		sendeBuffer[1] = 0x80 + (0x08 | BB);//Daten 2
	}
	
	sendeBuffer[0] = (sendeBuffer[3] ^ sendeBuffer[2]) ^ sendeBuffer[1];	  
}


void initTestHW() 
{
	RS232TREIBER_CTSPIN = 1;	

	rs232Enabled = 0x01;

	//SCON = SCON | 0x80;
	
	EV_RS232_streckenbefehl.Fehler = 0x00;

	BD = 1; // Baudratengenerator einschalten
	SM0 = 0; // Mode 1  8Bit variable Baudrate
	SM1 = 1;
	SRELH = 0x03; // 9600 Baud
	SRELL = 0xDF;
	REN = 1; // seriellen Empfang einschalten
	TI = 1;
	//SM0 = 0; 
	//SM1 = 1;	
	//RI = 1;
	//TI = 1;	
	//SM2 = 0;
	//SRELH = 0x03;
	//SRELH = 0xDF;

	//REN = 1;
	//BD = 1;
	//TI = 0;
	//RI=0;

	ES = 1;
	//EAL = 1;

}

void sendMsg(
	byte module_id, /*
	*
	*  Description: Vergebene Nummer zu einem der folgenden Module:
	*               0 = Leitzentrale
	*               1 = Befehlsvalidierung
	*               2 = Ergebnisvalidierung
	*  Direction  : in
	*  Values     : 0-2		(Modulnummer)
	*/

	const byte* msg /*
	*
	*  Description: Array der Groesse 6, mit Statusmeldung eines Moduls.
	*  Direction  : in
	*  Values     : [0]: 0-255,
	*               [1]: 0-255,
	*               [2]: 0-255,
	*               [3]: 0-255,
	*               [4]: 0-255,
	*               [5]: 0-255 (Statusmeldung eines Moduls)
	*/  
);


