#ifndef SWTF3_H
#define SWTF3_H
/*****************************************************************************
 *
 *        Dateiname:    SWTF3.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Felix Theodor Blueml
 *
 *
 *        Testobjekt:   Software Watchdog, Version 1.6
 *
 *        Beschreibung:
 *        Diese Datei ist Teil des Testfalls 3 fuer den Programmcode zum Modul
 *        Software Watchdog in der Version 1.6.
 *        Testname: Modul-Rueckmeldung, positiv
 *        Szenario: Abschalten des SW und Modul-Rueckmeldung beim SW.
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


#endif /* SWTF3_H */
