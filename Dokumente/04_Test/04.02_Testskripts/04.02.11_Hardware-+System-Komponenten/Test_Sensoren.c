/*****************************************************************************
 *
 *        Dateiname:    Test_Sensoren.c
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

/* Includes *****************************************************************/
#include <Test_Sensoren.h>

/* Definition globaler Konstanten *******************************************/

/* Definition globaler Variablen ********************************************/
int i;

/* Lokale Makros ************************************************************/
#define HIGH 1
#define LOW 0

/* Lokale Typen *************************************************************/

/* Lokale Konstanten ********************************************************/

/* Lokale Variablen *********************************************************/

/* Prototypen fuer lokale Funktionen ****************************************/
void initS88(void);
void getSensorData(void);
void wait(byte times);

/* Funktionsimplementierungen ***********************************************/

void main (void) {
	initS88();

	for(;;)	{
		getSensorData();
	}
}

void initS88(void) {
	S88_PS = LOW;
	S88_RESET = LOW;
	S88_CLK = LOW;
}

void getSensorData() {
	S88_PS = HIGH;
	wait(3);
	S88_CLK = HIGH;
	wait(9);
	S88_CLK = LOW;
	wait(9);
	S88_RESET = HIGH;
	wait(7);
	S88_RESET = LOW;
	wait(7);
	S88_PS = LOW;
	wait(9);

	for(i = 1;i< 16;i++) {
		S88_CLK = HIGH;
		wait(9);
		S88_CLK = LOW;
		wait(9);
	}
}

void wait(byte times) {
	while(times > 0) 
	{
		_nop_();
		times--;
	}
}