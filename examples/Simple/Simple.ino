#include <ArduX1.h>
#include "U8glib.h"
#include "logo_ray.h"

#define HMI   // Comenta esta linea si no tienes pantalla OLED...

#ifdef HMI
// Driver para OLED, azul = SH1106, blanco = SSD1306
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST); // Fast I2C / TWI 
//U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST); // Dev 0, Fast I2C / TWI
#endif

#define COSTE_KWH         0.131 // Precio del KWh para calcular el consumo total en €

// Crea instancia de ArduX1...
ArduX1  x1;

void setup(){ 
  // configura puerto serie
  Serial.begin(9600);


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
} 

void loop(){ 
  static unsigned long timer_send_data = 0;
 
#ifdef HMI
    gestiona_oled();
#endif  

  // Enviar datos por el puerto serie cada 3 segundos...
  if((millis() - timer_send_data) >= 3000){
    timer_send_data = millis();  
  
    Serial.print("Freq(Hz)        : ");
    Serial.println(x1.freqRead(),2);
  
    Serial.print("Vrms(V)         : ");
    Serial.println(x1.vrmsRead(),1);
  
    Serial.print("Irms(A)         : ");
    Serial.println(x1.irmsRead(),2);
  
    Serial.print("Vpeak(V)        : ");
    Serial.println(x1.vPeakRead(),1);
  
    Serial.print("Ipeak(A)        : ");
    Serial.println(x1.iPeakRead(),2);
  
    Serial.print("Pot. activa(W)    : ");
    Serial.println(x1.pActiveRead(),0);
  
    Serial.print("Pot. reactiva(VAR): ");
    Serial.println(x1.pReactiveRead(),0);
  
    Serial.print("Pot. aparente(VA) : ");
    Serial.println(x1.pApparentRead(),0);
  
    Serial.print("Factor potencia(%): ");
    Serial.println(x1.pfRead(),2);
  
    Serial.print("E. activa(KWh)    : ");
    Serial.println(x1.eActiveRead(),3);
  
    Serial.print("E. reactiva(KVARh): ");
    Serial.println(x1.eReactiveRead(),3);
  
    Serial.print("E. aparente(KVAh) : ");
    Serial.println(x1.eApparentRead(),3);
  
    Serial.print("AIN(V)    : ");
    Serial.println((x1.analogRead()*10.0)/1023.0,3);
  
    Serial.print("DIPSWITCH : ");
    Serial.println(x1.dipswitchRead());
  
    Serial.println();
  }
} 

#ifdef HMI
void gestiona_oled(void){
  static unsigned long timer_oled = 0;
  int h, w, temp, n;
  char sTemp[20];
  char sNum[10];
  static byte menu = 0;

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

  // Cada 0.5s actualiza la pantalla...
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
    }  // switch
  } // if
}

#endif //HMI

