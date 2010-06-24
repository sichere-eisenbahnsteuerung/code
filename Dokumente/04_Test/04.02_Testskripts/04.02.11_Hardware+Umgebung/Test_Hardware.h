#ifndef Test_Hardware_H
#define Test_Hardware_H
/*****************************************************************************
 *
 *        Dateiname:    Test_Hardware.h
 *
 *        Projekt:      Sichere Eisenbahnsteuerung
 *
 *        Autor:        Norman Nie�
 *						Kai Dziembala
 *
 *        Modul:        Testspezifikation Hardware+Umgebungskomponenten, 1.0
 *        Beschreibung:
 *        Mit einem Mikrocontroller-Programm wird die Lokomotive 1 dauerhaft auf dem Hauptgleis
 *	  	  vorw�rts bewegt. Gleichzeitig werden �ber den Arduino Debug-Ausgaben get�tigt. Somit kann
 *        sichergestellt werden, dass der Mikrocontroller, die Multimaus, der XpressNet-Adapter, der
 *        DCC-Verst�rker, die entsprechenden Transformatoren, die Lokomotive 1, der Arduino und die
 *        jeweilige Verkabelung funktionst�chtig sind.
 *        Dieser Test muss sowohl mit dem Mikrocontroller eins und zwei durchgef�hrt werden.
 *
 ****************************************************************************/

#include <reg515c.h>
/* Globale Makrodefinitionen ************************************************/
#ifndef MODUL_LZ
#define MODUL_LZ 0
#endif

#ifndef MODUL_BV
#define MODUL_BV 1
#endif

#ifndef MODUL_EV
#define MODUL_EV 2
#endif	

/* Globale Typdefinitionen **************************************************/

/* Deklaration von globalen Konstanten **************************************/
#define V_STOPP 0x00
#define V_Abkuppeln 0x08
#define V_Ankuppeln 0x1F
#define V_Fahrt 0xDD
#define Lok1_address 0x01
#define Lok2_address 0x02
#define W1_address 0x03
#define W2_address 0x04
#define W3_address 0x02
#define EK1_address 0x06
#define EK2_address 0x05
#define CTS_readyToSend 1
#define CTS_stop_send 0

/* Deklaration von globalen Variablen ***************************************/
sbit RS232TREIBER_CTSPIN = P3^5;

/* Deklaration von globalen Funktionen **************************************/
void main(void);

#endif /* Test_Hardware_H */
