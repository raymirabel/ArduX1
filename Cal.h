//#define cal_ini

// Calibrations...
#ifndef cal_ini

#define VRMS_CAL		1.0230		// Voltage rms calibration
#define IRMS_CAL		0.9896		// Current rms calibracion
#define V_PEAK_CAL		1.0499		// Voltage peak calibracion
#define I_PEAK_CAL		1.0051		// Current peak calibracion
#define P_ACT_CAL		1.0576		// Active power calibracion
#define P_REA_CAL		0.7590		// Reactive power calibracion
#define P_APA_CAL		1.2459		// Aparent power calibracion
#define IRMS_OFFSET		-0.02		// Current rms offset calibration (A)
#define CFDEN_VALUE		720			// Pulse output frecuency divisor value (increase value = frequency decreases)

#else

#define VRMS_CAL		1.0000		// Voltage rms calibration
#define IRMS_CAL		1.0000		// Current rms calibracion
#define V_PEAK_CAL		1.0000		// Voltage peak calibracion
#define I_PEAK_CAL		1.0000		// Current peak calibracion
#define P_ACT_CAL		1.0000		// Active power calibracion
#define P_REA_CAL		1.0000		// Reactive power calibracion
#define P_APA_CAL		1.0000		// Aparent power calibracion
#define IRMS_OFFSET		0.0		    // Current rms offset calibration (A)
#define CFDEN_VALUE		720			// Pulse output frecuency divisor value (increase value = frequency decreases)

#endif


