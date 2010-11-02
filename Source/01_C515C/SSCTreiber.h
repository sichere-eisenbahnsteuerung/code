/**
 * @file     SSCTreiber.h
 *
 * @author   Matthias Buss
 *   
 * @brief    Das SSC-Modul sorgt fuer die Kommunikation zwischen den
 *  	        redudanten Mikrocontrollern
 *
 * @date     13.01.2010
 */


#ifndef SSCTreiber_H
#define SSCTreiber_H

/**
 * @brief   Initialisiert den SSC Treiber. 
 *          @Note: Vor workSSC() aufrufen.
 */
void initSSC();

/**
 * @brief   Schnittstelle des Moduls, wird von auﬂerhalb aufgerufen. 
 */
void workSSC();


#endif /* SSCTreiber_H */
