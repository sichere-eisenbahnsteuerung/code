/*****************************************************************************
 *
 *        Dateiname:    Test_RS232Treiber.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Jan-Christopher Icken
 *
 *
 *        Modul:        Testskript für das RS232-Treiber-Modul
 *
 *        Beschreibung:
 *        
 *        _______________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *        ________________________________________________________________
 *
 ****************************************************************************/

/* Includes *****************************************************************/
#include "RS232Treiber.h"
#include "Test_RS232Treiber.h"
#include <intrins.h>
#include <stdio.h>

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/

/* Prototypen fuer lokale Funktionen ****************************************/
void run_rs232_test1(void);  //Fahrbefehle Lok 1
void run_rs232_test2(void);  //Fahrbefehle Lok 2
void run_rs232_test3(void);  //Weichenbefehle
void run_rs232_test4(void);  //Entkopplerbefehle
/* Funktionsimplementierungen ***********************************************/
void work_rs232_tests(void) {
	run_rs232_test1();
	run_rs232_test2();
	run_rs232_test3();
	run_rs232_test4();  	
		for(;;) {
	} 	
} 

void run_rs232_test1(void) {
	//Modul initialisieren
	initRS232();
	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Lok = 0x03; 	//Stop Vorwärts
	workRS232();
	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Lok = 0x07; 	//V_Abkuppeln Vorwärts
	workRS232();
	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Lok = 0x0B;	//V_Ankuppeln Vorwärts
	workRS232();
       	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Lok = 0x0F;	 //V_Fahrt Vorwärts
	workRS232();
	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Lok = 0x01;	 //Stop Rückwärts
	workRS232();
   	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Lok = 0x05;	 //V_Abkuppeln Rückwärts
	workRS232();
   	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Lok = 0x09;	 //V_Ankuppeln Rückwärts
	workRS232();
   	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Lok = 0x0D;	 //V_Fahrt Rückwärts
	workRS232();
   	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}	
	EV_RS232_streckenbefehl.Lok = 0x01;	 //Stop Rückwärts
	workRS232();
}

void run_rs232_test2(void) {
//Modul initialisieren
	initRS232();
	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Lok = 0x02; 	//Stop Vorwärts
	workRS232();
	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Lok = 0x06; 	//V_Abkuppeln Vorwärts
	workRS232();
	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Lok = 0x0A;	//V_Ankuppeln Vorwärts
	workRS232();
       	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Lok = 0x0E;	 //V_Fahrt Vorwärts
	workRS232();
	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Lok = 0x00;	 //Stop Rückwärts
	workRS232();
   	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Lok = 0x04;	 //V_Abkuppeln Rückwärts
	workRS232();
   	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Lok = 0x08;	 //V_Ankuppeln Rückwärts
	workRS232();
   	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Lok = 0x0C;	 //V_Fahrt Rückwärts
	workRS232();
   	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}	
	EV_RS232_streckenbefehl.Lok = 0x00;	 //Stop Rückwärts
	workRS232();
	
}

void run_rs232_test3(void) {

	initRS232();
	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Weiche = 0x06;	 //Weiche 3 Geradeaus
	workRS232();
	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Weiche = 0x07;	 //Weiche 3 Abbiegen
	workRS232();
	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Weiche = 0x08;	 //Weiche 4 Geradeaus
	workRS232();
	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Weiche = 0x09;	 //Weiche 4 Abbiegen
	workRS232();
       	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Weiche = 0x0A;	 //Weiche 5 Geradeaus
	workRS232();
	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Weiche = 0x0B;	 //Weiche 5 Abbiegen
	workRS232();


}

void run_rs232_test4(void) {
	initRS232();
	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Entkoppler = 0x0C;	 //Entkoppler 6 schalten
	workRS232();
	initRS232();
	while(!TasterPin) {	//Auf Tastendruck warten bevor Befehl abgeschickt wird
   	}
	EV_RS232_streckenbefehl.Entkoppler = 0x0E;	 //Entkoppler 7 schalten
	workRS232();
}  