#ifndef FUNCIONES_H_
#define FUNCIONES_H_

void configuracion();
void DesplazarTabla(int16 *v, int16 val, int n);
int16 MedianaMovil(int16 *v, int n);
void Int_TMR2();
//***********************************************
void inicializar_SPI();
void escribir_SPI(char dato);
char enviar_y_recoger_spi(char dato);

//Prototipos de funciones utilizadas para gestión del expansor MCP23S17
void init_MCP23S17();
void reset_MCP23S17();
void escribir_MCP23S17(char direccion, char dato);
void leer_MCP23S17(char direccion, char dato);

//Prototipos de funciones para manejo del LCD
void lcd_init(void);   //Inicialización del LCD
void lcd_send_byte(char address, char b);
void lcd_gotoxy( char x, char y);
void lcd_putc( char c);
void lcd_clr_line(char fila);
void lcd_set_cgram(char cgram_p);
#endif

