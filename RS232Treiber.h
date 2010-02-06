#ifndef RS232Treiber_H
#define RS232Treiber_H
/*****************************************************************************
 *
 *        Dateiname:    RS232Treiber.h
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Oliver Gatti
 *
 *
 *        Modul:        RS232-Treiber, 1.2
 *
 *        Beschreibung:
 *        Dieses Modul implementiert die Kommunikation zwischen dem Micro-
 *        controller und dem LI101f Interface. Die Kommunikation findet 
 *        über die RS232 Schnittstelle statt. Die Ergebnisvalidierung legt 
 *        die Streckenbefehle im Shared-Memory ab und der RS232-Treiber liest
 *        diese aus und verarbeitet sie weiter.
 *
 ****************************************************************************/


/* Globale Makrodefinitionen ************************************************/
#define V_STOPP 0x00
#define V_Abkuppeln 0x08
#define V_Ankuppeln 0x1F
#define V_Fahrt 0xDD
#define Lok1_address 0x01
#define Lok2_address 0x02
#define W1_address 0x03
#define W2_address 0x04
#define W3_address 0x02
#define EK1_address 0x06
#define EK2_address 0x05
#define CTS_ready_to_send 1
#define CTS_stop_send 0

/* Globale Typdefinitionen **************************************************/

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

/* Deklaration von globalen Funktionen **************************************/

/*
 * Initialisiert alle benötigten Register und Variablen. Muss vor workRS32 ausgeführt werden.
 */
void initRS232();

/*
 *	Hauptfunktion des Moduls. Die Funktion liest Streckenbefehle aus dem Shared-Memory und konvertiert sie in das XpressNet-Format.
 *	Anschließend werden die einzelnen Bytes über die Serielle Schnittstelle (RS232) verschickt.
 *  Es wird maximal _ein_ Streckenbefehl pro Aufruf verschickt.
 */
void workRS232();


#endif /* <RS232Treiber>_H */
