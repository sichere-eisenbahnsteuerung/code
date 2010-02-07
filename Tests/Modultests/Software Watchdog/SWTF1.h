#ifndef SWTF1_H
#define SWTF1_H
/*****************************************************************************
 *
 *        Dateiname:    SWTF1.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Felix Theodor Blueml
 *
 *
 *        Testobjekt:   Software Watchdog, Version 1.6
 *
 *        Beschreibung:
 *        Diese Datei ist Teil des Testfalls 1 fuer den Programmcode zum Modul
 *        Software Watchdog in der Version 1.6.
 *        Testname: Rueckmeldung, abschalten
 *        Szenario: Rueckmeldung beim SW und hinterher Abschalten des SW.
 *        Testspezifikationsversion: 0.1
 *
 ****************************************************************************/




/* Globale Makrodefinitionen ************************************************/

#define TRUE 1 
#define FALSE 0 

/* Globale Typdefinitionen **************************************************/

typedef unsigned char byte;
typedef bit boolean;	

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/

sbit NOTAUS_PIN	= P3^4;

/* Deklaration von globalen Funktionen **************************************/


#endif /* SWTF1_H */
