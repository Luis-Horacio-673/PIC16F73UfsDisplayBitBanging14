/*
 * File:   newmain14.c
 * Author: Luis Soto
 *
 * Created on 12 de septiembre de 2024, 15:43
 */
#include <xc.h>
#include <string.h>
#pragma config FOSC = HS // Oscilador de alta velocidad
#pragma config WDTE = OFF       // Watchdog Timer desactivado
#pragma config PWRTE = ON       // Power-up Timer activado
#pragma config BOREN = ON       // Brown-out Reset activado
//#pragma config LVP = OFF        // Programaci?n en baja tensi?n desactivada
#pragma config CP = OFF        // Protecci?n de datos de memoria desactivada
//#pragma config WRT = OFF        // Protecci?n de escritura desactivada
#define _XTAL_FREQ 3686400 // Define la frecuencia del oscilador

#define TX_PIN PORTCbits.RC6  // Definir el pin de transmisión
#define RX_PIN PORTCbits.RC7   // Definir el pin de recepción

#define MAX_DIN  RC0 // RB2
#define MAX_LAT   RC1 // RB0
#define MAX_CLK  RC2  // RB1
#define MAX_DIN2  RA0  //RB5
#define MAX_LAT2   RA1 // RB3
#define MAX_CLK2  RA2  //RB4

#define STX 0x02 /* 0000 0010 Start of text */
#define ETX 0x03 /* 0000 0011 End of text */
#define ACK 0x06 /* 0000 0110 Positive acknowledgement */
#define NAK 0x15 /* 0001 0101 Negative acknowledgement */

//***************************************************************************
// Definicion de Registros del MAX7219
//***************************************************************************
#define NOPER 0x00
#define DIG1  0x01 // Digito 1
#define DIG2  0x02 // Digito 2
#define DIG3  0x03 // Digito 3
#define DIG4  0x04 // Digito 4
#define DIG5  0x05 // Digito 5
#define DIG6  0x06 // Digito 6
#define DIG7  0x07 // Digito 7
#define DIG8  0x08 // Digito 8
#define MODO  0x09 // Modo de de decodificaci?n
  #define ON_DECO  0xFF // Comn decodificador
  #define OFF_DECO  0x00 // Sin decodificar 
#define INTENSI  0x0A // Intensidad El segundo va de 00 a 0F maxima corriente
#define LIM  0x0B // Límite de escaneo Data REsiter va de 00 a 07
#define SD  0x0C // Shutdown DATA o apagadp 1 Normal
#define TEST  0x0F // Test dsiplayu DATA 0 Normal    modo Test = 1

// Parámetros de bit-banging
#define BIT_DELAY 48  // Número de ciclos de instrucción para 19200 baudios

//***************************************************************************
// Definicion de Registros del MAX7219
//***************************************************************************
#define NOPER 0x00
#define DIG1  0x01 // Digito 1
#define DIG2  0x02 // Digito 2
#define DIG3  0x03 // Digito 3
#define DIG4  0x04 // Digito 4
#define DIG5  0x05 // Digito 5
#define DIG6  0x06 // Digito 6
#define DIG7  0x07 // Digito 7
#define DIG8  0x08 // Digito 8
#define MODO  0x09 // Modo de de decodificaci?n
  #define ON_DECO  0xFF // Comn decodificador
  #define OFF_DECO  0x00 // Sin decodificar 
#define INTENSI  0x0A // Intensidad El segundo va de 00 a 0F maxima corriente
#define LIM  0x0B // Límite de escaneo Data REsiter va de 00 a 07
#define SD  0x0C // Shutdown DATA o apagadp 1 Normal
#define TEST  0x0F // Test dsiplayu DATA 0 Normal    modo Test = 1
#define MESSAGE_LENGTH 30
//******************************************************************************
// Definicion de variables
//******************************************************************************
unsigned char bufferin[30]; // para definir la recpcion de datos
unsigned char dMil = 0x30;
unsigned char byteValue = 0;
signed char nbyte = 0;
unsigned char nSTX = 0;
unsigned char zerosleft = 0;
unsigned char BCC = 0;
unsigned char i = 0;
unsigned char cMillon = 0x30;
unsigned char sMillosold = 0x30;
unsigned char sMillosnew = 0x30;
unsigned char sMilold = 0x30;
unsigned char sMilnew = 0x30;
unsigned char flagCien = 0x30;
//***************************************************************************
void MAX_DIN_setHigh() {
    MAX_DIN = 1; // Pone el pin B2 en alto
}
//***************************************************************************
void MAX_DIN2_setHigh() {
    MAX_DIN2 = 1; // Pone el pin B2 en alto
}
//******************************************************************************
void MAX_DIN_setLow(){
    MAX_DIN = 0;
}
//******************************************************************************
void MAX_DIN2_setLow(){
    MAX_DIN2 = 0;
}
//******************************************************************************
void MAX_CLK_setHigh(){
    MAX_CLK = 1;
}
//******************************************************************************
void MAX_CLK2_setHigh(){
    MAX_CLK2 = 1;
}
//******************************************************************************
void MAX_CLK_setLow(){
    MAX_CLK = 0;
}
//******************************************************************************
void MAX_CLK2_setLow(){
    MAX_CLK2 = 0;
}
//******************************************************************************
void MAX_LAT_setLow(){
    MAX_LAT=0;
}
//******************************************************************************
void MAX_LAT2_setLow(){
    MAX_LAT2 = 0;
}
//******************************************************************************
void MAX_LAT_setHigh(){
    MAX_LAT = 1;
}
//******************************************************************************
void MAX_LAT2_setHigh(){
    MAX_LAT2 = 1;
}
//******************************************************************************
// Sends a byte data to register AMOUNT
//******************************************************************************
void MAX7219_send(uint8_t reg, uint8_t dat)
    {
    uint16_t d=(reg<<8)|dat;
    for (uint8_t i=0; i<16; i++)
        {
          if (d & (0x8000>>i))
            {MAX_DIN_setHigh();}
        else
            {MAX_DIN_setLow();}
        MAX_CLK_setHigh();
        MAX_CLK_setLow();
        }
    MAX_LAT_setHigh();
    MAX_LAT_setLow();    
    }
//******************************************************************************
// Sends a byte data to register COUNT
//******************************************************************************
void COMAX7219_send(uint8_t reg, uint8_t dat)
    {
    uint16_t d=(reg<<8)|dat;
     for (uint8_t i=0; i<16; i++)
        {
        if (d & (0x8000>>i))
            {MAX_DIN2_setHigh();}
        else
            {MAX_DIN2_setLow();}
        MAX_CLK2_setHigh();
        MAX_CLK2_setLow();
        }
    MAX_LAT2_setHigh();
    MAX_LAT2_setLow();    
    }
//******************************************************************************
// Inicializacion de MAXs
//*****************************************************************************
void initMAXs(){
   MAX7219_send(INTENSI, 0x0F); 
   COMAX7219_send(INTENSI, 0X0F);
   MAX7219_send(MODO, ON_DECO);
   COMAX7219_send(MODO, ON_DECO);
   MAX7219_send(LIM, 0X07);
   COMAX7219_send(LIM, 0X07);
   MAX7219_send(SD, 0X01);
   COMAX7219_send(SD, 0X01);
   //MAX7219_send(TEST, 0X00);
   //COMAX7219_send(TEST, 0X00);
}
//******************************************************************************

// Inicializar el temporizador para controlar la velocidad de bit
void setup_timer() {
    TMR0 = 0;
    OPTION_REGbits.T0CS = 0;  // Usar el reloj interno
    OPTION_REGbits.PSA = 0;   // Asignar preescalador a TMR0
    OPTION_REGbits.PS = 0b010;  // Preescalador de 8 (ajustar según sea necesario)
    INTCONbits.TMR0IE = 0;    // Habilitar interrupción del TMR0
    INTCONbits.TMR0IF = 0;    // Limpiar flag de interrupción
    INTCONbits.GIE = 1;       // Habilitar interrupciones globales
    INTCONbits.PEIE = 1;      // Habilitar interrupciones periféricas
}

// Transmisión de un byte con bit de paridad
void send_byte(unsigned char data) {
    unsigned char i, parity = 0;

    // Start bit (inversión: RS232 necesita 1 -> enviamos 0)
    TX_PIN = 1;  // Start bit invertido
    __delay_us(BIT_DELAY);

    // Enviar 8 bits de datos (inversión)
    for (i = 0; i < 8; i++) {
        TX_PIN = !((data >> i) & 0x01);  // Invertir cada bit de datos
        parity ^= !((data >> i) & 0x01);  // Calcular paridad con la señal invertida
        __delay_us(BIT_DELAY);
    }

    // Enviar bit de paridad (inversión de paridad)
    TX_PIN = !parity;  // Enviar bit de paridad invertido
    __delay_us(BIT_DELAY);

    // Stop bit (inversión: RS232 necesita 0 -> enviamos 1)
    TX_PIN = 0;  // Stop bit invertido
    __delay_us(BIT_DELAY);
}



unsigned char receive_byte() {
    unsigned char i, data = 0, parity = 0;

    // Esperar al start bit (inversión: RS232 necesita 1 -> recibimos 0)
    while (RX_PIN == 0);  // Esperar hasta que la línea esté alta

    __delay_us(BIT_DELAY / 2);  // Sincronizar con el centro del bit

    // Leer los 8 bits de datos (inversión)
    for (i = 0; i < 8; i++) {
        __delay_us(BIT_DELAY);
        data |= (!RX_PIN << i);  // Invertir la señal leída
        parity ^= !RX_PIN;       // Calcular paridad con la señal invertida
    }

    // Leer el bit de paridad y verificarlo
    __delay_us(BIT_DELAY);
    if (parity != !RX_PIN) {
        // Error de paridad
    }

    // Leer el stop bit (inversión: RS232 necesita 0 -> recibimos 1)
    __delay_us(BIT_DELAY);
    if (RX_PIN == 0) {
        // Error de stop bit
    }

    return data;
}


// Rutina de interrupción (para iniciar transmisión/recepción)
void __interrupt() isr() {
    if (INTCONbits.TMR0IF) {
        // Aquí podrías manejar el temporizador o iniciar la transmisión
        INTCONbits.TMR0IF = 0;  // Limpiar flag de interrupción
    }
}

void main() {
     TRISB = 0xC0;
     TRISC = 0X80;
     TRISA = 0XF8;
    TX_PIN = 0;  // Mantener TX en alto (estado inactivo)
    setup_timer();  // Inicializar temporizador
    initMAXs();
    // Inicializacion de displauy para luego ser borrada
    bufferin[0]= 0x02;
    bufferin[1]= 0x34;
    bufferin[2]= 0x36;
    bufferin[3]= 0x31;
    bufferin[4]= 0x31;
    bufferin[5]= 0x30;
    bufferin[6]= 0x30;
    bufferin[7]= 0x30; //AMOUNT 1
    bufferin[8]= 0x30; //AMOUNT 2
    bufferin[9]= 0x30; //AMOUNT 3
    bufferin[10]= 0x30; //AMOUNT 4
    bufferin[11]= 0x30; //AMOUNT 5
    bufferin[12]= 0x30; //AMOUNT 6    CM
    bufferin[13]= 0x30; //AMOUNT 7    DM
    bufferin[14]= 0x30; //AMOUNT 8    UM
    bufferin[15]= 0x30; //AMOUNT 9    CMIL
    bufferin[16]= 0x31; //AMOUNT 10   DMIL
    bufferin[17]= 0x30; //AMOUNT 11   UMIL
    bufferin[18]= 0x30; //AMOUNT 12   CEN
    bufferin[19]= 0x30; //AMOUNT 13   DEC
    bufferin[20]= 0x30; //AMOUNT 14   UNI
    bufferin[21]= 0x30; //COUNT 1
    bufferin[22]= 0x30; //COUNT 2
    dMil= 0x30; //COUNT 3 -
    bufferin[23]= 0x30; 
    bufferin[24]= 0x30; //COUNT 4 H
    bufferin[25]= 0x30; //COUNT 5 E
    bufferin[26]= 0x30; //COUNT 6 L 
    bufferin[27]= 0x31; //COUNT 7 P
    bufferin[28]= 0x03; //COUNT 7 P
    bufferin[29]= 0x31; //COUNT 7 P   s
    

    // Ejemplo de uso
    while (1) {
        if(!zerosleft)
        {
            if(bufferin[12]==0x30){bufferin[12]= 0x0F + 0x30;}else{zerosleft=1;}
        }
        MAX7219_send(DIG8, bufferin[12] -48);
        if(!zerosleft)
        {
            if(bufferin[13]==0x30){bufferin[13]= 0x0F + 0x30;}else{zerosleft=1;}
        }
        MAX7219_send(DIG7, bufferin[13] -48);
        if(!zerosleft)
        {
            if(bufferin[14]==0x30){bufferin[14]= 0x0F + 0x30;}else{zerosleft=1;}
        }
        MAX7219_send(DIG6, bufferin[14] -48);
        if(!zerosleft)
        {
            if(bufferin[15]==0x30){bufferin[15]= 0x0F + 0x30;}else{zerosleft=1;}
        }
        MAX7219_send(DIG5, bufferin[15] -48); //COMAX7219_send(DIG5, bufferin[12] -48);
        if(!zerosleft)
        {
            if(bufferin[16]==0x30){bufferin[16]= 0x0F + 0x30;}else{zerosleft=1;}
        }
        MAX7219_send(DIG4, bufferin[16] -48); // MAX7219_send(DIG8, bufferin[13] -48);
        if(!zerosleft)
        {
            if(bufferin[17]==0x30){bufferin[17]= 0x0F + 0x30;}else{zerosleft=1;}
        }
        MAX7219_send(DIG3, bufferin[17] -48);//MAX7219_send(DIG7, bufferin[14] -48);
        if(!zerosleft)
        {
            if(bufferin[18]==0x30){bufferin[18]= 0x0F + 0x30;}else{zerosleft=1;}
        }
        MAX7219_send(DIG2, bufferin[18] -48); //MAX7219_send(DIG6, bufferin[15] -48)
        if(!zerosleft)
        {
            if(bufferin[19]==0x30){bufferin[19]= 0x0F + 0x30;}else{zerosleft=1;}
        }
        MAX7219_send(DIG1, bufferin[19] -48);// MAX7219_send(DIG5, bufferin[16] -48);
        
        COMAX7219_send(DIG8, 0x0F);// 0x0F   MAX7219_send(DIG4, bufferin[17] -48);
        COMAX7219_send(DIG7, 0x0F); //MAX7219_send(DIG3, bufferin[18] -48);
        COMAX7219_send(DIG6, cMillon -48); //MAX7219_send(DIG2, bufferin[19] -48);
        zerosleft=0;
        if(!zerosleft) 
        {
            if(dMil==0x30){bufferin[23]= 0x3F;}else{zerosleft=1;}
        }
        COMAX7219_send(DIG1, bufferin[23] -48);
        if(!zerosleft)
        {
            if(bufferin[24]==0x30){bufferin[24]= 0x3F;}else{zerosleft=1;}
        }
        COMAX7219_send(DIG2, bufferin[24] -48);
        if(!zerosleft)
        {
            if(bufferin[25]==0x30){bufferin[25]= 0x3F;}else{zerosleft=1;}
        }
        COMAX7219_send(DIG3, bufferin[25] -48);
        if(!zerosleft)
        {
            if(bufferin[26]==0x30){bufferin[26]= 0x3F;}else{zerosleft=1;}
        }
        COMAX7219_send(DIG4,  bufferin[26] -48);
        COMAX7219_send(DIG5, bufferin[27] -48);
        // Recepción o transmisión continua
        if(nbyte==0){
        while(byteValue !=0x02){
          byteValue=receive_byte();  
        }
        /* if(byteValue = 0x02) // STX
         {
           nSTX++;
           if(nSTX == 0x0A){nSTX = 0;}
           bufferin[0]= byteValue;
         }else{nbyte=-1;}*/
        }
        
          byteValue = receive_byte();
          bufferin[1] = byteValue;  // Trial 1
        
          byteValue = receive_byte();
          bufferin[2] = byteValue;  // Trial 2
        
          byteValue = receive_byte();
          bufferin[3] = byteValue;  // STATUS 1
 
          byteValue = receive_byte();
          bufferin[4] = byteValue;  // STATUS 2

          byteValue = receive_byte();
          bufferin[5] = byteValue;  // STATUS 3

          byteValue = receive_byte();
          bufferin[6] = byteValue;  // STATUS 4

          byteValue = receive_byte();
          bufferin[7] = byteValue;  // AMOUNT 1

          byteValue = receive_byte();
          bufferin[8] = byteValue;  // AMOUNT 2
          
          byteValue = receive_byte();
          bufferin[9] = byteValue;  // AMOUNT 3
          
          byteValue = receive_byte();
          bufferin[10] = byteValue;  // AMOUNT 4
          
          byteValue = receive_byte();
          bufferin[11] = byteValue;  // AMOUNT 5
          
          byteValue = receive_byte();
          bufferin[12] = byteValue;  // AMOUNT 6
          
          byteValue = receive_byte();
          bufferin[13] = byteValue;  // AMOUNT 7
          sMillosnew = byteValue;
          
          byteValue = receive_byte();
          bufferin[14] = byteValue;  // AMOUNT 8
          
          byteValue = receive_byte();
          bufferin[15] = byteValue;  // AMOUNT 9
          
          byteValue = receive_byte();
          bufferin[16] = byteValue;  // AMOUNT 10
          
          byteValue = receive_byte();
          bufferin[17] = byteValue;  // AMOUNT 11
          
          byteValue = receive_byte();
          bufferin[18] = byteValue;  // AMOUNT 12
          
          byteValue = receive_byte();
          bufferin[19] = byteValue;  // AMOUNT 13
          
          byteValue = receive_byte();
          bufferin[20] = byteValue;  // AMOUNT 14
          
          byteValue = receive_byte();
          bufferin[21] = byteValue;  // COUNT 1
          
          byteValue = receive_byte();
          bufferin[22] = byteValue;  // COUNT 2
          
          byteValue = receive_byte();
          bufferin[23] = byteValue;  // COUNT 3
          
          byteValue = receive_byte();
          bufferin[24] = byteValue;  // COUNT 4
          sMilnew = byteValue;
          
          byteValue = receive_byte();
          bufferin[25] = byteValue;  // COUNT 5
          
          byteValue = receive_byte();
          bufferin[26] = byteValue;  // COUNT 6
          
          byteValue = receive_byte();
          bufferin[27] = byteValue;  // COUNT 7
          
          byteValue = receive_byte();
          bufferin[28] = byteValue;  // ETX
          
          byteValue = receive_byte();
          bufferin[29] = byteValue;  // BCC
          
          nbyte ++;
          
          if(nbyte == 30){nbyte = 0;}
          BCC=0;   // verificacion de errores horizontales
            for (i=1; i<MESSAGE_LENGTH-1; i++)
            {BCC=BCC^bufferin[i];}
            if (BCC==bufferin[29]) {
             send_byte(ACK);   
            }else{send_byte(NAK);}
         // camiar cmillos? 
          if((bufferin[3]==0x31)&& ((sMillosnew-sMillosold)<0)&&(sMillosold == 0x39)){
            cMillon++;
            flagCien = 0x31;
            if(cMillon == 0x3A){cMillon = 0x30;}
        }
        if((bufferin[3]==0x31)&&((sMilnew-sMilold)<0)&&(sMilold == 0x39)){
            dMil++;
            flagCien = 0x31;
            if(dMil == 0x3A){dMil = 0x30;}
        }
        sMillosold = sMillosnew;
        sMilold = sMilnew;
        // Rutina de borrado
        if((flagCien==0x30)&&(bufferin[13]==30)&&(bufferin[14]==30)&&(bufferin[15]==30)&&
                (bufferin[16]==30)&&(bufferin[17]==30)&&(bufferin[18]==30)&&(bufferin[19]==30)&&
                (bufferin[20]==30)&&(bufferin[24]==30)&&(bufferin[25]==30)&&
                (bufferin[26]==30)&&(bufferin[27]==30))
        {
            cMillon = 0x30;
            dMil = 0x30;
        }
        if((flagCien==0x31)&&((!bufferin[24]==30)||(!bufferin[25]==30)||
                (!bufferin[26]==30)|(!bufferin[27]==30)))
        {
            flagCien = 0x30;
        }
        //Fin Rutina de borrado
    }
}