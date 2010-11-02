// include the SoftwareSerial library so you can use its functions:
#include <Wire.h>

#define ON 1
#define OFF 0
#define RX_PIN 2
#define TX_PIN 3
#define LED_PIN_I2C 13
#define LED_PIN_RS232 13
#define MAX_MELDUNGEN 30

// Puffer der Nachrichten
byte message_i2c[MAX_MELDUNGEN][7];
byte message_rs232[MAX_MELDUNGEN][7];

// Anzahl der aktuell gepufferten Nachrichten
int puffered_msg_i2c;		//von i2c   nach rs232
int puffered_msg_rs232;		//von rs232 nach i2c

//Setup-Funktion, wird zu Programmstart aufgerufen
void setup()
{
   // Initialisiere Variablen
   puffered_msg_i2c=0;
   puffered_msg_rs232=0;
 
   // Definiere den jeweiligen Pin-Modus
   pinMode(RX_PIN, INPUT);
   pinMode(TX_PIN, OUTPUT);
	pinMode(LED_PIN_RS232, OUTPUT);
	// Definiere I2C Pin nur, wenn sich dieser vom rs232 unterscheidet
	if (LED_PIN_RS232!=LED_PIN_I2C)	
		pinMode(LED_PIN_I2C, OUTPUT);
   
   // Setze die Datenrate des SoftwareSerial-Port
   Serial.begin(9600);
   // Verbinde zum I^2C-Bus mit Adresse #4
   Wire.begin(4);
	// Fange Events ab
   Wire.onReceive(receiveEvent_i2c);
	// Serial Event?!?!
}

// Loop-Funktion, wird nach Setup in einer Endlosschleife aufgerufen
void loop()
{
	if(Serial.available()>0)
	{
		receive_rs232(Serial.available());
	}

	//Überprüfe beide Puffer
	check_puffer();

}

void check_puffer()
{
	check_i2c_puffer();
	check_rs232_puffer();
}

void check_i2c_puffer()
{
	//Puffer von i2c nach RS232
	if(puffered_msg_i2c > 0) 
	{
   	convert_messages(puffered_msg_i2c);
   	puffered_msg_i2c = 0;
  	}
}

void check_rs232_puffer()
{
	//Puffer von RS232 nach i2c
	if(puffered_msg_rs232 > 0) 
	{
   	for(int i=0; i<puffered_msg_rs232;++i)
			for(int n=0; n<7;++n)
			{
				Wire.send(message_rs232[i][n]);
			}
   
		puffered_msg_rs232 = 0;
  	}
}

//Setzt die pinNum auf state
void set_led(int pinNum, int state)
{
  digitalWrite(pinNum, state);
}

// Receive mit Anzahl der Bits als Parameter
void receive_rs232(int number_bytes)
{
	int number_messages = number_bytes/7;
  	for(int i=0;i<number_messages;++i)
	{
  		// Überprüfe aktuelle Puffergröße
  		if(puffered_msg_rs232<MAX_MELDUNGEN)
  		{
      	// Fülle alle 7 Bytes der Nachricht
      	for(int i=0;i<7;++i)
      	{
      	  // Empfange auf Daten (7x1Byte!)
      	  message_rs232[puffered_msg_rs232][i] = Serial.read();
      	}
      	++puffered_msg_rs232;
  		}	
  		else
  		{
  		   // Puffer ist voll, setze LED_PIN auf ON
  		   set_led(LED_PIN_RS232,ON);
			// breche Schleife ab
			i = number_messages;
  		}
	}
}

// ReceiveEvent mit Anzahl der Bits als Parameter
void receiveEvent_i2c(int number_bytes)
{
	int number_messages = number_bytes/7;
  	for(int i=0;i<number_messages;++i)
	{
		if(puffered_msg_i2c<MAX_MELDUNGEN)
		{
   		for(int j=0;j<7;++j) 
			{
      		message_i2c[i][j] = Wire.receive();
			}
			++puffered_msg_i2c;
		}
  		else
  		{
  	 		// Puffer ist voll, setze LED_PIN auf ON
  	   	set_led(LED_PIN_I2C,ON);
			// breche Schleife ab
			i = number_messages;
  		}
	}
}

void convert_messages(int count_msg) 
{
   for(int i =0;i<count_msg;++i) 
	{
		//Untersuche das erste Byte für die Herkunft
   	switch(message_i2c[i][0]) 
		{
	    	case 0:	convert_message_lz(i);	break;	//Nachricht Leitzentrale
         case 1:  convert_message_bv(i);	break;	//Nachricht Befehlsvalidierung
		   case 2:	convert_message_ev(i);  break;	//Nachricht Ergebnisvalidierung
			case 16: Serial.println("Pufferueberlauf Leitzentrale.");			break;	//Pufferüberlauf Leitzentrale
			case 17: Serial.println("Pufferueberlauf Befehlsvalidierung.");	break;	//Pufferüberlauf Befehlsvalidierung
		   case 18: Serial.println("Pufferueberlauf Ergebnisvalidierung.");  break;	//Pufferüberlauf Ergebnisvalidierung
		   default:	Serial.print("Unbekannte Definition des Byte 0: (");
                  Serial.print(message_i2c[i][0]);
                  Serial.println(").");												break; 	//Fehlerausgabe
    	}
  	}
}

//Ausgabe der Auditing Nachrichten der Leitzentrale
void convert_message_lz(int message_number) 
{ 
	/*
	 *    Byte 1:
	 *		Obere  4 Bits: Loknummer die gerade gesteuert wird
    *		Untere 4 Bits: Fehlercode
	 *
	 *   	Fehlercodes:
	 *   		0 Sprung in einen nicht existenten Zustand
	 *   		1 Ausführung eines nicht definierten Fahrbefehls
	 *   		2 Weichenstellung lässt sich nicht bestimmen
	 *   		3 Die angestrebte Zielposition wurde erreicht
	 *   		4 Ein Kuppelversuch schlug fehl
	 *   		5 Ein Ankuppelversuch wurde gestartet
	 *   		6 Ein Abkuppelversuch wurde gestartet
	 *   		7 Ein Gleisabschnitt ist nicht befahrbar
    */

	//Variablen Initialisierung
  	int gleisabschnitt_not_accessible = 0;
  	byte fehlercode = message_i2c[message_number][1] & B00001111;
  	byte loknummer  = (message_i2c[message_number][1] & B11110000)>>4;
  	
	//Ausgabe der Fehlermeldung
	Serial.println("Leitzentrale ERROR: ");
   Serial.print("Loknummer: ");
	Serial.println(loknummer,DEC);
  	switch(fehlercode)
	{
		case 0:	Serial.println("Nicht existenter Zustand. ");		break;
  		case 1:  Serial.println("Undefinierter Fahrbefehl. ");		break;
  		case 2:  Serial.println("Weichenstellung unbekannt. "); 		break;
		case 3:  Serial.println("Zielposition erreicht. ");     		break;
		case 4:  Serial.println("Kuppelversuch fehlgeschlagen. ");	break;
		case 5:  Serial.println("Ankuppelversuch gestartet. ");    	break;
		case 6:  Serial.println("Abkuppelversuch gestartet. ");    	break;
		case 7:  Serial.println("Gleisabschnitt nicht befahrbar. ");
					gleisabschnitt_not_accessible = 1;						break;
  		default: Serial.print("Unbekannter Fehlercode:");	
					Serial.println(fehlercode,DEC);							break;
  }

	/*
	 *		Byte 2: Zustand der Lok1
	 *		
	 *		Zustände:
	 *			0 fahrend
	 *			1 wartend
	 *			2 angehalten
	 *			3 ankuppelnd
	 *			4 abkuppelnd
	 *			5 holt Fahranweisung
	 */
	Serial.print("\nZustand Lok#1:");
	switch(message_i2c[message_number][2]) 
	{
		case 0:	Serial.println("Fahrend");					break;
		case 1:  Serial.println("Wartend");   				break;
		case 2:  Serial.println("Angehalten");				break;
		case 3:  Serial.println("Ankuppelnd");				break;
		case 4:  Serial.println("Abkuppelnd");				break;
		case 5:  Serial.println("Holt Fahranweisung");	break;
		default: Serial.println("Unbekannter Zustand");	break;
  	}

	/*
	 *		Byte 3: Zustand der Lok2
	 *		
	 *		Zustände:
	 *			0 fahrend
	 *			1 wartend
	 *			2 angehalten
	 *			3 ankuppelnd
	 *			4 abkuppelnd
	 *			5 holt Fahranweisung
	 */
	Serial.print("\nZustand Lok#2:");
	switch(message_i2c[message_number][3]) 
	{
		case 0:	Serial.println("Fahrend");					break;
		case 1:  Serial.println("Wartend");   				break;
		case 2:  Serial.println("Angehalten");				break;
		case 3:  Serial.println("Ankuppelnd");				break;
		case 4:  Serial.println("Abkuppelnd");				break;
		case 5:  Serial.println("Holt Fahranweisung");	break;
		default: Serial.println("Unbekannter Zustand");	break;
  	}

	/*
	 *		Byte 4: Position von Lok#1
	 */
	Serial.print("\nPosition Lok#1: ");
	Serial.println(message_i2c[message_number][4],BIN);

	/*
	 *		Byte 5: Position von Lok#2
	 */
  	Serial.print("\nPosition Lok#2: ");
  	Serial.println(message_i2c[message_number][5],BIN);

	/*
	 *		Byte 6: Fahrbefehl/Gleisabschnitt der aktiven Lok
	 *		
	 *		Je nach Fehlercode (Byte 1) wird der Fahrbefehl der aktiven Lok
	 *    oder der Gleisabschnitt der aktiven Lok ausgegeben.
	 */
  	if(gleisabschnitt_not_accessible) 
 	{
    	Serial.print("\nFahrbefehl: ");
    	Serial.println(message_i2c[message_number][6],BIN);
  	}
  	else 
	{
    	Serial.print("\nGleisabschnitt: ");
    	Serial.println(message_i2c[message_number][6],BIN);
  	}
}

//Ausgabe der Auditing Nachrichten der Befehlsvalidierung
void convert_message_bv(int message_number) 
{
  	/*
	 *		Byte 1: Zustand der Befehlsvalidierung
	 *		
	 *		Zustandscodes:
	 *			0 Programm befindet sich in der Hauptroutine workBV()
	 *		   1 Programm befindet sich in der Funktion checkSensorDaten()
	 *		   2 Programm befindet sich in der Funktion sendSensorDaten()
	 *		   3 Programm befindet sich in der Funktion checkStreckenBefehl()
	 *		   4 Programm befindet sich in der Funktion sensorNachbarn()
	 *		   5 Programm befindet sich in der Funktion checkKritischerZustand()
	 */

	Serial.println("Zustand der Befehlsvalidierung:");
	switch(message_i2c[message_number][1])
	{
		case 0:	Serial.println("workBV");							break;
		case 1:  Serial.println("checkSensorDaten");    		break;
		case 2:  Serial.println("sendSensorDaten");	    		break;
		case 3:  Serial.println("checkStreckenBefehl");    	break;
		case 4:  Serial.println("sensorNachbarn");    			break;
		case 5:  Serial.println("checkKritischerZustand");  	break;
		default:	Serial.println("Undefinierter Zustand");		break;
  }

	/*
	 *		Byte 2: Fehler der Befehlsvalidierung
	 *
	 *		Fehlercodes:
	 *			0 Kein Fehler
	 *			--------------------------
	 *			Allgemeine Fehler
	 *			--------------------------
	 *			1 Sensordaten sind fehlerhaft
	 *			2 Kritischer Zustand wurde zu oft festgestellt
	 *			3 Kopie(n) der Streckentopologie manipuliert
    *			4 Falschen internen Zustand erkannt
	 *			--------------------------
	 *			Sensordaten-Fehler
	 *			--------------------------
	 *			8 Fehlerbyte in den Sensordaten gesetzt
	 *			9 Kein Zug neben dem aktivierten Sensor
	 *			10 Alte Sensordaten noch nicht von LZ verarbeitet
	 *			11 Sensor hat weder Nachfolger noch Vorgänger
	 *			--------------------------
	 *			Streckenbefehl-Fehler
	 *			--------------------------
	 *			16 Syntaxfehler: Entkoppler-Nr. ungültig
	 *			17 Syntaxfehler: Weichen-Nr. ungültig
	 *			18 Entkoppeln, während ein schneller Zug auf diesem Gleisabschnitt ist
	 *			19 Weiche soll gestellt werden, die belegt ist
	 *			20 Weiche soll gestellt werden, die von einem anderen Zug angefahren wird
	 *			21 Lokbefehl: Mit Vollgas auf belegtes Gleis fahren
	 *			22 Lokbefehl: Weiche zum Ziel ist belegt
	 *			23 Lokbefehl: Weiche zum Ziel ist falsch gestellt
	 *			--------------------------
	 *			Kritischer-Zustand-Fehler
	 *			--------------------------
	 *			32 Ein Zug fährt mit Vollgas in Richtung eines belegten Abschnitts
	 *			33 Zwei Züge in benachbarten Abschnitten fahren aufeinander zu
	 *			34 Ein Zug fährt auf eine für ihn falsch gestellte Weiche
	 *			35 Zu viele Waggons und Loks sind auf einem Abschnitt
	 */
	Serial.print("Fehler der Befehlsvalidierung:");
	switch(message_i2c[message_number][2]) 
	{
		case 0:	Serial.println("Kein Fehler.");													break;
		case 1:  Serial.println("Sensordaten fehlerhaft.");									break;
		case 2:  Serial.println("Kritischer Zustand zu oft aufgetreten.");   			break;
		case 3:  Serial.println("Kopie der Streckentopologie manipuliert."); 			break;
		case 4:  Serial.println("Falscher Interner Zustand."); 								break;
		case 8:  Serial.println("Fehlerbyte in Sensordaten.");  								break;
		case 9:	Serial.println("Kein Zug neben dem aktivierten Sensor."); 				break;
		case 10: Serial.println("Alte Sensordaten noch nicht von LZ verarbeitet.");  	break;
		case 11: Serial.println("Sensor hat weder Nachfolger noch Vorgänger.");  		break;
		case 16: Serial.println("Syntaxfehler: Entkoppler-Nr. ungültig."); 				break;
		case 17: Serial.println("Syntaxfehler: Weichen-Nr. ungültig.");  					break;
		case 18: Serial.println("Entkoppeln, während ein schneller Zug auf diesem Gleisabschnitt ist."); 		break;
		case 19: Serial.println("Weiche soll gestellt werden, die belegt ist."); 										break;
		case 20: Serial.println("Weiche soll gestellt werden, die von einem anderen Zug angefahren wird.");	break;
		case 21: Serial.println("Lokbefehl: Mit Vollgas auf belegtes Gleis fahren."); 								break;
		case 22: Serial.println("Lokbefehl: Weiche zum Ziel ist belegt.");												break;
		case 23: Serial.println("Lokbefehl: Weiche zum Ziel ist falsch gestellt.");  									break;
		default: Serial.println("Undefinierter Fehler der Befehlsvalidierung.");										break;
  }


	/*
	 *		Byte 3: ?
	 *		
	 *		nextState (interner Zustand, vgl. Kapitel 5.1 im Dokument Moduldesign Befehlsvalidierung)
	 */
  Serial.print("Nächster Zustand: ");
  Serial.println(message_i2c[message_number][3],DEC);

	/*
	 *		Byte 4: ?
	 *		
    *		criticalStatecounter (vgl. Kapitel 6.2 im Dokument Moduldesign Befehlsvalidierung)
	 */
  Serial.print("Zustandszähler kritischer Zustände:");
  Serial.println(message_i2c[message_number][4],DEC);

	/*
	 *		Byte 5: Position der Lok#1
	 */
  Serial.print("Position Lok#1: ");
  Serial.println(message_i2c[message_number][5],BIN);

	/*
	 *		Byte 6: Position der Lok#2
	 */
  Serial.print("Position Lok#2: ");
  Serial.println(message_i2c[message_number][6],BIN);
}

//Umwandlung der Auditing Nachrichten der Ergebnisvalidierung
void convert_message_ev(int message_number) 
{ 
  	/* 
	 * 	Ergebnisvalidierung
    *	
	 *		B0	B1	B2	B3	B4	B5	B6	|(nr)Beschreibung
    *		2  1  2  X  0  0  0  |(00)Warnung: Anzahl aufeinander folgender unterschiedlicher Streckenbefehle
    *		2  1  3  4  0  0  0  |(01)Fehler: Streckenbefehle ungleich
    *		2  2  2  X  0  0  0  |(02)Warnung: Anzahl vergeblicher Versuche den Streckenbefehl an den SSC-Trb zu senden
    *		2  2  3  4  0  0  0  |(03)Fehler: Streckenbefehl konnte nicht an den SSC-Treiber gesendet werden
    *		2  3  2  X  0  0  0  |(04)Warnung: Anzahl vergeblicher Versuche den Streckenbefehl an den RS232-Trb zu senden
    *		2  3  3  4  0  0  0  |(05)Fehler: Streckenbefehl konnte nicht an den RS232-Treiber gesendet werden
    *		2  4  3  0  X  0  0  |(06)Fehler: Fehlermeldung vom SSC-Treiber kommend
    *		2  5  3  0  X  0  0  |(07)Fehler: Fehlermeldung vom RS232-Treiber kommend
    *		2  6  1  X  X  X  0  |(08)Info: Streckenbefehl der an den SSC-Treiber gesendet wurde
    *		2  7  1  X  X  X  0  |(09)Info: Streckenbefehl der an den RS232-Treiber gesendet wurde.
	 *
	 *
	 *		Ein belibiger Wert (X) wird im Quelltext durch eine -1 realisiert.
	 */
	Serial.println("Meldung der Ergebnisvalidierung:");
	const int MAX_NOTICE = 9;
	int b[7] = {0,0,0,0,0,0,0};	//Info-Byte
	for(int i=0; i<MAX_NOTICE;++i)
	{
		//Setze Info-Bytes
		switch(i)
		{
			case  0: b[0]=2; b[1]=1; b[2]=2; b[3]=-1;b[4]=0; b[5]=0; b[6]=0; break;
			case  1: b[0]=2; b[1]=1; b[2]=3; b[3]=4; b[4]=0; b[5]=0; b[6]=0; break;
			case  2: b[0]=2; b[1]=2; b[2]=2; b[3]=-1;b[4]=0; b[5]=0; b[6]=0; break;
			case  3: b[0]=2; b[1]=2; b[2]=3; b[3]=4; b[4]=0; b[5]=0; b[6]=0; break;
			case  4: b[0]=2; b[1]=3; b[2]=2; b[3]=-1;b[4]=0; b[5]=0; b[6]=0; break;
			case  5: b[0]=2; b[1]=3; b[2]=3; b[3]=4; b[4]=0; b[5]=0; b[6]=0; break;
			case  6: b[0]=2; b[1]=4; b[2]=3; b[3]=0; b[4]=-1;b[5]=0; b[6]=0; break;
			case  7: b[0]=2; b[1]=5; b[2]=3; b[3]=0; b[4]=-1;b[5]=0; b[6]=0; break;
			case  8: b[0]=2; b[1]=6; b[2]=1; b[3]=-1;b[4]=-1;b[5]=-1;b[6]=0; break;
			case  9: b[0]=2; b[1]=7; b[2]=1; b[3]=-1;b[4]=-1;b[5]=-1;b[6]=0; break;
			default: Serial.println("MAX_NOTICE Konstante falsch eingestellt, oder Fehlerkodierung fehlend."); break;
		}

		//Überprüfe die Info-Bytes
		for(int n=0;n<6;++n)
		{
			if(b[n]==-1)
				continue;	//Schleifendurchlauf abbrechen
			if(b[n]!=message_i2c[message_number][n])
				break;		//Schleifenausführung abbrechen
			if(n==6)
			{
				//Bytekombination stimmt überein
				switch(i)
				{
					case  0: 
						Serial.print("Warnung: Anzahl aufeinander folgender unterschiedlicher Streckenbefehle:"); 
						Serial.println(message_i2c[message_number][3],DEC);
						break;
					case  1: 
						Serial.println("Fehler: Streckenbefehle ungleich."); 
						break;
					case  2: 
						Serial.print("Warnung: Anzahl vergeblicher Versuche den Streckenbefehl an den SSC-Treiber zu senden:");
						Serial.println(message_i2c[message_number][3],DEC);
						break;
					case  3: 
						Serial.println("Fehler: Streckenbefehl konnte nicht an den SSC-Treiber gesendet werden;");
 						break;
					case  4: 
						Serial.print("Warnung: Anzahl vergeblicher Versuche den Streckenbefehl an den RS232-Treiber zu senden:");
						Serial.println(message_i2c[message_number][3],DEC);
						break;
					case  5: 
						Serial.println("Fehler: Streckenbefehl konnte nicht an den RS232-Treiber gesendet werden."); 
						break;
					case  6: 
						Serial.print("Fehler: Fehlermeldung vom SSC-Treiber:"); 
						Serial.println(message_i2c[message_number][4],BIN);
						break;
					case  7: 
						Serial.print("Fehler: Fehlermeldung vom RS232-Treiber:");
						Serial.println(message_i2c[message_number][4],BIN);
						break;
					case  8: 
						Serial.print("Info: Streckenbefehl der an den SSC-Treiber gesendet wurde:");
    					Serial.println(message_i2c[message_number][3],BIN);
    					Serial.println(message_i2c[message_number][4],BIN);
    					Serial.println(message_i2c[message_number][5],BIN);
 						break;
					case  9: 
						Serial.print("Info: Streckenbefehl der an den RS232-Treiber gesendet wurde:"); 
    					Serial.println(message_i2c[message_number][3],BIN);
    					Serial.println(message_i2c[message_number][4],BIN);
    					Serial.println(message_i2c[message_number][5],BIN);
						break;
					default: Serial.println("Programmierfehler!"); break;
				}
				//Breche den Schleifendurchlauf ab, da die Nachricht gefunden wurde.
				i=MAX_NOTICE;
			}
		}
	}
}
