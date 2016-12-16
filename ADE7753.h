/**************************************************************************
 @FILE: 		ADE7753.h
 @AUTHOR:      	Raimundo Alfonso
 @COMPANY:		Ray Ingeniería Electronica, S.L.
 @DESCRIPTION: 	Librería para gestión del chip analizador de energía ADE7753
 				http://www.analog.com/media/en/technical-documentation/data-sheets/ADE7753.pdf
  
 @LICENCE DETAILS:
	Esta librería está basada en software libre. Tu puedes redistribuir
	y/o modificar esta librería bajo los términos de licencia GNU.

	Esta biblioteca se distribuye con la esperanza de que sea útil,
	pero SIN NINGUNA GARANTÍA, incluso sin la garantía implícita de
	COMERCIALIZACIÓN O PARA UN PROPÓSITO PARTICULAR.
	Consulte los términos de licencia GNU para más detalles:
                                                                       
	http://www.gnu.org/licenses/gpl-3.0.txt

 @VERSIONS:
	01-12-2016 - v1.00 :
  
**************************************************************************/

  
#ifndef ADE7753_h
#define ADE7753_h

#include <stdint.h>
#include <EEPROM.h>
#include "Arduino.h"
#include "Cal.h"

#define PIN_CS  		10			// Define chip select ADE7753
#define PIN_IRQ 		2			// Define interrupt input for ADE7753 (only hardware interrupt supported)
#define LINE_CYCLE		200			// Define Line Cycle Accumulation time in half line cycles (i.e. 200 = 2 seconds)
#define POS_ACC 		0			// Positive accumulacion (0=accumulacion positive and negative; 1=accumulation positive only)
#define T_SAVE_EEPROM	600			// How long the data is recorded in the eeprom in seconds (recommended time 10 minutes)

// EEPROM save energy registers...
#define DIR_EE_TEST       0 
#define DIR_EE_EACTIVA    10
#define DIR_EE_EREACTIVA  14
#define DIR_EE_EAPARENTE  18

// ADE7753 internal registers...
const byte READ  		= 0b01111111;   // read command communication register
const byte WRITE 		= 0b10000000;   // write command communication register
const byte LAENERGY		= 0x04;
const byte LVAENERGY	= 0x07;
const byte LVARENERGY	= 0x08;
const byte MODE  		= 0x09;
const byte IRQEN  		= 0x0A;
const byte RSTSTATUS 	= 0x0C;
const byte PHCAL	 	= 0x10;
const byte WGAIN	 	= 0x12;
const byte CFNUM  		= 0x14;
const byte CFDEN  		= 0x15;
const byte IRMS  		= 0x16;
const byte VRMS  		= 0x17;
const byte IRMSOS  		= 0x18;
const byte VRMSOS  		= 0x19;
const byte VAGAIN	 	= 0x1A;
const byte LINECYC  	= 0x1C;
const byte IPEAK  		= 0x22;
const byte RSTIPEAK  	= 0x23;
const byte VPEAK  		= 0x24;
const byte RSTVPEAK  	= 0x25;
const byte PERIOD  		= 0x27;

class ADE7753{
  public:
	ADE7753();
	float vrmsRead(void);		// Lee voltaje RMS			(V)
	float irmsRead(void);		// Lee corriente RSM		(A)
	float freqRead(void);		// Lee frecuencia de red 	(Hz)
	float vPeakRead(void);		// Lee voltaje RMS de pico	(V)
	float iPeakRead(void);		// Lee corriente RMS de pico(V)
	float pActiveRead(void);	// Lee potencia activa		(W)
	float pReactiveRead(void);	// Lee potencia reactiva	(VAR)
	float pApparentRead(void);	// Lee potencia aparente	(VA)
	void vPeakReset(void);		// Reset voltaje de pico
	void iPeakReset(void);		// Reset corriente de pico	
	float pfRead(void);			// Lee factor de potencia	
	float eActiveRead(void);	// Lee energía activa		(KWh)
	float eReactiveRead(void);	// Lee energía reactiva		(KVARh)
	float eApparentRead(void);	// Lee energía aparente		(KVAh)
	void eClear(void);			// Reset energías

	static uint8_t  readRegister(uint8_t);
	static uint16_t readRegister16(uint8_t);
	static uint16_t readRegister12u(uint8_t);
	static int16_t  readRegister12s(uint8_t);
	static uint32_t readRegister24u(uint8_t);
	static int32_t  readRegister24s(uint8_t);
	
  
  private:
	void writeRegister(uint8_t, uint8_t);
	void writeRegister12s(uint8_t, int16_t);
	void writeRegister12u(uint8_t, uint16_t);
	void writeRegister16(uint8_t, uint16_t);

	
};

#endif  // #ifndef ADE7753_h
