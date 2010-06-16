#ifndef Test_Sensoren_H
#define Test_Sensoren_H
/*****************************************************************************
 *
 *        Dateiname:    Test_Sensoren.h
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Norman Nieß
 *						Kai Dziembala
 *
 *        Modul:        Testspezifikation_Hardware-+System-Komponenten, 0.3
 *
 *        Beschreibung:
 *        Die beiden Lokomotiven werden mit angekoppelten Wagons nacheinander
 *        Multi-Maus-ferngesteuert, jeweils mit langsamer, mittlerer und maximaler
 *        Geschwindigkeit über das Streckennetz bewegt. Bei diesem manuellen Fahren
 *        muss sichergestellt sein, das jeder Lokomotiven- und Wagon- Magnet alle
 *        Hall-Sensoren mindestens einmal passiert und somit schaltet. Zeitgleich
 *        läuft auf dem Mikrocontroller dauerhaft ein Steuerprogramm für das
 *        S88-Rückmeldemodul, wodurch die jeweiligen Sensordaten kontinuierlich
 *        eingelesen werden. Ein Abgriff dieser jeweils vom S88-Rückmeldemodul
 *        gelieferten Sensorzustände erfolgt über einen angeschlossenen Logikanalysator
 *        „Agilent Logic Wave“. Dessen Binärmesswerte sind mittels Rechnerkopplung und
 *        entsprechender Software einsehbar. Somit kann überprüft werden, ob jeder Magnet
 *        bei den drei verschiedenen Fahrgeschwindigkeiten ein Sensorsignal für jeden
 *        Hall-Sensor erzeugt und somit die hardwareseitige Sensordatenerfassung
 *        funktioniert. Dieser Test muss jeweils für das Rückmeldemodul 1 und 2
 *        durchgeführt werden, wodurch auch beide Sensorsätze auf Funktion überprüft
 *        werden.
 *
 ****************************************************************************/

#include <reg515c.h>
/* Globale Makrodefinitionen ************************************************/

/* Globale Typdefinitionen **************************************************/

/* Deklaration von globalen Konstanten **************************************/

/* Deklaration von globalen Variablen ***************************************/
sbit S88_PS = P5^0; 
sbit S88_RESET = P5^1;     
sbit S88_CLK = P5^2;
	/**
	* sbit Zuweisung muss in einer Headerdatei erfolgen
	* siehe: http://www.keil.com/support/docs/1317.htm
	**/

/* Deklaration von globalen Funktionen **************************************/



#endif /* Test_Sensoren_H */
