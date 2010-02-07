/*****************************************************************************
 *
 *        Dateiname:    SoftwareWatchdog.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Felix Theodor Blueml
 *
 *
 *        Modul:        Software Watchdog, Version 1.6
 *
 *        Beschreibung:
 *        Der Software-Watchdog erkennt Deadlocks im Programmablauf. Bei
 *        Erkennung wird mit Hilfe des Not-Aus-Treibers das System
 *        angehalten.
 *        Deadlocks werden erkannt durch:
 *        - Zu viel verstrichene Zeit zwischen Rueckmeldungen
 *        - Pruefung auf gleiche Statusmeldungen von Modulen
 *
 ****************************************************************************/

/* Includes *****************************************************************/

#include "SoftwareWatchdog.h"
#include "Notaus.h"
#include "Betriebsmittelverwaltung.h"


/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/

/* Prototypen fuer lokale Funktionen ****************************************/

/* Funktionsimplementierungen ***********************************************/

/*
 * initSW()
 * Schnittstelle nur fuer das Modul Betriebsmittelverwaltung. Die 
 * Schnittstelle dient der Initialisierung eines Speicherfeldes fuer die
 * zuletzt gemeldeten Status der Module und der Scharfschaltung des
 * Hardwarezaehlers.
 * 
 * Nach Aufruf dieser Schnittstelle wird zuerst das Speicherfeld fuer die
 * zuletzt gemeldeten Status der Module in Form eines globalen Arrays mit dem
 * Wert FFh befuellt.
 * Danach wird der Hardwarezaehler Timer-0 des Mikrocontrollers C515C
 * scharfgeschaltet und der Interrupt Timer-0-Overflow freigegeben.
 * 
 * Rueckgabe: Keine
 * 
 * Autor: Felix Blueml
 */
void initSW()
{
	int i;
	for(i=0; i<6; i++)
	{
		// Statusspeicher initialisieren
		SW_status_array[i] = 0xff;
	}
	
	TR0 = 0;		// Timer0 ersteinmal stoppen

	TMOD &= ~(1<<3);	// setze GATE = 0; gebe Timer0 frei
	TMOD &= ~(1<<2);	// setze C/T = 0; verwende die Timer Funktion

	// waehle Mode 0; verwende 16 Bit fuer den Zaehler
	TMOD &= ~(1<<1);	// setze M1 = 0
	TMOD |=  (1<<0);	// setze M0 = 1

	// Zeitfensterkonfiguration des Hardwarezaehlers
	TL0 = SW_T0_STARTWERT_LO;
	TH0 = SW_T0_STARTWERT_HI;

	// "Timer 0 Overflow" gehoert zur Interruptgruppe 2, verwende IPx.1
	// Gebe dem Timer0-Interrupt die hoechste Prioritaet
	IP0 |= (1<<1);		// setze IP0.1 = 1
	IP1 |= (1<<1);		// setze IP1.1 = 1
	
	ET0 = 1;		// Freigabe des Timer0-Interrupts
	
	TR0 = 1;		// Scharfschalten des Timer0
}

/*
 * hello()
 * Schnittstelle nur fuer das Modul Betriebsmittelverwaltung. Die
 * Schnittstelle dient der Rueckmeldung innerhalb des vorgegebenen
 * Zeitfensters zur Bewaeltigung von Aufgaben. (Siehe dazu das Makro
 * Zeitfensterkonfiguration des Hardwarezaehlers in der Header-Datei
 * SoftwareWatchdog.h)
 * 
 * Nach Aufruf dieser Schnittstelle wird der Hardwarezaehler Timer-0 des
 * Mikrocontrollers C515C zurueckgesetzt.
 * 
 * Rueckgabe: Keine
 * 
 * Autor: Felix Blueml
 */
void hello()
{
	TR0 = 0;		// Timer0 stoppen
	
	// Zeitfensterkonfiguration des Hardwarezaehlers
	// Timer0 resetten
	TL0 = SW_T0_STARTWERT_LO;
	TH0 = SW_T0_STARTWERT_HI;
	
	TR0 = 1;		// Scharfschalten des Timer0
}

/*
 * helloModul(byte module_id, byte status)
 * Schnittstelle mit Statusmeldungen fuer Module mit zugeordneter Modulnummer.
 * (Siehe dazu den Prototypen-Kommentar in der Header-Datei 
 * SoftwareWatchdogHelloModul.h)
 * Die Schnittstelle dient der Ueberwachung der Taetigkeiten der Module.
 * 
 * Nach Aufruf dieser Schnittstelle wird zunaechst ueberprueft, ob die
 * erhaltene Modul-ID ungueltig ist. I.d.F. wird der Not-Aus-Treiber
 * kontaktiert und somit der Programmfluss angehalten.
 * Danach wird geprueft, ob der erhaltene Status bereits durch einen 
 * vorherigen Aufruf im globalen Array gesetzt wurde. I.d.F. wird ebenso der 
 * Not-Aus-Treiber kontaktiert.
 * Sollten beide Pruefungen negativ verlaufen sein, so wird der erhaltene 
 * Status im globalen Array gespeichert.
 * 
 * Rueckgabe: Keine
 * 
 * Autor: Felix Blueml
 */
void helloModul(byte module_id, byte status)
{
	// Modul-ID ungueltig oder ist der Status des Moduls seit dem letzten
	// Aufruf noch gleich geblieben?
	if(module_id > 6 || SW_status_array[module_id] == status)
	{
		emergency_off();	// Systemstopp!
	}
	
	SW_status_array[module_id] = status;
}

/*
 * stopSW()
 * Schnittstelle nur fuer das Modul Not-Aus-Treiber. Die Schnittstelle dient 
 * zum abschalten des Hardwarezaehlers.
 * 
 * Nach Aufruf dieser Schnittstelle wird der Hardwarezaehler Timer-0 des
 * Mikrocontrollers C515C inaktiv geschaltet. Dazu wird TR0 = 0 gesetzt.
 * 
 * Rueckgabe: Keine
 * 
 * Autor: Felix Blueml
 */
void stopSW()
{
	TR0 = 0;		// Timer0 stoppen
}

/*
 * timer0overflow()
 * Funktion zum Interrupt "Timer 0 Overflow", mit der Interruptvektoradresse
 * 0x000B. Bestimmung der Interruptnummer durch die Formel:
 * Nummer = (Interruptvektoradresse-3) / 8
 * Nummer = 1
 * Quelle: C fuer den Mikrocontroller 80C515C
 * 
 * Wird dieser Interrupt ausgeloest, so wird der Not-Aus-Treiber kontaktiert
 * und somit der Programmfluss angehalten.
 * 
 * Autor: Felix Blueml
 */
timer0overflow() interrupt 1
{
	emergency_off();	// Systemstopp!
}
