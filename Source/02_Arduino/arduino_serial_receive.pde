// include the SoftwareSerial library so you can use its functions:
#include <SoftwareSerial.h>

#define RX_PIN 2
#define TX_PIN 3
#define LED_PIN 13
#define ON 1
#define OFF 0

#ifndef MAX_MELDUNGEN
#define MAX_MELDUNGEN	30
#endif



// Erstelle SoftwareSerial-Port
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);
// Puffer der Nachrichten
byte message[MAX_MELDUNGEN][7];
// Anzahl der aktuell gepufferten Nachrichten
int puffered_msg;

//Setup-Funktion, wird vermutlich zu Programmstart aufgerufen
void setup()
{
  // Initialisiere Variablen
  puffered_msg=0;
  pinState=0;
  // Definiere den jeweiligen Pin-Modus
  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  // Setze die Datenrate des SoftwareSerial-Port
  mySerial.begin(9600);
}

//Loop-Funktion, wird vermutlich nach Setup in einer Endlosschleife aufgerufen
void loop()
{
  // Überprüfe aktuelle Puffergröße
  if(puffered_message<MAX_MELDUNGEN)
  {
      // Fülle alle 7 Bytes der Nachricht
      for(int i=0;i<7;++i)
      {
        //Warte auf Daten (7x1Byte!)
        message[puffered_msg][i] = mySerial.read();
      }
      ++puffered_message;
  }
  else
  {
      //Puffer ist voll, setze LED_PIN auf ON
      set_led(LED_PIN,ON);
  }
}

void set_led(int pinNum, int state)
{
  digitalWrite(pinNum, state);
}
