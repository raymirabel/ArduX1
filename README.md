# ArduX1 #
## Analizador de energía eléctrica monofásico ##

----------
ArduX1 es un dispositivo que nos permite medir los principales parámetros de una línea eléctrica monofásica alterna. Su arquitectura basada en Arduino hace que este medidor sea muy versátil, pudiendo modificar el firmware fácilmente a medida del usuario.

![ArduX1](https://github.com/raymirabel/ArduX1/blob/master/Doc/ArduX1.JPG)

Lo más destacado:

- Tamaño compacto en caja carril DIN.
- Alimentación directa desde 90 hasta 260VAC.
- Medidor de precisión basado en el chip [ADE7753](https://github.com/raymirabel/ArduX1/blob/master/Doc/ADE7753.pdf).
- Error de medida < 1%.
- Capacidad de medida hasta 30A o 100A.
- Medidas de voltaje, corriente, frecuencia, potencia (activa, reactiva, aparente, factor de potencia), energías y picos de voltaje y corriente.
- Microcontrolador Atmel ATmega328P.
- Puerto RS485.
- Salida relé.
- Entrada analógica 5V/10V/20mA.
- Dipswitch 6 posiciones.
- Buzzer acústico.
- Opción HMI (pantalla OLED 1.3" + pulsador).
- Opción WIFI.
- Opción RTC.
- Opción GSM/GPRS SIM800L.


## Funciones disponibles: ##

    void relayWrite(bool);			// Escribe estado del relé
	bool relayRead(void);			// Lee estado del relé
    void relayToggle(void);			// Alterna estado del relé
    void buzzerWrite(bool);			// Escribe estado del buzzer
    bool buzzerRead(void);			// Lee estado del buzzer
    void buzzerToggle(void);		// Alterna estado del buzzer
    bool digitalRead(int8_t);		// Lee entrada AIN en modo digital
    uint8_t dipswitchRead(void);	// Lee estado del dipswitch
    uint16_t analogRead(void);		// Lee entrada AIN en modo analógico
	float vrmsRead(void);			// Lee voltaje RMS			(V)
	float irmsRead(void);			// Lee corriente RSM		(A)
	float freqRead(void);			// Lee frecuencia de red 	(Hz)
	float vPeakRead(void);			// Lee voltaje RMS de pico	(V)
	float iPeakRead(void);			// Lee corriente RMS de pico(V)
	float pActiveRead(void);		// Lee potencia activa		(W)
	float pReactiveRead(void);		// Lee potencia reactiva	(VAR)
	float pApparentRead(void);		// Lee potencia aparente	(VA)
	void vPeakReset(void);			// Reset voltaje de pico
	void iPeakReset(void);			// Reset corriente de pico	
	float pfRead(void);				// Lee factor de potencia	
	float eActiveRead(void);		// Lee energía activa		(KWh)
	float eReactiveRead(void);		// Lee energía reactiva		(KVARh)
	float eApparentRead(void);		// Lee energía aparente		(KVAh)
	void eClear(void);				// Reset energías