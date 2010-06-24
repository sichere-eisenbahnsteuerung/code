/*****************************************************************************
 *
 *        Dateiname:    Test_AuditingSystem.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Jan-Christopher Icken
 *
 *
 *        Modul:        Testskript für das Auditing-System-Modul
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
#include "AuditingSystem.h"
#include "Test_AuditingSystem.h"

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/
byte nachricht[6] = {0, 0, 0, 0, 0, 0};
/* Prototypen fuer lokale Funktionen ****************************************/
void run_auditingsystem_test1(void); //Leitzentrale Meldungen 
void run_auditingsystem_test2(void); //Befehlsvalidierung Meldungen
void run_auditingsystem_test3(void); //Ergebnisvalidierung Meldungen
void run_auditingsystem_test4(void); //Report All Messages
/* Funktionsimplementierungen ***********************************************/

void work_auditingsystem_tests(void) {
	run_auditingsystem_test1(); //Leitzentrale Meldungen 
	run_auditingsystem_test2(); //Befehlsvalidierung Meldungen
	run_auditingsystem_test3(); //Ergebnisvalidierung Meldungen
	run_auditingsystem_test4(); //Report All Messages		
}

void  run_auditingsystem_test1(void) {
	
}
	nachricht[0] = zustand;
	nachricht[1] = fehler;
	nachricht[2] = nextState;
	nachricht[3] = criticalStateCounter;
	nachricht[4] = zugPosition[0];
	nachricht[5] = zugPosition[1];
	
	sendMsg(MODUL_BV, nachricht);
