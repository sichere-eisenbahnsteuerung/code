/*****************************************************************************
 *
 *        Dateiname:    secure_train_control.pde
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Icken, Jan-Christopher
 *
 *
 *        Modul:        Auditing-System
 *
 *        Beschreibung:
 *        Teilmodul zur Umwandlung von an den Arduino geschickter
 *        Auditing-Nachrichten in Klartext.
 *
 ****************************************************************************/

#include <Wire.h>

#ifndef MAX_MELDUNGEN
#define MAX_MELDUNGEN	30
#endif

byte message_array[MAX_MELDUNGEN][7];
int number_of_messages = 0;

void setup()
{
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(115200);           // start serial for output
}

void loop()
{
  if(number_of_messages > 0) {
    convert_messages();
    number_of_messages = 0;
  }
}


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  //Zaehlen wieviele Auditing Meldungen erhalten wurden
  Serial.println("");
  Serial.print(howMany);
  Serial.print(" Auditing Meldungen erhalten.");
  Serial.println("");
  number_of_messages = howMany/7;
  //byte message_array[7];
  for(int i=0;i<number_of_messages;i++) {
    for(int j=0;j<7;j++) {
      message_array[i][j] = Wire.receive();
      //Serial.println(message_array[i],BIN);
    }
    //Nachrichten umwandeln
    //convert_message(message_array);
  }
}

void convert_messages() {
  for(int i =0;i<number_of_messages;i++) {
    switch(message_array[i][0]) {
    case 0: //Nachricht von der Leitzentrale
      convert_message_lz(i);
      break;
    case 1: //Nachricht von Befehlsvalidierung
      convert_message_bv(i);
      break;
    case 2: //Nachricht von der Ergebnisvalisierdung
      convert_message_ev(i);
      break;
      /*16 = Leitzentrale, Pufferueberlauf
       17 = Befehlsvalidierung, Pufferueberlauf
       18 = Ergebnisvalidierung, Pufferueberlauf*/
    case 16: //Leitzentrale Pufferueberlauf
      Serial.println("Leitzentrale Pufferueberlauf.");
      break;
    case 17: //Befehlsvalidierung Pufferueberlauf
      Serial.println("Befehlsvalidierung Pufferueberlauf.");
      break;
    case 18: //Ergebnisvalidierung Pufferueberlauf
      Serial.println("Ergebnisvalidierung Pufferueberlauf.");
      break;
    default:
      break; 
    }

  }

}

void convert_message_lz(int message_number) { //Funktion zu Umwandeln der Auditing Nachrichten der Leitzentrale
  /*[Byte 1]
   Untere 4 Byte Fehlercode
   Fehlercodes:
   0 Sprung in einen nicht existenten Zustand
   1 Ausführung eines nicht definierten Fahrbefehls
   2 Weichenstellung lässt sich nicht bestimmen
   3 Die angestrebte Zielposition wurde erreicht
   4 Ein Kuppelversuch schlug fehl
   5 Ein Ankuppelversuch wurde gestartet
   6 Ein Abkuppelversuch wurde gestartet
   7 Ein Gleisabschnitt ist nicht befahrbar
   Obere 4 Byte Loknummer die gerade gesteuert wird*/
  int gleisabschnitt_not_accessible = 0;
  byte fehlercode = 0;
  byte loknummer = 0;
  Serial.println("LZ FCODE: ");
  fehlercode = message_array[message_number][1] & B00001111;
  loknummer = (message_array[message_number][1] & B11110000)>>4;
  switch(fehlercode) {
  case 0:
    Serial.print("Nicht existenter Zustand. ");
    break;
  case 1:
    Serial.print("Undefinierter Fahrbefehl. ");
    break;
  case 2:
    Serial.print("Weichenstellung unbekannt. ");
    break;
  case 3:
    Serial.print("Zielposition erreicht. ");
    break;
  case 4:
    Serial.print("Kuppelversuch fehlgeschlagen. ");
    break;
  case 5:
    Serial.print("Ankuppelversuch gestartet. ");
    break;
  case 6:
    Serial.print("Abkuppelversuch gestartet. ");
    break;
  case 7:
    Serial.print("Gleisabschnitt nicht befahrbar. ");
    gleisabschnitt_not_accessible = 1;
    break;
  default:
    Serial.print("Unbekannter Fehlercode. ");
    break;
  }
  Serial.print("Loknummer: ");
  Serial.print(loknummer,DEC);
  /*[Byte 2]
   FAHREND = 0
   WARTEND = 1
   ANGEHALTEN = 2
   ANKUPPELND = 3
   ABKUPPELND  = 4
   HOLT_FAHRANWEISUNG = 5
   Zustand von Lok #1*/
  Serial.print(" Zustand Lok#1: ");
  switch(message_array[message_number][2]) {
  case 0:
    Serial.print("Fahrend");
    break;
  case 1:
    Serial.print("Wartend");
    break;
  case 2:
    Serial.print("Angehalten");
    break;
  case 3:
    Serial.print("Ankuppelnd");
    break;
  case 4:
    Serial.print("Abkuppelnd");
    break;
  case 5:
    Serial.print("Holt Fahranweisung");
    break;
  default:
    Serial.print("Unbekannter Zustand");
    break;
  }
  /*[Byte 3]
   FAHREND = 0
   WARTEND = 1
   ANGEHALTEN = 2
   ANKUPPELND = 3
   ABKUPPELND  = 4
   HOLT_FAHRANWEISUNG = 5
   Zustand von Lok #2*/
  Serial.print(" Zustand Lok#2: ");
  switch(message_array[message_number][3]) {
  case 0:
    Serial.print("Fahrend");
    break;
  case 1:
    Serial.print("Wartend");
    break;
  case 2:
    Serial.print("Angehalten");
    break;
  case 3:
    Serial.print("Ankuppelnd");
    break;
  case 4:
    Serial.print("Abkuppelnd");
    break;
  case 5:
    Serial.print("Holt Fahranweisung");
    break;
  default:
    Serial.print("Unbekannter Zustand");
    break;
  }
  /*[Byte 4]
   Die Position von Lok #1*/
  Serial.print(" Position Lok#1: ");
  Serial.print(message_array[message_number][4],BIN);
  /*[Byte 5]
   Die Position von Lok #2*/
  Serial.print(" Position Lok#2: ");
  Serial.print(message_array[message_number][5],BIN);
  /*[Byte 6]
   Wenn Fehlercode 0-6: Fahrbefehl
   Fehlercode 7:        Gleisabschnitt */
  if(gleisabschnitt_not_accessible) {
    Serial.print(" Fahrbefehl: ");
    Serial.print(message_array[message_number][6],BIN);
  }
  else {
    Serial.print(" Gleisabschnitt: ");
    Serial.print(message_array[message_number][6],BIN);
  }
  Serial.println("");

}

void convert_message_bv(int message_number) { //Funktion zu Umwandeln der Auditing Nachrichten der Befehlsvalidierung
  /*Befehlsvalidierung
   [Byte 0]    
   1 = Befehlsvalidierung
   [Byte 1]
   Zustandscodes:
   0 Programm befindet sich in der Hauptroutine workBV()
   1 Programm befindet sich in der Funktion checkSensorDaten()
   2 Programm befindet sich in der Funktion sendSensorDaten()
   3 Programm befindet sich in der Funktion checkStreckenBefehl()
   4 Programm befindet sich in der Funktion sensorNachbarn()
   5 Programm befindet sich in der Funktion checkKritischerZustand()*/
  Serial.println("BV Zustand: ");
  switch(message_array[message_number][1]) {
  case 0:
    Serial.print("workBV ");
    break;
  case 1:
    Serial.print("checkSensorDaten ");
    break;
  case 2:
    Serial.print("sendSensorDaten ");
    break;
  case 3:
    Serial.print("checkStreckenBefehl ");
    break;
  case 4:
    Serial.print("sensorNachbarn ");
    break;
  case 5:
    Serial.print("checkKritischerZustand ");
    break;
  default:
    break;
  }
  /*[Byte 2]
   Fehlercodes:
   0 Kein Fehler
   --------------------------
   Allgemeine Fehler
   --------------------------
   1 Sensordaten sind fehlerhaft
   2 Kritischer Zustand wurde zu oft festgestellt
   3 Kopie(n) der Streckentopologie manipuliert
   4 Falschen internen Zustand erkannt
   --------------------------
   Sensordaten-Fehler
   --------------------------
   8 Fehlerbyte in den Sensordaten gesetzt
   9 Kein Zug neben dem aktivierten Sensor
   10 Alte Sensordaten noch nicht von LZ verarbeitet
   11 Sensor hat weder Nachfolger noch Vorgänger
   --------------------------
   Streckenbefehl-Fehler
   --------------------------
   16 Syntaxfehler: Entkoppler-Nr. ungültig
   17 Syntaxfehler: Weichen-Nr. ungültig
   18 Entkoppeln, während ein schneller Zug auf diesem Gleisabschnitt ist
   19 Weiche soll gestellt werden, die belegt ist
   20 Weiche soll gestellt werden, die von einem anderen Zug angefahren wird
   21 Lokbefehl: Mit Vollgas auf belegtes Gleis fahren
   22 Lokbefehl: Weiche zum Ziel ist belegt
   23 Lokbefehl: Weiche zum Ziel ist falsch gestellt
   --------------------------
   Kritischer-Zustand-Fehler
   --------------------------
   32 Ein Zug fährt mit Vollgas in Richtung eines belegten Abschnitts
   33 Zwei Züge in benachbarten Abschnitten fahren aufeinander zu
   34 Ein Zug fährt auf eine für ihn falsch gestellte Weiche
   35 Zu viele Waggons und Loks sind auf einem Abschnitt*/
  Serial.println("BV Fehler: ");
  switch(message_array[message_number][2]) {
  case 0:
    Serial.print("Kein Fehler ");
    break;
  case 1:
    Serial.print("Sensordaten fehlerhaft ");
    break;
  case 2:
    Serial.print("Kritischer Zustand zu oft aufgetreten ");
    break;
  case 3:
    Serial.print("Kopie der Streckentopologie manipuliert ");
    break;
  case 4:
    Serial.print("Falscher Interner Zustand ");
    break;
  case 8:
    Serial.print("Fehlerbyte in Sensordaten ");
    break;
  case 9:
    Serial.print("Kein Zug neben dem aktivierten Sensor ");
    break;
  case 10:
    Serial.print("Alte Sensordaten noch nicht von LZ verarbeitet ");
    break;
  case 11:
    Serial.print("Sensor hat weder Nachfolger noch Vorgänger ");
    break;
  case 16:
    Serial.print("Syntaxfehler: Entkoppler-Nr. ungültig ");
    break;
  case 17:
    Serial.print("Syntaxfehler: Weichen-Nr. ungültig ");
    break;
  case 18:
    Serial.print("Entkoppeln, während ein schneller Zug auf diesem Gleisabschnitt ist ");
    break;
  case 19:
    Serial.print("Weiche soll gestellt werden, die belegt ist ");
    break;
  case 20:
    Serial.print("Weiche soll gestellt werden, die von einem anderen Zug angefahren wird ");
    break;
  case 21:
    Serial.print("Lokbefehl: Mit Vollgas auf belegtes Gleis fahren ");
    break;
  case 22:
    Serial.print("Lokbefehl: Weiche zum Ziel ist belegt ");
    break;
  case 23:
    Serial.print("Lokbefehl: Weiche zum Ziel ist falsch gestellt ");
    break;
  default:
    break;
  }
  /*[Byte 3]   
   nextState (interner Zustand, vgl. Kapitel 5.1 im Dokument Moduldesign Befehlsvalidierung)*/
  Serial.print(" Next state: ");
  Serial.print(message_array[message_number][3],DEC);
  /*[Byte 4]
   criticalStatecounter (vgl. Kapitel 6.2 im Dokument Moduldesign Befehlsvalidierung)*/
  Serial.print(" criticalStatecounter: ");
  Serial.print(message_array[message_number][4],DEC);
  /*[Byte 5]
   Die Position von Lok #1*/
  Serial.print(" Position Lok#1: ");
  Serial.print(message_array[message_number][5],BIN);
  /*[Byte 6]
   Die Position von Lok #2*/
  Serial.print(" Position Lok#2: ");
  Serial.print(message_array[message_number][6],BIN);
  Serial.println("");

}

void convert_message_ev(int message_number) { //Funktion zu Umwandeln der Auditing Nachrichten der Ergebnisvalidierung
  /* Ergebnisvalidierung
   [Byte 0]  [Byte 1]  [Byte 2]  [Byte 3]  [Byte 4]  [Byte 5]  [Byte 6]
   2          1        2           X        0          0          0        Warnung: Anzahl aufeinander folgender unterschiedlicher Streckenbefehle
   2          1        3           4        0          0          0        Fehler: Streckenbefehle ungleich
   2          2        2           X        0          0          0        Warnung: Anzahl vergeblicher Versuche den Streckenbefehl an den SSC-Treiber zu senden
   2          2        3           4        0          0          0        Fehler: Streckenbefehl konnte nicht an den SSC-Treiber gesendet werden
   2          3        2           X        0          0          0        Warnung: Anzahl vergeblicher Versuche den Streckenbefehl an den RS232-Treiber zu senden
   2          3        3           4        0          0          0        Fehler: Streckenbefehl konnte nicht an den RS232-Treiber gesendet werden
   2          4        3           0        X          0          0        Fehler: Fehlermeldung vom SSC-Treiber kommend
   2          5        3           0        X          0          0        Fehler: Fehlermeldung vom RS232-Treiber kommend
   2          6        1           X        X          X          0        Info: Streckenbefehl der an den SSC-Treiber gesendet wurde
   2          7        1           X        X          X          0        Info: Streckenbefehl der an den RS232-Treiber gesendet wurde.*/
  Serial.println("EV Meldung: ");
  switch(message_array[message_number][1]) {
  case 1:
    if(message_array[message_number][2] == 2) {
      Serial.print("Warnung: Anzahl aufeinander folgender unterschiedlicher Streckenbefehle ");
      Serial.print(message_array[message_number][3],DEC);
    }
    if(message_array[message_number][2] == 3) {
      Serial.print("Fehler: Streckenbefehle ungleich ");
    }
    break;
  case 2:
    if(message_array[message_number][2] == 2) {
      Serial.print("Warnung: Anzahl vergeblicher Versuche den Streckenbefehl an den RS232-Treiber zu senden ");
      Serial.print(message_array[message_number][3],DEC);
    }
    if(message_array[message_number][2] == 3) {
      Serial.print("Fehler: Streckenbefehl konnte nicht an den RS232-Treiber gesendet werden ");
    }
    break;
  case 3:
    if(message_array[message_number][2] == 2) {
      Serial.print("Warnung: Anzahl vergeblicher Versuche den Streckenbefehl an den SSC-Treiber zu senden ");
      Serial.print(message_array[message_number][3],DEC);
    }
    if(message_array[message_number][2] == 3) {
      Serial.print("Fehler: Streckenbefehl konnte nicht an den SSC-Treiber gesendet werden ");
    }
    break;
  case 4:
    Serial.print("Fehlermeldung SSC-Treiber ");
    Serial.print(message_array[message_number][4],BIN);
    break;
  case 5:
    Serial.print("Fehlermeldung RS232-Treiber ");
    Serial.print(message_array[message_number][4],BIN);
    break;
  case 6:
    Serial.print("Streckenbfehl SSC-Treiber ");
    Serial.print(message_array[message_number][3],BIN);
    Serial.print(message_array[message_number][4],BIN);
    Serial.print(message_array[message_number][5],BIN);
    break;
  case 7:
    Serial.print("Streckenbfehl RS232-Treiber ");
    Serial.print(message_array[message_number][3],BIN);
    Serial.print(message_array[message_number][4],BIN);
    Serial.print(message_array[message_number][5],BIN);
    break;
  default:
    break;
  }
  Serial.println("");
}