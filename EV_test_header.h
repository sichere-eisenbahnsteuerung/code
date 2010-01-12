#ifndef EV_TEST_HEADER_H
#define EV_TEST_HEADER_H
/*****************************************************************************
 *
 *        Dateiname:    ev_test_header.h
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Philip Weber
 *
 *
 *        Modul:        Ergebnisvalidierung, <Version des Moduldesigns>
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
typedef unsigned char byte;
typedef bit boolean;
typedef struct streckenbefehl{
	byte Lok;
	byte Weiche;
	byte Entkoppler;
	byte Fehler;
} Streckenbefehl;

/* Deklaration von globalen Konstanten **************************************/
#define TRUE 1
#define FALSE 0
#define LEER 0xFF

/* Deklaration von globalen Variablen ***************************************/
Streckenbefehl BV_EV_streckenbefehl = {LEER,LEER,LEER,LEER};
Streckenbefehl EV_RS232_streckenbefehl = {LEER,LEER,LEER,LEER};
Streckenbefehl EV_SSC_streckenbefehl = {LEER,LEER,LEER,LEER};
Streckenbefehl RS232_EV_streckenbefehl = {LEER,LEER,LEER,0};
Streckenbefehl SSC_EV_streckenbefehl = {LEER,LEER,LEER,LEER};

/* Deklaration von globalen Funktionen **************************************/
void send_msg(byte msg, byte caller){
}
void emergency_off(void){
}


#endif /* EV_TEST_HEADER_H */
