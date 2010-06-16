#ifndef Test_Sensoren_H
#define Test_Sensoren_H
/*****************************************************************************
 *
 *        Dateiname:    Test_Sensoren.h
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Norman Nie�
 *						Kai Dziembala
 *
 *        Modul:        Testspezifikation_Hardware-+System-Komponenten, 0.3
 *
 *        Beschreibung:
 *        Die beiden Lokomotiven werden mit angekoppelten Wagons nacheinander
 *        Multi-Maus-ferngesteuert, jeweils mit langsamer, mittlerer und maximaler
 *        Geschwindigkeit �ber das Streckennetz bewegt. Bei diesem manuellen Fahren
 *        muss sichergestellt sein, das jeder Lokomotiven- und Wagon- Magnet alle
 *        Hall-Sensoren mindestens einmal passiert und somit schaltet. Zeitgleich
 *        l�uft auf dem Mikrocontroller dauerhaft ein Steuerprogramm f�r das
 *        S88-R�ckmeldemodul, wodurch die jeweiligen Sensordaten kontinuierlich
 *        eingelesen werden. Ein Abgriff dieser jeweils vom S88-R�ckmeldemodul
 *        gelieferten Sensorzust�nde erfolgt �ber einen angeschlossenen Logikanalysator
 *        �Agilent Logic Wave�. Dessen Bin�rmesswerte sind mittels Rechnerkopplung und
 *        entsprechender Software einsehbar. Somit kann �berpr�ft werden, ob jeder Magnet
 *        bei den drei verschiedenen Fahrgeschwindigkeiten ein Sensorsignal f�r jeden
 *        Hall-Sensor erzeugt und somit die hardwareseitige Sensordatenerfassung
 *        funktioniert. Dieser Test muss jeweils f�r das R�ckmeldemodul 1 und 2
 *        durchgef�hrt werden, wodurch auch beide Sensors�tze auf Funktion �berpr�ft
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
