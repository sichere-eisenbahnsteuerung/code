#ifndef S88_H
#define S88_H
#include "Betriebsmittelverwaltung.h"
/*****************************************************************************
 *
 *        Dateiname:    S88.h
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Icken, Jan-Christopher
 *
 *
 *        Modul:        s88-Treiber, 0.1
 *
 *        Beschreibung:
 *        Dieses Modul liest die Sensordaten ueber den S88-Bus ein und
 *        speichert diese im Shared Memory
 *        ________________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *
 ****************************************************************************/




/* Globale Makrodefinitionen ************************************************/

/* Globale Typdefinitionen **************************************************/

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

/* Deklaration von globalen Funktionen **************************************/
void initS88(void); 
void workS88(void); 
void get_sensor_data(void);
void write_sensor_data(void);
byte validate_sensor_data(void);  
void send_error(byte error);
void wait(byte times);


#endif /* s88_H */