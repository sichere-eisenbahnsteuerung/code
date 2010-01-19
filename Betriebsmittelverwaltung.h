#ifndef BETRIEBSMITTELVERWALTUNG_H
#define BETRIEBSMITTELVERWALTUNG_H
/*****************************************************************************
 *
 *        Dateiname:    Betriebsmittelverwaltung.h
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Altan Gottwald, Vitali Voroth, Jan-Christopher Icken
 *
 *
 *        Modul:        Betriebsmittelverwaltung, ver 0.5
 *
 *        Beschreibung:
 *        Belegung von Ressourcen
 *        unsigned char sind als typedef bye definiert
 *        Timer0 wird vom SW verwendet
 *        Benennenung der verwendeten Ports
 *        ________________________________________________________________
 *        ________________________________________________________________
 *
 ****************************************************************************/
#include <reg515c.h>
/* Globale Makrodefinitionen ************************************************/

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

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

extern Streckenbefehl LZ_BV_streckenbefehl;
extern Sensordaten BV_LZ_sensordaten;
extern byte BV_LZ_bestaetigung;
extern Sensordaten S88_BV_sensordaten;
extern Streckenbefehl BV_EV_streckenbefehl;
extern Streckenbefehl SSC_EV_streckenbefehl;
extern Streckenbefehl EV_SSC_streckenbefehl;
//extern byte EV_SSC_failure; //Ueberfluessig, weil schon integriert im Streckenbefehl
//extern byte SSC_EV_failure; //Ueberfluessig, weil schon integriert im Streckenbefehl
extern byte SW_status_array[6];
extern Streckenbefehl EV_RS232_streckenbefehl;
extern byte AS_msg_counter;

/**
   Interrupt 1 für SW
   Interrupt 4 für RS232
   Interrupt 18 für S88
**/

//EAL = 1; //enable all interrupts

//Pins
//P4.1 bis P4.4 für SSC reserviert
//P4.6 und P4.7 für CAN reserviert
/*extern bit S88_PS;
extern bit S88_RESET;
extern bit S88_CLK;
extern bit S88_Data;
extern bit NOTAUS_PIN;
extern bit RS232TREIBER_CTSPIN;
extern bit AS_PORT_I2C_SDA;
extern bit AS_PORT_I2C_SCL;*/

/**
* sbit Zuweisung muss in einer Headerdatei erfolgen
* siehe: http://www.keil.com/support/docs/1317.htm
**/
sbit S88_PS = P1^6; 
sbit S88_RESET = P1^7;     
sbit S88_CLK = P3^2;
sbit S88_Data = P3^3;
sbit NOTAUS_PIN	= P3^4;
sbit RS232TREIBER_CTSPIN = P3^5;
sbit AS_PORT_I2C_SDA = P3^6;
sbit AS_PORT_I2C_SCL = P3^7;

/* Deklaration von globalen Funktionen **************************************/

#endif /* BETRIEBSMITTELVERWALTUNG_H */