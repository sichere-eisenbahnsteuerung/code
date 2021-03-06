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
 *        Modul:        Betriebsmittelverwaltung
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
/*
  Modul IDs f�r SW

  Leitzentrale - 0
  Befehlsvalidierung - 1
  Ergebnisvalidierung - 2
  S88-Treiber - 3
  SSC-Treiber - 4
  RS232-Treiber - 5
  Auditing System - 6
*/
enum module_id { MODUL_LZ=0, MODUL_BV=1, MODUL_EV=2, MODUL_S88=3, MODUL_SSC=4, MODUL_RS232=5, MODUL_AS=6};

/* Deklaration von globalen Variablen ***************************************/

extern Streckenbefehl LZ_BV_streckenbefehl;
extern Sensordaten BV_LZ_sensordaten;
extern byte BV_LZ_bestaetigung;
extern Sensordaten S88_BV_sensordaten;
extern Streckenbefehl BV_EV_streckenbefehl;
extern Streckenbefehl SSC_EV_streckenbefehl;
extern Streckenbefehl EV_SSC_streckenbefehl;
//extern byte SW_status_array[6];
extern Streckenbefehl EV_RS232_streckenbefehl;
extern Streckenbefehl RS232_EV_streckenbefehl;
extern byte AS_msg_counter;

/**
   Interrupt 1 f�r SW
   Interrupt 4 f�r RS232
   Interrupt 18 f�r S88
**/

//Pins
//P4.1 bis P4.4 fuer SSC reserviert
//P4.6 und P4.7 fuer CAN reserviert

/**
* sbit Zuweisung muss in einer Headerdatei erfolgen
* siehe: http://www.keil.com/support/docs/1317.htm
**/
sbit NOTAUS_PIN	= P3^4;
sbit RS232TREIBER_CTSPIN = P3^5;
sbit S88_PS = P5^0; 
sbit S88_RESET = P5^1;     
sbit S88_CLK = P5^2;
sbit S88_Data = P5^3;
sbit AS_PORT_I2C_SDA = P5^4;
sbit AS_PORT_I2C_SCL = P5^5;
sbit Neustart = P1^0;

/* Deklaration von globalen Funktionen **************************************/


/* Prototypen fuer lokale Funktionen ****************************************/
void initAll(void);
void work(void);


#endif /* BETRIEBSMITTELVERWALTUNG_H */