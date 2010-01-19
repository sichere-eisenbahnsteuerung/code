#ifndef BETRIEBSMITTELVERWALTUNG_H
#define BETRIEBSMITTELVERWALTUNG_H
/*****************************************************************************
 *
 *        Dateiname:    Betriebsmittelverwaltung.h
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Altan Gottwald
 *
 *
 *        Modul:        Betriebsmittelverwaltung, ver 0.5
 *
 *        Beschreibung:
 *        Belegung von Ressourcen
 *        unsigned char sind als typedef bye definiert
 *        Timer0 wird vom SW verwendet
 *        ________________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *
 ****************************************************************************/
/* Globale Makrodefinitionen ************************************************/
/**
    #define <NAME> <ersetzung>
    dies ersetzt im gesamten quelltext begriffe
**/

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

typedef struct 
{ 
	byte Byte0; 
	byte Byte1; 
	byte Fehler; 
} Sensordaten;

/* Deklaration von globalen Variablen ***************************************/
Streckenbefehl LZ_BV_streckenbefehl = {LEER,LEER,LEER,0};
Sensordaten BV_LZ_sensordaten = {LEER,LEER,0}; 
byte BV_LZ_bestaetigung = LEER; 
Sensordaten S88_BV_sensordaten = {LEER,LEER,0};
Streckenbefehl BV_EV_streckenbefehl = {LEER,LEER,LEER,0};
Streckenbefehl SCC_EV_streckenbefehl = {LEER,LEER,LEER,0};
Streckenbefehl EV_SSC_streckenbefehl = {LEER,LEER,LEER,0};
byte EV_SSC_failure = LEER;
byte SSC_EV_failure = LEER; 
byte SW_status_array[6] = {LEER,LEER,LEER,LEER,LEER,LEER}; 
Streckenbefehl EV_RS232_streckenbefehl = {LEER,LEER,LEER,0};
//neu
byte AS_msg_counter = LEER;

/**
   Interrupt 1 für SW
   Interrupt 4 für RS232
   Interrupt 18 für S88
**/

//EAL = 1; //enable all interrupts

//Pins
//P4.1 bis P4.4 für SSC reserviert
//P4.6 und P4.7 für CAN reserviert
sbit S88_PS = P1^6; 
sbit S88_RESET = P1^7;     
sbit S88_CLK = P3^2;
sbit S88_Data = P3^3;
sbit NOTAUS_PIN	= P3^4;
sbit RS232TREIBER_CTSPIN = P3^5;
sbit AS_PORT_I2C_SDA = P3^6;
sbit AS_PORT_I2C_SCL = P3^7;

#endif /* <BETRIEBSMITTELVERWALTUNG>_H */