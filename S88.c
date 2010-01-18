/*****************************************************************************
 *
 *        Dateiname:    S88.c
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

/* Includes *****************************************************************/
//#include "Betriebsmittelverwaltung.h"
#include "S88.h"
//#include <Notaus.h>  
#include <intrins.h>

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/
#define HIGH 1
#define LOW 0
/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/
int i;
int sensor_count;
byte temp_sensor_data;
byte error;
/* Prototypen fuer lokale Funktionen ****************************************/

/* Funktionsimplementierungen ***********************************************/
void initS88(void) 
{
	S88_PS = LOW;
	S88_RESET = LOW;
	S88_CLK = LOW;
	S88_Data = HIGH;
	sensor_count = 0;
	S88_BV_sensordaten.Byte0 = 0x00;
	S88_BV_sensordaten.Byte1 = 0x00;
}
 
void workS88(void) {
//hello(0x03, byte status) %anmelden beim SW  (ID: 3 Status: noch zu klaeren)
//Ueberpruefen ob Daten aus dem Struct gelesen wurden
//Falls ja, s88-Bus abfragen
if(S88_BV_sensordaten.Byte0 == LEER && S88_BV_sensordaten.Byte1 == LEER)
	{
		return;
	}
	else
	{
		S88_BV_sensordaten.Byte0 = 0x00;
		S88_BV_sensordaten.Byte1 = 0x00;
		get_sensor_data(); //Sensordaten abfragen
	}
}

void get_sensor_data() {
	S88_PS = HIGH;
	wait(3);
	S88_CLK = HIGH;
	wait(9);
	S88_CLK = LOW;
	wait(9);
	S88_RESET = HIGH;
	wait(7);
	S88_RESET = LOW;
	wait(7);
	write_sensor_data();
	S88_PS = LOW;
	wait(9);
	for(i = 0;i< 15;i++) 
	{
		S88_CLK = HIGH;
		write_sensor_data();
		wait(9);
		S88_CLK = LOW;
		wait(9);
	} 
}

void write_sensor_data() 
{
	temp_sensor_data = S88_Data; //Daten vom Dateneingang in temp Variable schreiben
	if(sensor_count	< 0x08)
	{
		//In Byte0 vom struct Sensordaten schreiben
		S88_BV_sensordaten.Byte0+= temp_sensor_data;
		if(sensor_count < 0x07)
		{
			S88_BV_sensordaten.Byte0 = S88_BV_sensordaten.Byte0 << 1;
		}		
		sensor_count++;

	}
	else if(sensor_count <= 0x0F)
	{
		//In Byte1 vom struct Sensordaten schreiben
		S88_BV_sensordaten.Byte1 += temp_sensor_data;
		
		if(sensor_count == 0x0F)
		{
			sensor_count = 0;	//Sensordaten von vorne beginnen einzulesen	
		}
		else
		{
			S88_BV_sensordaten.Byte1 = S88_BV_sensordaten.Byte1 << 1;
			sensor_count++;
		}
		
	}
	else
	{
		error = 0xFF;
		send_error(error);//Fehler werfen
	}

}

byte validate_sensor_data()
{
	return 1;
}

void send_error(byte error)
{
	//Sende Error an Auditing System
}

void wait(byte times) {
	while(times > 0) 
	{
		_nop_();
		times--;
	}
}