/*
   TC1047A - Sensor de temperatura.

   Autor: Tiago Melo
   Blog: Microcontrolandos
   Compilador: MikroC PRO PIC
   Bibliotecas: ADC, Lcd, Lcd_Constants, Conversions, C_String
*/

//Pinos do LCD.
sbit LCD_RS at RC0_bit;
sbit LCD_EN at RC1_bit;
sbit LCD_D4 at RC2_bit;
sbit LCD_D5 at RC3_bit;
sbit LCD_D6 at RC4_bit;
sbit LCD_D7 at RC5_bit;
sbit LCD_RS_Direction at TRISC0_bit;
sbit LCD_EN_Direction at TRISC1_bit;
sbit LCD_D4_Direction at TRISC2_bit;
sbit LCD_D5_Direction at TRISC3_bit;
sbit LCD_D6_Direction at TRISC4_bit;
sbit LCD_D7_Direction at TRISC5_bit;

//O PIC16F876A tem um ADC de 10 bits (2^10 = 1024)
#define RESOLUCAO_ADC 1024

int TC1047A_Read(char channel) {
    unsigned adc = ADC_Get_Sample(channel);
    return (adc * 500L) / RESOLUCAO_ADC - 50;
}

void main() {
char texto[7];

   //Define somente o pino RA0 como analogico.
   ADCON1 = 0x0E;
   //Desabilita o comparador analogico.
   CMCON = 7;
   //Inicializa o modulo ADC.
   ADC_Init();
   //Inicializa o LCD.
   Lcd_Init();
   Lcd_Cmd(_LCD_CLEAR);
   Lcd_Cmd(_LCD_CURSOR_OFF);
   
   while(1) {
       //Temperatura. Canal analogico 0.
       int valor = TC1047A_Read(0);
       //Converte em texto.
       IntToStr(valor, texto);
       //Escreve no LCD.
       Lcd_Out(1, 5, "TC1047A");
       Lcd_Out(2, 3, texto);
       Lcd_Chr_CP(223);
       Lcd_Out_CP("C");
       //Faz nada durante 1s.
       Delay_ms(1000);
   }
}