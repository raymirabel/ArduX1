/**************************************************************************
 @FILE: 		ADE7753.cpp
 @AUTHOR:      	Raimundo Alfonso
 @COMPANY:		Ray Ingenieria Electronica, S.L.
 @DESCRIPTION: 	Librería para gestión del chip analizador de energía ADE7753
 				http://www.analog.com/media/en/technical-documentation/data-sheets/ADE7753.pdf
  
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
#include <ADE7753.h>
#include <SPI.h>
#include <stddef.h>
#include <stdint.h>

void ADE7753_IRQ(void);
void lee_eeprom(void);
void escribe_eeprom(void);
void write_eeprom_float(unsigned int, float);
void write_eeprom_char(unsigned int, char);
float read_eeprom_float(unsigned int);
char read_eeprom_char(unsigned int);
volatile float _freq;
volatile float _vrms;
volatile float _irms;
volatile float _vpeak;
volatile float _ipeak;
volatile float _pActiva;
volatile float _pReactiva;
volatile float _pAparente;
volatile float _eActiva;
volatile float _eReactiva;
volatile float _eAparente;
 
  
/**************************************************************************
 * Constructors
 *************************************************************************/
ADE7753::ADE7753(void){
	pinMode(PIN_CS, OUTPUT);
	pinMode(PIN_IRQ, INPUT);
	digitalWrite(PIN_CS, HIGH);
	SPI.begin();
	SPI.setDataMode(SPI_MODE1);
	
	_freq = 0;
	_vrms = 0;
	_irms = 0;
	_vpeak = 0;
	_ipeak = 0;
	_pActiva = 0;
	_pReactiva = 0;
	_pAparente = 0;	
	_eActiva = 0;
	_eAparente = 0;
	_eReactiva = 0;
	  
    // Salida de pulsos...
	writeRegister12u(CFNUM,0x0);
	writeRegister12u(CFDEN,CFDEN_VALUE);
	
	// Activa CYCMODE... acumula cada 1 segundo
	writeRegister16(MODE,0x0088 | (POS_ACC * 0x8000));
	//writeRegister16(MODE,0x0388);
	writeRegister16(LINECYC,LINE_CYCLE);  
	writeRegister16(IRQEN,0x44);
	
	// Offset corriente
	writeRegister12s(IRMSOS, ((IRMS_OFFSET * 70.0)/0.025));
	
	// Correccion fase
	writeRegister(PHCAL,-31);
	
	// Calibración potencia activa y aparente...
	writeRegister12s(WGAIN, (-(1-(P_ACT_CAL)) * 4095));
	writeRegister12s(VAGAIN, (-(1-(P_APA_CAL)) * 4095));
	
	// Interrupcion ADE7753
	attachInterrupt(digitalPinToInterrupt(PIN_IRQ), ADE7753_IRQ, LOW);
    readRegister16(RSTSTATUS);
	
	// Reset valores de pico...
	ADE7753::readRegister24u(RSTVPEAK);
	ADE7753::readRegister24u(RSTIPEAK);
	
	// Inicializa eeprom...
	if(EEPROM.read(DIR_EE_TEST) == 0xFF)
		escribe_eeprom();
	else
		lee_eeprom();	
	
}

 /**************************************************************************
 * ADE7753 high level functions
 **************************************************************************/
 
float ADE7753::freqRead(void){
	return(_freq);
}
 
float ADE7753::vrmsRead(void){
	return(_vrms);
}
 
float ADE7753::vPeakRead(void){
	return(_vpeak);
}

void ADE7753::vPeakReset(void){
	ADE7753::readRegister24u(RSTVPEAK);
}

float ADE7753::iPeakRead(void){
	return(_ipeak);
}

void ADE7753::iPeakReset(void){
	ADE7753::readRegister24u(RSTIPEAK);
}
 
float ADE7753::irmsRead(void){
	return(_irms);
}
 
float ADE7753::pActiveRead(void){
	return(_pActiva);
}

float ADE7753::pReactiveRead(void){
	return(_pReactiva);
}

float ADE7753::pApparentRead(void){
	return(_pAparente);
}

float ADE7753::pfRead(void){
	return(abs(_pActiva/_pAparente));
}

float ADE7753::eActiveRead(void){
	return(_eActiva);
}

float ADE7753::eReactiveRead(void){
	return(_eReactiva);
}

float ADE7753::eApparentRead(void){
	return(_eAparente);
}

void ADE7753::eClear(void){
	_eActiva = 0;
	_eReactiva = 0;
	_eAparente = 0;
	escribe_eeprom();
}
 
 /**************************************************************************
 * ADE7753 low level functions
 **************************************************************************/
 
void ADE7753_IRQ(void){
	static unsigned long timer_counter = 0;
	uint32_t lTemp;
	int32_t  lsTemp;
	uint16_t uTemp;

//	digitalWrite(A2, HIGH);
	
	// Reset flag interrupciones...
	ADE7753::readRegister16(RSTSTATUS);
		
	// Lectura de frecuencia...
	uTemp = ADE7753::readRegister16(PERIOD);
	_freq = 448003.0 / (float)uTemp;
	
	// Lectura de Vrms...
	lTemp = ADE7753::readRegister24u(VRMS);
	_vrms = (lTemp * 374.5 * VRMS_CAL) / 1561400.0;
	
	// Lectura de vpeak...
	lTemp = ADE7753::readRegister24u(VPEAK);
	_vpeak = (lTemp * 374.5 * V_PEAK_CAL) / (2 * 9900.0);

	// Lectura de Irms...
	lTemp = ADE7753::readRegister24u(IRMS);
	_irms = (lTemp * 30.53 * IRMS_CAL) / 1868467.0;

	// Lectura de ipeak...
	lTemp = ADE7753::readRegister24u(IPEAK);
	_ipeak = (lTemp * 30.53 * I_PEAK_CAL) / 2642412.0;
	
	// Lectura potencia activa...
	lsTemp = ADE7753::readRegister24s(LAENERGY);
	_pActiva = (float)(lsTemp / 4);
	
	// Lectura potencia reactiva...
	lsTemp = ADE7753::readRegister24s(LVARENERGY);
	_pReactiva = (float)lsTemp * P_REA_CAL;
	
	// Lectura potencia aparente...
	lsTemp = ADE7753::readRegister24s(LVAENERGY);
    _pAparente = (float)(lsTemp / 4);
	
	// Calcula energías en KWh...
	_eActiva   += ((_pActiva   * (LINE_CYCLE / 100.0))/3600000.0);
	_eReactiva += ((_pReactiva * (LINE_CYCLE / 100.0))/3600000.0);
	_eAparente += ((_pAparente * (LINE_CYCLE / 100.0))/3600000.0);
	
	// Graba en eeprom...
	if(timer_counter++ >= ((T_SAVE_EEPROM * 100L) / LINE_CYCLE)){
		timer_counter = 0;
		escribe_eeprom();
/*		
		digitalWrite(A0, HIGH);
		delay(50);
		digitalWrite(A0, LOW);
*/		
	}
	
//	digitalWrite(A2, LOW);
}

void lee_eeprom(void){
  _eActiva   = read_eeprom_float(DIR_EE_EACTIVA);
  _eReactiva = read_eeprom_float(DIR_EE_EREACTIVA); 
  _eAparente = read_eeprom_float(DIR_EE_EAPARENTE); 
}

void escribe_eeprom(void){
  write_eeprom_char(DIR_EE_TEST,   0);
  write_eeprom_float(DIR_EE_EACTIVA, _eActiva);
  write_eeprom_float(DIR_EE_EREACTIVA, _eReactiva);
  write_eeprom_float(DIR_EE_EAPARENTE, _eAparente);
}

void write_eeprom_float(unsigned int dir, float valor){
	char temp;
	char *pChar;

	pChar = (char*)&valor;
	for(temp=0; temp<4; temp++)
        	EEPROM.write (dir + temp, *(pChar++));
}

void write_eeprom_char(unsigned int dir, char valor){
	if(read_eeprom_char(dir) != valor) EEPROM.write (dir, valor);
}

char read_eeprom_char(unsigned int dir){
	char temp;

   	temp = EEPROM.read(dir);
	return(temp);
}

float read_eeprom_float(unsigned int dir){
	char n;
	char *pChar;
	float temp;

	pChar = (char*)&temp;
	for(n=0; n<4; n++)
      	*(pChar++) = EEPROM.read(dir + n);
	return(temp);
}
 
uint8_t ADE7753::readRegister(uint8_t reg){
	uint8_t inByte = 0;   
	uint8_t n;
	uint8_t result = 0;   
	uint8_t dataToSend = reg & READ;

	digitalWrite(PIN_CS, LOW);
	SPI.transfer(dataToSend);
	delayMicroseconds(4);
	result = SPI.transfer(0x00);
	digitalWrite(PIN_CS, HIGH);
	return(result);
}

int16_t ADE7753::readRegister12s(uint8_t reg){
	uint8_t inByte = 0;   
	uint8_t n;
	uint16_t result = 0;   
	uint8_t dataToSend = reg & READ;

	digitalWrite(PIN_CS, LOW);
	SPI.transfer(dataToSend);
	delayMicroseconds(4);
	for(n=0;n<2;n++){
		result = result << 8;
		inByte = SPI.transfer(0x00);
		result = result | inByte;
	}
	digitalWrite(PIN_CS, HIGH);
	result = result << 4;
	result = result >> 4; 
	//if(bitRead(result,11)){	// Resultado negativo...
	//	result |= 0xF000;
	//}
	return(result);
}

uint16_t ADE7753::readRegister12u(uint8_t reg){
	uint8_t inByte = 0;   
	uint8_t n;
	uint16_t result = 0;   
	uint8_t dataToSend = reg & READ;

	digitalWrite(PIN_CS, LOW);
	SPI.transfer(dataToSend);
	delayMicroseconds(4);
	for(n=0;n<2;n++){
		result = result << 8;
		inByte = SPI.transfer(0x00);
		result = result | inByte;
	}
	digitalWrite(PIN_CS, HIGH);
	return(result);
}
 
uint16_t ADE7753::readRegister16(uint8_t reg){
	uint8_t inByte = 0;   
	uint8_t n;
	uint16_t result = 0;   
	uint8_t dataToSend = reg & READ;

	digitalWrite(PIN_CS, LOW);
	SPI.transfer(dataToSend);
	delayMicroseconds(4);
	for(n=0;n<2;n++){
		result = result << 8;
		inByte = SPI.transfer(0x00);
		result = result | inByte;
	}
	digitalWrite(PIN_CS, HIGH);
	return(result);
}

uint32_t ADE7753::readRegister24u(uint8_t reg) {
	uint8_t inByte = 0;   
	uint8_t n;
	uint32_t result = 0;  
	uint8_t dataToSend = reg & READ;

	digitalWrite(PIN_CS, LOW);
	SPI.transfer(dataToSend);
	delayMicroseconds(4);
	for(n=0;n<3;n++){
		result = result << 8;
		inByte = SPI.transfer(0x00);
		result = result | inByte;
	}
	digitalWrite(PIN_CS, HIGH);
	//result = result << 8;
	//result = result >> 8;  
	return(result);
}

int32_t ADE7753::readRegister24s(uint8_t reg) {
	uint8_t inByte = 0;   
	uint8_t n;
	int32_t result = 0;  
	uint8_t dataToSend = reg & READ;

	digitalWrite(PIN_CS, LOW);
	SPI.transfer(dataToSend);
	delayMicroseconds(4);
	for(n=0;n<3;n++){
		result = result << 8;
		inByte = SPI.transfer(0x00);
		result = result | inByte;
	}
	digitalWrite(PIN_CS, HIGH);
	result = result << 8;
	result = result >> 8;  
	return(result);
}
 
 void ADE7753::writeRegister(uint8_t reg, uint8_t value) {
	uint8_t dataToSend = reg | WRITE;

	digitalWrite(PIN_CS, LOW);
	SPI.transfer(dataToSend); 
	delayMicroseconds(1);  
	SPI.transfer(value);  
	digitalWrite(PIN_CS, HIGH);
}

void ADE7753::writeRegister12s(uint8_t reg, int16_t value) {
	uint8_t dataToSend = reg | WRITE;
  
	bitWrite(value,11,bitRead(value,15));
	value &= 0x0FFF;  
	digitalWrite(PIN_CS, LOW);
	SPI.transfer(dataToSend); 
	delayMicroseconds(1);
	SPI.transfer(highByte(value));  
	SPI.transfer(lowByte(value));    
	digitalWrite(PIN_CS, HIGH);
}

void ADE7753::writeRegister12u(uint8_t reg, uint16_t value){
	value &= 0x0FFF;  
	uint8_t dataToSend = reg | WRITE;
  
	digitalWrite(PIN_CS, LOW);
	SPI.transfer(dataToSend); 
	delayMicroseconds(1);
	SPI.transfer(highByte(value));  
	SPI.transfer(lowByte(value));    
	digitalWrite(PIN_CS, HIGH);
}

void ADE7753::writeRegister16(uint8_t reg, uint16_t value) {
	uint8_t dataToSend = reg | WRITE;
  
	digitalWrite(PIN_CS, LOW);
	SPI.transfer(dataToSend); 
	delayMicroseconds(1);
	SPI.transfer(highByte(value));  
	SPI.transfer(lowByte(value));    
	digitalWrite(PIN_CS, HIGH);
}











