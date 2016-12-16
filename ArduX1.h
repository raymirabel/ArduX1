/**************************************************************************
 @FILE: 		ArduX1.cpp
 @AUTHOR:      	Raimundo Alfonso
 @COMPANY:		Ray Ingeniería Electronica, S.L.
 @DESCRIPTION: 	Librería para gestión del analizador de energía ArduX1
  
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

  
#ifndef ArduX1_h
#define ArduX1_h

#include "Arduino.h"
#include "ADE7753.h"


#define ON		  	1
#define OFF		  	0
#define RELAY     	9
#define AIN       	A6
#define DIN			A6
#define BUTTON    	A1
#define BUZZER	  	A0
#define DIPSW1		5
#define DIPSW2		6
#define DIPSW3		7
#define DIPSW4		8
#define DIPSW5		A3
#define DIPSW6		A7


class ArduX1: public ADE7753{
  public:
	ArduX1();
    void relayWrite(bool);			// Escribe el estado del relé
	bool relayRead(void);			// Lee el estado del relé
    void relayToggle(void);			// Alterna el estado del relé
    void buzzerWrite(bool);			// Escribe el estado del buzzer
    bool buzzerRead(void);			// Lee el estado del buzzer
    void buzzerToggle(void);		// Alterna el estado del buzzer
    bool digitalRead(int8_t);		// Lee la entrada AIN en modo digital
    uint8_t dipswitchRead(void);	// Lee el estado del dipswitch
    uint16_t analogRead(void);		// Lee la entrada AIN en modo analógico

	private:
};


#endif  // #ifndef ArduX1_h
