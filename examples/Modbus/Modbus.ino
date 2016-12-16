#include <ArduX1.h>
#include <ModbusSlave.h>
#include <avr/wdt.h> 
#include "U8glib.h"
#include "logo_ray.h"

#define HMI
#ifdef HMI
// Driver para OLED, azul = SH1106, blanco = SSD1306
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST); // Fast I2C / TWI 
//U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST); // Dev 0, Fast I2C / TWI
#endif

#define COSTE_KWH         0.131

/**************************************************************************
  MAPA MODBUS
  -----------
  
  MODO R: FUNCION 3 - READ BLOCK HOLDING REGISTERS
  MODO W: FUNCION 6 - WRITE SINGLE HOLDING REGISTER   

  DIRECCION   TIPO    MODO  FORMATO    MAXIMO      MINIMO    UNIDADES    DESCRIPCION
  -----------------------------------------------------------------------------------------------
  0x0000      uint    R     0000.0     0400.0      0000.0    Hz          FRECUENCIA DE RED
  0x0001      uint    R     0000.0     0999.9      0000.0    V           VOLTAJE RMS
  0x0002      uint    R     000.00     500.00      000.00    A           CORRIENTE RMS   
  0x0003      uint    R     0000.0     0999.9      0000.0    V           MAXIMO VOLTAJE RMS
  0x0004      uint    R     000.00     500.00      000.00    A           MAXIMO CORRIENTE RMS
  0x0005      int     R     00000      32000       -32000    W           POTENCIA ACTIVA
  0x0006      int     R     00000      32000       -32000    VAR         POTENCIA REACTIVA
  0x0007      int     R     00000      32000       -32000    VA          POTENCIA APARENTE
  0x0008      int     R     00000      00100       00000     %           FACTOR DE POTENCIA
  0x0009      float   R     00000      65535       00000     Wh          ENERGIA ACTIVA (L)
  0x000A      float   R     00000      65535       00000     Wh          ENERGIA ACTIVA (H)
  0x000B      float   R     00000      65535       00000     VARh        ENERGIA REACTIVA (L)
  0x000C      float   R     00000      65535       00000     VARh        ENERGIA REACTIVA (H)
  0x000D      float   R     00000      65535       00000     VAh         ENERGIA APARENTE (L)
  0x000E      float   R     00000      65535       00000     Vah         ENERGIA APARENTE (H)
  0x000F      int     R/W   00000      12345       00000     ---         RESET REGISTRO ENERGIAS (escribir 12345 para borrar)
  0x0010      int     R/W   00000      12345       00000     ---         RESET MAXIMO VOLTAJE RMS (escribir 12345 para borrar)
  0x0011      int     R/W   00000      12345       00000     ---         RESET MAXIMO CORRIENTE RMS (escribir 12345 para borrar)
  0x0012      uint    R/W   00000      00001       00000     ---         RELE
  0x0013      uint    R     00000      01023       00000     ---         ENTRADA ANALOGICA
  0x0014      uint    R     00000      00063       00000     ---         DIPSWITCH

**************************************************************************/

// Mapa de registros modbus
enum {        
        MB_FREQ,         // Lectura frecuencia de red
        MB_VRMS,         // Lectura voltaje de linea
        MB_IRMS,         // Corriente de linea
        MB_VPEAK,        // Lectura pico de voltaje
        MB_IPEAK,        // Lectura pico de corriente
        MB_P_ACT,        // Potencia activa
        MB_P_REA,        // Potencia reactiva
        MB_P_APA,        // Potencia aparente
        MB_PF,           // Factor de potencia
        MB_E_ACT_L,      // Lectura energia activa (parte baja float)
        MB_E_ACT_H,      // Lectura energia activa (parte alta float)
        MB_E_REA_L,      // Lectura energia reactiva (parte baja float)
        MB_E_REA_H,      // Lectura energia reactiva (parte alta float)
        MB_E_APA_L,      // Lectura energia aparente (parte baja float)
        MB_E_APA_H,      // Lectura energia aparente (parte alta float) 
        MB_RESET_E,      // Reset registro energías (escribir 12345)
        MB_RESET_VPEAK,  // Reset registro pico voltaje (escribir 12345)        
        MB_RESET_IPEAK,  // Reset registro pico corriente (escribir 12345)                
        MB_RELE,         // Lectura/escritura del rele
        MB_AIN,          // Lectura entrada analogica
        MB_DIPSW,        // Lectura dipswitch
        MB_REGS	 	 // Numero total de registros
};
int regs[MB_REGS];	

// Crea instancia de ArduX1...
ArduX1  x1;

// Crea instancia de modbus...
ModbusSlave modbus;

void setup(){ 
  wdt_disable();
 
  // configura modbus...
  modbus.config(9600,'n');
  modbus.direccion = x1.dipswitchRead();

#ifdef HMI
  // Configura OLED...
  u8g.begin();
  u8g.setFont(u8g_font_9x15Br); 
  u8g.setFontRefHeightText();
  u8g.setFontPosTop();    

  // Visualiza logo en pantalla OLED...
  u8g.firstPage();  
  do {
    u8g.drawBitmapP( 0, 0, 16, 20, logo_ray);   
    u8g.drawBox(0, 28, 128, 16); 
    u8g.setDefaultBackgroundColor(); 
    u8g.drawStr(37,30, "ArduX1");   
    u8g.setDefaultForegroundColor();      
    u8g.drawStr(19,50, "Ray - 2016");   
       
  } while( u8g.nextPage() );  

  delay(4000); 
#endif 

  x1.buzzerWrite(ON);
  delay(100);
  x1.buzzerWrite(OFF);

  // Activa WDT cada 4 segundos...   
  wdt_enable(WDTO_4S); 
} 

void loop(){ 
  int *ptInt;  
  long lTemp;
 
#ifdef HMI
  gestiona_oled();
#endif  

  // Asigna valores a la tabla modbus...
  regs[MB_VRMS]   = int(x1.vrmsRead() * 10);
  regs[MB_IRMS]   = int(x1.irmsRead() * 100);  
  regs[MB_VPEAK]  = int(x1.vPeakRead() * 10);
  regs[MB_IPEAK]  = int(x1.iPeakRead() * 100);    
  regs[MB_FREQ]   = int(x1.freqRead() * 10);    
  regs[MB_P_ACT]  = int(x1.pActiveRead());      
  regs[MB_P_REA]  = int(x1.pReactiveRead());        
  regs[MB_P_APA]  = int(x1.pApparentRead()); 
  regs[MB_PF]     = int(x1.pfRead() * 100); 
  regs[MB_AIN]    = int(x1.analogRead());
  regs[MB_DIPSW]  = int(x1.dipswitchRead());

  lTemp = (x1.eActiveRead() * 1000);
  ptInt = (int *)&lTemp;
  regs[MB_E_ACT_L] = *ptInt;          
  ptInt++;
  regs[MB_E_ACT_H] = *ptInt;        

  lTemp = (x1.eReactiveRead() * 1000);
  ptInt = (int *)&lTemp;
  regs[MB_E_REA_L] = *ptInt;          
  ptInt++;
  regs[MB_E_REA_H] = *ptInt;    

  lTemp = (x1.eApparentRead() * 1000);
  ptInt = (int *)&lTemp;
  regs[MB_E_APA_L] = *ptInt;          
  ptInt++;
  regs[MB_E_APA_H] = *ptInt;   
  
  // Reset registro de energía...
  if(regs[MB_RESET_E] == 12345){
    x1.eClear();
    regs[MB_RESET_E] = 0;
  }
    
  // Reset registro de maximos...
  if(regs[MB_RESET_VPEAK] == 12345){
    x1.vPeakReset();
    regs[MB_RESET_VPEAK] = 0;
  }
  if(regs[MB_RESET_IPEAK] == 12345){
    x1.iPeakReset();
    regs[MB_RESET_IPEAK] = 0;
  }
    
  // Controla el rele...
  if(regs[MB_RELE] > 0)
    x1.relayWrite(ON);
  else
    x1.relayWrite(OFF);
   

  delay_modbus(100);
  wdt_reset();
} 


// Rutina de espera que atiende la tarea modbus...
void delay_modbus(int t){
  int n,tt;
  tt = t/10;
  
  for(n=0;n<=tt;n++){
    modbus.actualiza(regs,MB_REGS);
    delay(10);
  }  
}


#ifdef HMI
void gestiona_oled(void){
  int h, w, temp, n;
  char sTemp[20];
  char sNum[10];
  static byte menu = 0;
  static unsigned long timer_oled = 0;

  // Detecta cuando se pulsa el boton MODE...
  if(x1.digitalRead(BUTTON)){
    n = 0;
    while(x1.digitalRead(BUTTON)){
      n++;
      delay(100);
      if(n > 20){
         if(menu == 1){
            x1.vPeakReset();
            x1.iPeakReset(); 
         }
         if(menu == 0){
            x1.eClear();
         }
         x1.buzzerWrite(ON);
         delay(1000);
         x1.buzzerWrite(OFF);
         return;
      }
    }
    
    x1.buzzerWrite(ON);
    menu++;
    if(menu >= 3) menu = 0;
    delay(100);
    x1.buzzerWrite(OFF);
  }

  // Cada 0.5s actualiza la pantalla..
  if((millis() - timer_oled) >= 500){
    timer_oled = millis();
    switch(menu){
      case 1:
      u8g.firstPage();
      do{
        u8g.setFont(u8g_font_8x13r);  
        u8g.setFontRefHeightText();
        u8g.setFontPosTop();
        h = u8g.getFontAscent()-u8g.getFontDescent();
        w = u8g.getWidth();
      
        // Visualiza valores...
        dtostrf(x1.vrmsRead(), 5, 1, sNum);      
        sprintf(sTemp, "Vrms:   %s V",sNum);          
        u8g.drawStr(1, 0*h-1 , sTemp);
    
        dtostrf(x1.vPeakRead(), 5, 1, sNum);      
        sprintf(sTemp, "Vpeak:  %s V",sNum);          
        u8g.drawStr(1, 1*h-1 , sTemp);
        
        dtostrf(x1.irmsRead(), 5, 2, sNum);      
        sprintf(sTemp, "Irms:   %s A",sNum);          
        u8g.drawStr(1, 2*h-1 , sTemp);
    
        dtostrf(x1.iPeakRead(), 5, 2, sNum);      
        sprintf(sTemp, "Ipeak:  %s A",sNum);          
        u8g.drawStr(1, 3*h-1 , sTemp);
        
        dtostrf(x1.freqRead(), 5, 1, sNum);      
        sprintf(sTemp, "Freq:   %s Hz",sNum);              
        u8g.drawStr(1, 4*h-1 , sTemp);
      }while(u8g.nextPage());
      break;
  
      case 2:
      u8g.firstPage();
      do{
        u8g.setFont(u8g_font_8x13r);  
        u8g.setFontRefHeightText();
        u8g.setFontPosTop();
        h = u8g.getFontAscent()-u8g.getFontDescent();
        w = u8g.getWidth();
      
        // Visualiza valores...      
        dtostrf(x1.pActiveRead(), 5, 0, sNum);      
        sprintf(sTemp, "Pact:  %s W",sNum);          
        u8g.drawStr(1, 0*h-1 , sTemp);
        
        dtostrf(x1.pReactiveRead(), 5, 0, sNum);      
        sprintf(sTemp, "Prea:  %s VAR",sNum);          
        u8g.drawStr(1, 1*h-1 , sTemp);
    
        dtostrf(x1.pApparentRead(), 5, 0, sNum);      
        sprintf(sTemp, "Papa:  %s VA",sNum);          
        u8g.drawStr(1, 2*h-1 , sTemp);
    
        dtostrf(x1.pfRead(), 5, 2, sNum);      
        sprintf(sTemp, "PF:    %s %%",sNum);          
        u8g.drawStr(1, 3*h-1 , sTemp);
  
      }while(u8g.nextPage());
      break;
  
      case 0:
      u8g.firstPage();
      do{
        u8g.setFont(u8g_font_courB24r);  
        u8g.setFontRefHeightText();
        u8g.setFontPosTop();
        h = u8g.getFontAscent()-u8g.getFontDescent();
        w = u8g.getWidth();
  
        dtostrf(x1.pActiveRead(), 4, 0, sNum);      
        sprintf(sTemp, "%s W",sNum);          
        u8g.drawStr(1, 0*h , sTemp);   
      
        u8g.setFont(u8g_font_8x13r);  
        u8g.setFontRefHeightText();
        u8g.setFontPosTop();
        h = u8g.getFontAscent()-u8g.getFontDescent();
        w = u8g.getWidth();
  
        dtostrf(x1.eActiveRead(), 7, 1, sNum);      
        sprintf(sTemp, "Ea: %s KWh",sNum);          
        u8g.drawStr(1, 3*h-1 , sTemp);
        
        dtostrf(x1.eReactiveRead(), 7, 1, sNum);      
        sprintf(sTemp, "Er: %s KVARh",sNum);          
        u8g.drawStr(1, 4*h-1 , sTemp);   
  
        dtostrf((x1.eActiveRead() * COSTE_KWH), 7, 1, sNum);      
        sprintf(sTemp, "Co: %s Euro",sNum);          
        u8g.drawStr(1, 5*h-1 , sTemp);  
        
      }while(u8g.nextPage());
      break;
    } // switch
  } // if
}

#endif //HMI

