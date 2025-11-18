#include "source.h"
#include "funciones.h"

void configuracion(){
   //Configuracion del oscilador
   OSCCON = 0b01110010;			//setup_oscillator(OSC_8MHZ);
   //Inicializacion de la LCD
   inicializar_SPI();
   delay_ms(10);
   init_MCP23S17();
   delay_ms(10);
   lcd_init();			
   delay_ms(10);
   //Configuracion de los pines
   bit_clear(TRISD,0);		//RD0 como salida
   bit_clear(PORTD,0);		//RD0 en 0
   bit_set(ANSELA,0);		//RA0 como pin analogico
   bit_set(TRISA,0);		//RA0 como entrada
   //Configuracion analogica
   ADCON0 = 0b00000001;		//Selección A0 como canal y habilitacion ADC
   ADCON1 = 0b10010000;		//Formato a la derecha, Fosc/8 (TAD=1ms), y Vref+=VDD Vref-=Vss
   ADCON2 = 0b00010000;		//ADC lanzado por CCP1
   //Configuracion CCP1
   CCP1CON = 0b00001011;
   //Para alcanzar una interrupcion cada 100ms = 4*4*(CCPR1)/(8MHz)
   //Se llega a que el valor de CCPR1 = 50000
   CCPR1L = 50000 & 0xFF;
   CCPR1H = 50000 >> 8;
   //Configuracion TMR1
   T1CON = 0b00100001;		//Fuente de reloj Fosc/4, Prescaler 1:4, TMR1 habilitado
   //Configuracion TMR2
   T2CON = 0b00000001;		//Precaler 1:4 y postcaler 1:1  
   TMR2 = 0;			//Inicializacion del TMR2 a 0
   PR2 = 255;			//Desbordamiento del TMR2 a 255
   //Configuracion Interrupciones
   INTCON = 0b11000000;		//Activacion de interrupciones globales y por perifericos
   PIE1 = 0b00000010;		//TMR2 Interrupt Enable bit
}

extern int cont;

#INT_TIMER2
void Int_TMR2(){
   //En cada interrupcion el TMR2  se resetea
   cont++;
}

void DesplazarTabla(int16 *v, int16 val, int n){
   for(int i=n-1;i>0;i--){
      v[i]=v[i-1];
   }
   v[0]=val;
}

int16 MedianaMovil(int16 *v, int n){
   int16 promedio = 0;
   for(int i=0;i<n;i++){
      promedio+=v[i];
   }
   return promedio/10;
}

//**************************************************************************

void inicializar_SPI()
{
   RC5PPS=0b00010001;   //Se define RC5 como SDO xxx10001, también podría ser con directiva: #pin_select SDO1=PIN_C5
   RC3PPS=0b00010000;   //Se define RC3 como SCK xxx10000, también podría ser con directiva: #pin_select SCK1=PIN_C3
   TRISA=TRISA & (0b11111011); //RA2 como salida (se conecta a entrada Chip Select del expansor)
   TRISB=TRISB & (0b11011111); //RB5 como salida (es entrada Reset del expansor)
   TRISC=TRISC & (0b11010111); //RC3 es MOSI y RC5 es SCK, ambas líneas de salida
   
   ANSELA= ANSELA & (0b11111011);   //RA2 como señal digital
   ANSELB= ANSELB & (0b11011111);   //RB5 señal digital
   ANSELC= ANSELC & (0b11010111);   //RC3 y RC5 señales digitales

   //Módulo SSP habilitado; CKP=0 (reloj inactivo a 0) y Modo Maestro con fclock=fosc/4=8MHz/4=2MHz
   SSP1CON1=0b00100000; 
   //Muestreo al final del bit; CKE=1 (salida en transición de activo a inactivo)
   SSP1STAT=0b11000000; 
   //Con directiva: #use spi (MASTER, SPI1, BAUD=2000000, MODE=0, BITS=8, STREAM=SPI_1)
}
///////////////////////////////////////////////////////////
//Función para enviar un dato por SPI sin recoger dato 
///////////////////////////////////////////////////////////
void escribir_SPI(char dato)
{
   SSP1IF=0;      //Se pone el flag SSP1IF a 0
   SSP1BUF=dato;   //Se carga el dato a enviar en el registro SSP1BUF
   while (!SSP1IF);   //Se espera a que se haya completado el envío
}
////////////////////////////////////////////////////////////////////
//Función para enviar un dato y recoger el proveniente del esclavo
////////////////////////////////////////////////////////////////////
char enviar_y_recoger_spi(char dato)
{
   SSP1IF=0;   //Se pone el flag SSP1IF a 0
   SSP1BUF=dato;   //Se carga el dato a enviar en el registro SSP1BUF
   while (!SSP1IF);   //Se espera a que se haya completado el envío
   return(SSP1BUF);   //Se devuelve el dato recogido en SSP1BUF proveniente del otro dispositivo
}
////////////////////////////////////////////////////////////////////
//Función para resetear el MCP23S17
////////////////////////////////////////////////////////////////////
void reset_MCP23S17()
{
   Chip_Select=1; //Sin seleccionar el integrado
   Reset=0; //Se activa la línea de Reset (nivel activo bajo)
   delay_us(100); //Se mantiene así durante 100us
   Reset=1; //Se vuelve a desactivar 
}
/////////////////////////////////////////////////////////////////////////
//Para inicializar la dirección de los pines de los puertos del MCP23S17
/////////////////////////////////////////////////////////////////////////
void init_MCP23S17()
{
   escribir_MCP23S17(IOCON,0b00001000);
   escribir_MCP23S17(IODIRA,0b00111111); //Las líneas GPA7 y GPA6 son de salida, resto de entradas
   escribir_MCP23S17(IODIRB,0x00);   //Las líneas del GPB0 a GPB7 son de salida
}
////////////////////////////////////////////////////////////////////
//Función para escribir en un registro interno de MCP23S17 
// direccion es la dirección del registro
// dato es el valor que se quiere escribir
////////////////////////////////////////////////////////////////////
void escribir_MCP23S17(int direccion, int dato)
{
   Chip_Select=0;         //Se selecciona el integrado SPI
   escribir_SPI(0x40);      //Se envía la dirección del esclavo con el bit R/W=0 (operación de escritura de registro)
   escribir_SPI(direccion);   //Se envía la dirección del registro interno del Esclavo
   escribir_SPI(dato);      //Se envía el dato que se quiere escribir en el registro
   Chip_Select=1;       //Se deja de seleccionar el esclavo
}
////////////////////////////////////////////////////////////////////
//Función para leer el contenido de un registro interno de MCP23S17 
// direccion es la dirección del registro que se quiere leer
// la función devuelve el contenido del registro
////////////////////////////////////////////////////////////////////
char leer_MCP23S17(int direccion)
{
   Chip_Select=0;      //Se selecciona el integrado SPI
   escribir_SPI(0x41);      //Se envía la dirección del esclavo con el bit R/W=1 (operación de lectura de registro)
   escribir_SPI(direccion);   //Se envía la dirección del registro interno del Esclavo
   escribir_SPI(0xFF);      //Se envía un dato cualquiera, solo para generar flancos en SCK
   Chip_Select=1;      //Se deja de seleccionar el esclavo
   return(SSP1BUF);      //Se devuelve el valor recogido en SSP1BUF
}
//******************************************************************
// Función de inicialización del LCD
//******************************************************************
void lcd_init(void)
{
delay_ms(15); //Espera de 15ms
lcd_send_byte(0,0b00110000); //0011NFxx
delay_ms(5);
lcd_send_byte(0,0b00110000); //0011NFxx
//delay_us(200);
lcd_send_byte(0,0b00110000); //0011NFxx
//delay_us(100);
//Para inicializar, el LCD, se mandan una serie de comandos
lcd_send_byte(0,0b00111100); //0011NFxx N=1 (2 líneas) F=1 (patrones de 5x10)
//delay_us(100);
lcd_send_byte(0,0b00001100); //00001DCB D=1 (Display ON), C=0 (Cursor OFF) B=0 (parpadeo curso desactivado)
//delay_us(100);
lcd_send_byte(0,0b00000001); //Limpiar display
//delay_us(100);
lcd_send_byte(0,0x06); //000001 I/D S I/D=1 (modo incremento) S=0 (no se desplaza la pantalla)
}
//******************************************************************
// Funcion para escribir en LCD un byte
//
// Puede ser escritura en registro de datos (si address==1) 
// o registro de instrucción (si address==0)
//******************************************************************
void lcd_send_byte(char address, char b)
{
   if (address!=0)   //Si se trata de escribir en registro de datos
   {
      escribir_MCP23S17(OLATA,0x80);   //Se pone E a 0 y RS a 1
      escribir_MCP23S17(OLATB,b);   //Se saca el dato por el GPIOB
      delay_us(5); //Se espera 5us antes de subir la línea E
      escribir_MCP23S17(OLATA,0xC0);   //Se pone E y RS a 1
      delay_us(5);  //Se espera 5us antes de bajar la línea E
      escribir_MCP23S17(OLATA,0x80);   //Se pone E a 0 y RS a 0
   }
   else
   {
      escribir_MCP23S17(OLATA,0x0);   //Se pone E a 0 y RS a 0
      escribir_MCP23S17(OLATB,b);   //Se saca el dato por el GPIOB
      delay_us(5);   //Se espera 5us antes de subir la línea E
      escribir_MCP23S17(OLATA,0x40);   //Se pone E a 1, manteniendo RS a 0
      delay_us(5);  //Se espera 5us antes de bajar la línea E
      escribir_MCP23S17(OLATA,0);   //Se pone E a 0 y RS a 0
   }
   delay_ms(1); //Espera para asegurar que se procesa el comando
}
/////////////////////////////////////////////////////////////////////////////////////////////
// Sitúa el contador de direcciones en la DDRAM (para lectura o escritura posterior)
// x puede ir de 1 a 40, posición dentro de una línea (16 visibles)
// y puede ser 1 (línea 1) o 2 (línea 2)
/////////////////////////////////////////////////////////////////////////////////////////////
void lcd_gotoxy( char x, char y)
{
   char posicion;

   if(y!=1)
     posicion=lcd_linea_dos;
   else
     posicion=0;
   posicion+=x-1;
   lcd_send_byte(0,0b10000000|posicion); //Las direcciones de la DDRAM empiezan por 1xxxxxxx
}
////////////////////////////////////////////////////////////////////////////////////////////
// Envía un caracter c a la DDRAM del LCD, también admite algunos caracteres de control
////////////////////////////////////////////////////////////////////////////////////////////
void lcd_putc( char c)
{
   switch (c)
   {
     case '\f'   : lcd_send_byte(0,1);    //Limpia la pantalla
                   delay_ms(2);
                   break;
     case '\n'   : lcd_gotoxy(1,2);       //Coloca puntero en 1ª posicion de la 2ª línea
                   break;
     case '\b'   : lcd_send_byte(0,0x10); //Retrocede una posición el cursor
                   break;
     case '\t'   : lcd_send_byte(0,0x14); //Avanza una posición el cursor
                   break;
     case '\r'   : lcd_send_byte(0,0x18); //Retrocede una posición la pantalla visible
                   break;
     case '\v'   : lcd_send_byte(0,0x1C); //Avanza una posición la pantalla visible
                   break;
     default     : lcd_send_byte(1,c);    //Envía caracter a DDRAM,
                   break;                 //Si es una tira, los envía todos uno a uno
   }
}
///////////////////////////////////////////////////////////////////////////////////////////
// Limpia la linea correspondiente del LCD y se situa al principio de la misma                   //
///////////////////////////////////////////////////////////////////////////////////////////                                                                            
void lcd_clr_line(char fila)
{
   int j;
   
    lcd_gotoxy(1,fila);
    for (j=0;j<40;j++) lcd_putc(' ');

    lcd_gotoxy(1,fila);
}
////////////////////////////////////////////////////////////////////////////////////
// Función que coloca el puntero de la CGRAM para definir nuevos caracteres       //
////////////////////////////////////////////////////////////////////////////////////
void lcd_set_cgram(char cgram_p)
{
   lcd_send_byte(0,0b01000000|cgram_p); //Las direcciones de la CGRAM empiezan por 01xxxxxx
}

