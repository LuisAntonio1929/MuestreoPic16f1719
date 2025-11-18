#include <16f1719.h>
#use delay(clock=8M)

// Configuraci?n de los fusibles del microcontrolador
#FUSES NOWDT        // Desactiva el Watchdog Timer
#FUSES INTRC_IO     // Usa el oscilador interno con funciones de I/O en RA6 y RA7
#FUSES NOPUT        // Desactiva el Power-Up Timer
#FUSES NOPROTECT    // No protege el c?digo contra lectura
#FUSES MCLR         // Usa el pin MCLR como entrada de reset
#FUSES NOPLLEN 
//#FUSES NOCPD        // No protege la memoria EEPROM
#FUSES NOBROWNOUT   // Desactiva el detector de ca?da de voltaje
#FUSES IESO         // Habilita el cambio entre osciladores
#FUSES FCMEN        // Habilita el monitoreo del oscilador externo
#FUSES NODEBUG      // Desactiva el modo de depuraci?n


#byte portd = 0x0F
#byte trisd = 0x8F
#bit RD0 = portd.0

#byte ansela = 0x18C
#byte trisa = 0x8C

#byte ADCON0 = 0x9D
#byte ADCON1 = 0x9E
#byte ADCON2 = 0x9F
#byte ADRESH = 0x9C
#byte ADRESL = 0x9B

#byte CCP1CON = 0x293
#byte CCPR1L = 0x291
#byte CCPR1H = 0x292

#BYTE INTCON = 0x0B              // Registro de control de interrupciones
#BYTE PIE1 = 0x91                // Registro de habilitación de interrupciones perif?ricas
#byte PIR1 = 0x11
#BYTE T1CON = 0x18               // Registro de control del temporizador 1

#byte T2CON = 0x1C 
#byte PR2 = 0x1B
#byte TMR2 = 0x1A

#BYTE OSCCON = 0x99

//**********************************************************


//Dirección de los registros del PIC16F1719 que se emplean para conexión con MCP23S17
#BYTE   PORTA=0x00C
#BYTE   TRISA=0X08C
#BYTE   ANSELA=0x18C
#BIT    Chip_Select=PORTA.2 //Línea de /CS del MCP23S17

#BYTE   PORTB=0x00D
#BYTE   TRISB=0X08D
#BYTE   ANSELB=0x18D
#BIT    Reset=PORTB.5   //Línea de /Reset del MCP23S17

#BYTE   PORTC=0x00E
#BYTE   TRISC=0x08E
#BYTE   ANSELC=0x18E
#BIT    SDO=PORTC.5   // Serial Data Output (MOSI)
#BIT    SCK=PORTC.3   // Serial Clock, Output

//Para que RC5/3 sean SDO/SCK se cargan los registros RC3PPS y RC5PPS
#BYTE   RC5PPS=0xEA5
#BYTE   RC3PPS=0xEA3
#BYTE   PIR1=0x011
//Registros configuración SPI:
#BYTE   SSP1CON1=0x215
#BYTE   SSP1STAT=0x214
#BYTE   SSP1BUF=0x211   //Registro de envío y recogida de comunicación SPI
#BIT    SSP1IF=PIR1.3

//Direcciones de los registros internos del MCP23S17 si el bit IOCON.BANK=0
#DEFINE IOCON    0x0A
#DEFINE IODIRA   0x00
#DEFINE IODIRB   0x01
#DEFINE GPPUA   0x0C
#DEFINE GPIOA   0x12
#DEFINE GPIOB   0x13
#DEFINE OLATA   0x14
#DEFINE OLATB   0x15

#define lcd_linea_dos 0x40       // Dirección de inicio para la 2ª línea en la DDRAM del LCD
