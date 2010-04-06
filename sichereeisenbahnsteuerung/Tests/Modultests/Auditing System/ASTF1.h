#ifndef ASTF1_H
#define ASTF1_H
/*****************************************************************************
 *
 *        Dateiname:    ASTF1.c
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Felix Theodor Blueml
 *
 *
 *        Testobjekt:   Auditing-System, Version 0.4
 *
 *        Beschreibung:
 *        Diese Datei ist Teil des Testfalls 1 fuer den Programmcode zum Modul
 *        Auditing-System in der Version 0.4.
 *        Testname: Uebertragungstest
 *        Szenario: Test auf korrekte Uebertragung.
 *        Testspezifikationsversion: noch keine vorhanden
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

sbit AS_PORT_I2C_SDA = P5^4;
sbit AS_PORT_I2C_SCL = P5^5;

/* Deklaration von globalen Funktionen **************************************/


#endif /* ASTF1_H */
