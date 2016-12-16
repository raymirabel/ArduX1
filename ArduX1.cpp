/**************************************************************************
 @FILE: 		ArduX1.cpp
 @AUTHOR:      	Raimundo Alfonso
 @COMPANY:		Ray Ingenieria Electronica, S.L.
 @DESCRIPTION: 	Librería para gestión del analizador de energía ArduX1
  
 @LICENCE DETAILS:
	Esta librería está basada en software libre. Tu puedes redistribuir
	y/o modificar esta librería bajo los terminos de licencia GNU.

	Esta biblioteca se distribuye con la esperanza de que sea útil,
	pero SIN NINGUNA GARANTÍA, incluso sin la garantía implícita de
	COMERCIALIZACIÓN O PARA UN PROPÓSITO PARTICULAR.
	Consulte los terminos de licencia GNU para más detalles:
                                                                       
	http://www.gnu.org/licenses/gpl-3.0.txt

 @VERSIONS:
	01-12-2016 - v1.00 :
  
**************************************************************************/
  
  
/**************************************************************************
 * Includes
 *************************************************************************/  
#include <ArduX1.h>

  
/**************************************************************************
 * Constructors
 *************************************************************************/
 ArduX1::ArduX1(void){
	pinMode(RELAY, OUTPUT);
	pinMode(BUZZER, OUTPUT);
	pinMode(BUTTON, INPUT);
	pinMode(DIPSW1, INPUT);
	pinMode(DIPSW2, INPUT);
	pinMode(DIPSW3, INPUT);
	pinMode(DIPSW4, INPUT);
	pinMode(DIPSW5, INPUT);

	digitalWrite(RELAY, OFF);
	digitalWrite(BUZZER, OFF); 
}
 
 /**************************************************************************
 * User functions
 **************************************************************************/
void ArduX1::relayWrite(bool status){
	digitalWrite(RELAY,status);
}

bool ArduX1::relayRead(void){
	return(digitalRead(RELAY));
}

void ArduX1::relayToggle(void){
	digitalWrite(RELAY,!digitalRead(RELAY));
}

void ArduX1::buzzerWrite(bool status){
	digitalWrite(BUZZER,status);
}

bool ArduX1::buzzerRead(void){
	return(digitalRead(BUZZER));
}

void ArduX1::buzzerToggle(void){
	digitalWrite(BUZZER,!digitalRead(BUZZER));
}

uint16_t ArduX1::analogRead(void){
	return(::analogRead(AIN));
}

bool ArduX1::digitalRead(int8_t din){
	if(din == DIN){
		if(::analogRead(AIN)>512)
			return(ON);
		else
			return(OFF);
	}else{
		return(!::digitalRead(din));
	}
}

uint8_t ArduX1::dipswitchRead(void){
	byte a0,a1,a2,a3,a4,a5;

	a0 = !::digitalRead(DIPSW1);  
	a1 = !::digitalRead(DIPSW2);
	a2 = !::digitalRead(DIPSW3);
	a3 = !::digitalRead(DIPSW4);
	a4 = !::digitalRead(DIPSW5);
	if(::analogRead(DIPSW6) > 512) a5 = 0; else a5 = 1;  
	return(a0 + a1*2 + a2*4 + a3*8 + a4*16 + a5*32);
}


