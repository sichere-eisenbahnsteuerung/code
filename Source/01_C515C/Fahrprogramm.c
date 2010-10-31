/*****************************************************************************
*
* Dateiname:    Fahrprogramm.c
*
* Projekt:      Sichere Eisenbahnsteuerung
*
* Autor:        Thomas Musialski
*
*
* Modul:        Fahrprogramm, Version 0.2
*
* Beschreibung:
* Das Fahrprogramm hat die Aufgabe die notwendige Funktionalität
* für die im Pflichtenheft beschriebene Fahraufgabe bereitzustellen.
* Das Modul Fahraufgabe befindet sich in der Anwenderschicht und steht
* in direkter Kommunikation mit der Leitzentrale.
* Die notwendigen Anweisungen für eine Fahraufgabe werden in einem
*Ringpuffer hinterlegt und der Leitzentrale zur Verfügung gestellt.
*
****************************************************************************/

/* Includes *****************************************************************/
#include "Fahrprogramm.h"
#include "Betriebsmittelverwaltung.h"


/* Definition globaler Konstanten *******************************************/

#define lok1    0x0        // Lokomotive 0 nur im Zugbetrieb
#define lok2    0x1        // Lokomotive 1 im Zug- und Rangierbetrieb
#define err     0xFF       // Error
#define null    '\0'       // Null

// Fahrprogramm: Groeße des verwendeten Arrays
#define row     50          // Zeilen
#define col     2           // Spalten

/* Definition globaler Variablen ********************************************/

/* Lokale Makros ************************************************************/

/* Lokale Typen *************************************************************/

typedef int     Fahrprogramm[row][col];     // Definition Fahrprogrammarrays

/*
 * \enum Gleisabschnitt
 *
 * Definition aller verwendeten Geisabschnitte. Das verwendete Gleislayout, 
 * siehe 'Modul-Design Befehlsvaledierung', besteht aus 9 Geisabschnitten. 
 * Eine Gleisabschnitt ist 1 byte gross.
 */
typedef enum 
{
    gleisAbschnitt_1    = 0x1,
    gleisAbschnitt_2,
    gleisAbschnitt_3,
    gleisAbschnitt_4,
    gleisAbschnitt_5,
    gleisAbschnitt_6,
    gleisAbschnitt_7,
    gleisAbschnitt_8,
    gleisAbschnitt_9,
} Gleisabschnitt;

/*
 * \enum Fahrbefehl
 *
 * Definition aller möglichen Fahrbefehle.
 */
typedef enum 
{
    fahreLok1           = 0x0,
    fahreLok2           = 0x1,
    kuppelLok2          = 0x3,
    abkuppelLok2        = 0x5,
    haltLok2_10         = 0x7,
    haltLok2_20         = 0x17,
    haltLok2_30         = 0x27,
} Fahrbefehl;

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/

/*
 * Fahrprogramm_1 enthält Fahrbefehle für die Lok#1 (nur Zugbetreieb).
 * Fahraufgabe, siehe 'Pflichtenheft'.
 */
static Fahrprogramm fahrprogramm_1  = 
{
    { fahreLok1,    gleisAbschnitt_4 },
    { fahreLok1,    gleisAbschnitt_5 },
    { fahreLok1,    gleisAbschnitt_6 },
    { fahreLok1,    gleisAbschnitt_1 },
    { fahreLok1,    gleisAbschnitt_7 },
};

/*
 * Fahrprogramm_2 enthält Fahrbefehle für die Lok#2 (Zug- und Rangierbetreieb).
 * Fahraufgabe, siehe 'Pflichtenheft'
 */
static Fahrprogramm fahrprogramm_2  = 
{
    { fahreLok2,    gleisAbschnitt_1 },
    { fahreLok2,    gleisAbschnitt_7 },
    { fahreLok2,    gleisAbschnitt_4 },
    { fahreLok2,    gleisAbschnitt_3 },
    { kuppelLok2,   gleisAbschnitt_2 },     // Ankuppeln der 3 Wagons
    { fahreLok2,    gleisAbschnitt_1 },
    { fahreLok2,    gleisAbschnitt_8 },
    { abkuppelLok2, gleisAbschnitt_9 },     // Abkupeln der Wagons
    { fahreLok2,    gleisAbschnitt_8 },
    { fahreLok2,    gleisAbschnitt_1 },
    { fahreLok2,    gleisAbschnitt_2 },
    { haltLok2_20,  gleisAbschnitt_2 },     // Warte 20 Sekunden
    { fahreLok2,    gleisAbschnitt_1 },
    { fahreLok2,    gleisAbschnitt_8 },
    { kuppelLok2,   gleisAbschnitt_9 },     // Ankuppeln der drei Wagons
    { fahreLok2,    gleisAbschnitt_8 },
    { fahreLok2,    gleisAbschnitt_1 },
    { fahreLok2,    gleisAbschnitt_2 },
    { fahreLok2,    gleisAbschnitt_3 },
    { abkuppelLok2, gleisAbschnitt_2 },     // Abkupeln der Wagons
    { fahreLok2,    gleisAbschnitt_3 },
    { fahreLok2,    gleisAbschnitt_4 },
    { fahreLok2,    gleisAbschnitt_5 },
    { fahreLok2,    gleisAbschnitt_6 },
    { fahreLok2,    gleisAbschnitt_1 },
    { fahreLok2,    gleisAbschnitt_8 },
    { haltLok2_20,  gleisAbschnitt_8 },     // Warte 20 Sekunden
};

/* Prototypen fuer lokale Funktionen ****************************************/

/* Funktionsimplementierungen ***********************************************/

/*
 * @brief   Initialisierung des Fahrprogrammes
 *
 *          Initialisierung der wichtigsten Einstellungen fuer das Fahrprogramm.
 */
void initFP()
{
}

/*
 * @brief   Ermittlung des aktuellen Kommandos für eine Lok
 *
 *          Die Methode ermittelt anhand der uebergeben Lok die naechste Fahranweisung. Die
 *          Fahranweisungen sind für jede Lok gemaess des Pflichenhefts statisch im Code
 *          des Fahrprogrammes definiert.
 *
 * @param   lok     Für welche Lok soll die nächste Fahranweisung ermittelt werden.
 * @return  Nächste Fahranweisung für die übergebene
 */

Fahranweisung get_command(byte lok)
{
    // Counter für Fahrprogramm f1 und f2
    static unsigned int f1_Index = 0;
    static unsigned int f2_Index = 0;
    Fahranweisung anweisung;

    /*
     * Überprüfen des Wertes von 'lok' und Übergabe der jeweils gültigen
     * Fahranweisung an die Leitzentrale. Die Fahraufgabe beinhaltet 2 Loks,
     * siehe 'Pflichtenheft'.
     */
    if (lok == lok1)
    {
        if (fahrprogramm_1[f1_Index][1] == null) // Prüfung, fahrprogramm_1 Array Ende
        {
            f1_Index = 0;
        }

        // Aufruf von gültiger Fahranweisung
        anweisung.fahrbefehl = fahrprogramm_1[f1_Index][0];
        anweisung.gleisabschnittNr = fahrprogramm_1[f1_Index][1];

        f1_Index ++;

        // Rückgabe aktuelle Fahranweisung, anweisung = (fahrbefehl_n,gleisabschnit_n)
        return anweisung;

    }
    else if (lok == lok2)
    { 
        if (fahrprogramm_2[f2_Index][1] == null) // Prüfung, fahrprogramm_2 Array Ende
        {
            f2_Index = 0;
        }

        // Aufruf von gültiger Fahranweisung
        anweisung.fahrbefehl = fahrprogramm_2[f2_Index][0];
        anweisung.gleisabschnittNr = fahrprogramm_2[f2_Index][1];

        f2_Index++;

        // Rückgabe aktuelle Fahranweisung, anweisung = (fahrbefehl_n,gleisabschnit_n)
        return anweisung;
    }
    // Fehler Anweisung bei falscher bzw. nicht vorhandener Lok
    else
    {
        anweisung.fahrbefehl = err;
        anweisung.gleisabschnittNr = err;
 
        // Rückgabe aktuelle Anweisung = (Fahrbefehl_err,Gleisabschnit_err)
        return anweisung;
    }
}

