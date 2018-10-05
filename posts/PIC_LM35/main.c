/*
   LM35 - Sensor de temperatura.

   Autor: Tiago Melo
   Blog: Microcontrolandos
   Compilador: MikroC PRO PIC
   Bibliotecas: ADC, Conversions, C_String, Lcd, Lcd_Constants
*/

//Pinos do LCD.
sbit LCD_RS at RB0_bit;
sbit LCD_EN at RB1_bit;
sbit LCD_D4 at RB2_bit;
sbit LCD_D5 at RB3_bit;
sbit LCD_D6 at RB4_bit;
sbit LCD_D7 at RB5_bit;
sbit LCD_RS_Direction at TRISB0_bit;
sbit LCD_EN_Direction at TRISB1_bit;
sbit LCD_D4_Direction at TRISB2_bit;
sbit LCD_D5_Direction at TRISB3_bit;
sbit LCD_D6_Direction at TRISB4_bit;
sbit LCD_D7_Direction at TRISB5_bit;

//Resolição de 10 bits do canal analógico.
#define RESOLUCAO_ADC 1024

//Para ler apenas temperaturas positivas.
unsigned LM35_Read(char channel) {
    unsigned long value0 = ADC_Get_Sample(channel);
    return value0 * 500UL / RESOLUCAO_ADC;
}

//Para ler temperaturas tanto negativas quanto positivas.
signed LM35_Negative_Read(char channel1, char channel2) {
    long value0 = ADC_Get_Sample(channel1);
    long value1 = ADC_Get_Sample(channel2);
    return (value0 - value1) * 500L / RESOLUCAO_ADC;
}

void main() {
    char texto[7]; //texto para guarda o valor da temperatura.
    signed temperaturaSensor0;
    unsigned temperaturaSensor1;

    //Desativa os comparadores analógico.
    CMCON = 0x07;
    //Inicializa o conversor analógico.
    ADC_Init();
    //Inicializa o LCD.
    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);

    while(1) {
        Lcd_Out(1, 1, "Sensor 0:");
        //Lê a temperatura do sensor 0.
        temperaturaSensor0 = LM35_Negative_Read(0, 1);
        //Converte para texto.
        IntToStr(temperaturaSensor0, texto);
        //Escreve o texto no LCD
        //ignorando os dois caracteres iniciais (que são espaços em branco).
        Lcd_Out_CP(texto + 2);
        //Escreve o °C.
        Lcd_Chr_CP(223);
        Lcd_Chr_CP('C');

        Lcd_Out(2, 1, "Sensor 1:");
        //Lê a temperatura do sensor 1.
        temperaturaSensor1 = LM35_Read(2);
        //Converte para texto.
        IntToStr(temperaturaSensor1, texto);
        //Escreve o texto no LCD
        //ignorando os dois caracteres iniciais (que são espaços em branco).
        Lcd_Out_CP(texto + 2);
        //Escreve o °C.
        Lcd_Chr_CP(223);
        Lcd_Chr_CP('C');

        //Faz nada por 1s.
        Delay_ms(1000);
    }
}