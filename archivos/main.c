/*
El repositorio de este proyecto se encuentra en:
https://github.com/LuisAntonio1929/MuestreoPic16f1719.git
*/

#include "source.h"
#include "funciones.h"

#define ADC_FLAG 0b01000000
#define TMR2ON 0b00000100

int16 lecturas[]={0,0,0,0,0,0,0,0,0,0};

int cont = 0;				//Contador de interrupciones

int main(){
   configuracion();
   while(1){
      if(PIR1 & ADC_FLAG){
	 //Reseteo del timer y contador
	 cont = 0;
	 TMR2 = 0;
	 //
	 T2CON |= TMR2ON;		//TMR2 activado
	 RD0 = 1;			//Encender RD0
	 PIR1 &= ~ADC_FLAG;		//Limpiar flag	
	 //Obtención del valor de la conversión digital
	 int16 x = ADRESH;
	 x = x<<8;
	 x = x + ADRESL;
	 //Catualizacion de la tabla
	 DesplazarTabla(lecturas, x, 10);
	 //Calculo de la media movil
	 x = MedianaMovil(lecturas, 10);
	 T2CON &= ~TMR2ON;		//TMR2 desactivado
	 RD0 = 0;			//Apagar RD0
	 /*
	 Cálculo del tiempo total transcurrido usando TMR2:
	 Fosc = 8 MHz -> ciclo de instrucción = 1 / (Fosc / 4) = 0.5 µs
	 Prescaler = 1:4 -> cada incremento de TMR2 tarda 0.5 µs * 4 = 2 µs
	 PR2 = 255 -> TMR2 genera interrupción cada 256 ticks
	 Postscaler = 1:1 -> cada desbordamiento genera una interrupción
	 Por lo tanto, el tiempo total es:
	 (cont * 256 + TMR2) * 2 µs
	 Donde:
	 - cont = número de interrupciones acumuladas
	 - TMR2 = valor actual del temporizador
	 */
	 int16 t = (TMR2+cont*256)*2;
	 //Obtención de los centivoltios
	 x = (x>>1)-(x>>6)+(x>>8);//Equivalente a la operacion x*500/1023
	 //Imprimir en la primera fila de la pantalla de la LCD
	 char u = x/100 + '0';
	 int d = x%100;   
	 lcd_gotoxy(1,1);
	 lcd_putc("VProm: ");
	 lcd_putc(u);
	 printf(lcd_putc, ".%02d V", d);
	 //Imprimir en la segunda fila de la pantalla de la LCD
	 u = t/100 + '0';
	 d = t%100;   
	 lcd_gotoxy(1,2);
	 lcd_putc("TCalc: ");
	 lcd_putc(u);
	 printf(lcd_putc, "%02d us", d);
      }
   }
   return 0;
}
