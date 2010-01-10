#ifndef BETRIEBSMITTELVERWALTUNG_H
#define BETRIEBSMITTELVERWALTUNG_H

#define TRUE 1 
#define FALSE 0 
#define LEER 255

typedef unsigned char byte;

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

#endif /* BETRIEBSMITTELVERWALTUNG_H */