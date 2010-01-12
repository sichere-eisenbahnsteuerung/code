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




/* Globale Makrodefinitionen ************************************************/

/* Globale Typdefinitionen **************************************************/

/* Deklaration von globalen Konstanten **************************************/
#define V_Abkuppeln 0x08
#define V_Ankuppeln 0x1F
#define V_Fahrt 0xDD
#define Lok1_address 0x00
#define Lok2_address 0x01
#define W1_address 0x03
#define W2_address 0x04
#define W3_address 0x02
#define EK1_address 0x06
#define EK2_address 0x05

/* Deklaration von globalen Variablen ***************************************/


/* Deklaration von globalen Funktionen **************************************/
void initRS232();
void workRS232();


#endif /* <RS232Treiber>_H */
