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
#include <intrins.h>
#include "S88Treiber.h" 


/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/
#define HIGH 1
#define LOW 0
/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/
int sensor_data_old[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int sensor_data_new[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int i;
int sensor_count;
byte temp_sensor_data;
byte error;
/* Prototypen fuer lokale Funktionen ****************************************/

void get_sensor_data(void);
void write_sensor_data(int sensor_number);
void validate_sensor_data(void);  
/*void send_error(byte error);*/
void wait(byte times);

/* Funktionsimplementierungen ***********************************************/
void initS88(void) 
{
	S88_PS = LOW;
	S88_RESET = LOW;
	S88_CLK = LOW;
	S88_Data = HIGH;
	sensor_count = 0;
	S88_BV_sensordaten.Byte0 = LEER;
	S88_BV_sensordaten.Byte1 = LEER;
}
 
void workS88(void) {
hello(0x03, byte status) /*anmelden beim SW  (ID: 3 Status: noch zu klaeren)*/
//Ueberpruefen ob Daten aus dem Struct gelesen wurden
//Falls ja, s88-Bus abfragen
if(S88_BV_sensordaten.Byte0 == LEER && S88_BV_sensordaten.Byte1 == LEER)
	{
		S88_BV_sensordaten.Byte0 = 0x00;
		S88_BV_sensordaten.Byte1 = 0x00;
		get_sensor_data(); //Sensordaten abfragen
	}
	else
	{
		return;
	}
}

void get_sensor_data() {
	S88_PS = HIGH;
	wait(3);
	S88_CLK = HIGH;
	wait(9);
	S88_CLK = LOW;			   keine
	wait(9);
	S88_RESET = HIGH;
	wait(7);
	S88_RESET = LOW;
	wait(7);
	write_sensor_data(0); //1. Sensor auslesen und in Array schreiben
	S88_PS = LOW;
	wait(9);
	for(i = 1;i< 16;i++) 
	{
		S88_CLK = HIGH;
		write_sensor_data(i);  //Sensor i auslesen und in Array schreiben
		wait(9);
		S88_CLK = LOW;
		wait(9);
	}
	validate_sensor_data(); 
}

void write_sensor_data(int sensor_number)
{
	sensor_data_new[sensor_number] = S88_Data;
	sensor_data_new[sensor_number] = sensor_data_new[sensor_number] <<7;	
}

void validate_sensor_data()
{
	//Alte Sensordaten mit neuen Sensordaten vergleichen
	//und in Shared Memory schreiben
	for(i = 15;i>= 0;i--) 
	{
		//Ueberpruefung ob sich Sensordaten seit dem letzten Auslesen geaendert haben
		if(sensor_data_old[i] == sensor_data_new[i])
		{
			sensor_data_new[i] = 0;	//Sensordaten "loeschen" falls Flanke bereits gemeldet					
		}
		//Daten in Shared Memory schreiben
		if( i < 0x08)
		{
			//In Byte0 vom struct Sensordaten schreiben
			S88_BV_sensordaten.Byte0 += sensor_data_new[i];
			//Bitshift fuer 1. Sensor = LSB und 8. Sensor = MSB
			if(i > 0x00)
			{
				S88_BV_sensordaten.Byte0 = S88_BV_sensordaten.Byte0 >> 1;
			}		
		}
		else if(i < 0x10)
		{
			//In Byte1 vom struct Sensordaten schreiben
			S88_BV_sensordaten.Byte1 += sensor_data_new[i];
		
			if(i > 0x08)
			{
				//Bitshift fuer 9. Sensor = LSB und 16. Sensor = MSB
				S88_BV_sensordaten.Byte1 = S88_BV_sensordaten.Byte1 >> 1;	
			}
		}
		sensor_data_old[i] = sensor_data_new[i];
	}	
}

/*void send_error(byte error)
{
	S88_BV_sensordaten.Fehler = error;//Setze Error Flag im struct
} */

void wait(byte times) {
	while(times > 0) 
	{
		_nop_();
		times--;
	}
}