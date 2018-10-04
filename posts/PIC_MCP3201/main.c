/*
   MCP3201 - Conversor Analógico/Digital de 12-bits.

   Autor: Tiago Melo
   Blog: Microcontrolandos
   Compilador: MikroC PRO PIC
   Bibliotecas: LCD, Lcd_Constants, Conversions, C_String
*/

//Pinos do MCP3201.
sbit MCP3201_Clock at RB0_bit;
sbit MCP3201_Data at RB1_bit;
sbit MCP3201_CS at RB2_bit;
sbit MCP3201_Clock_Direction at TRISB0_bit;
sbit MCP3201_Data_Direction at TRISB1_bit;
sbit MCP3201_CS_Direction at TRISB2_bit;
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

//Inicializa o pinos de comunicação com MCP3201.
void MCP3201_Init() {
    MCP3201_CS_Direction = 0; //Pino de saída.
    MCP3201_Clock_Direction = 0; //Pino de saída.
    MCP3201_Data_Direction = 1; //Pino de entrada.
    MCP3201_CS = 1;
    MCP3201_Clock = 0;
}

//Lê o valor bruto do MCP3201.
unsigned int MCP3201_Read() {
    char i;
    unsigned value;
    //Inicia a conversão.
    MCP3201_Clock = 1;
    MCP3201_CS = 0;
    //Lê 15 bits. Os 3 primeiros serão ignorados.
    for(i = 0; i < 15; i++) {
        value <<= 1;
        //Dá um pulso de descida. (HIGH para LOW).
        MCP3201_Clock = 1;
        Delay_us(1);
        MCP3201_Clock = 0;
        Delay_us(1);
        //Lê o valor após um pulso de descida do clock.
        value |= MCP3201_Data;
    }
    //Encerra.
    MCP3201_Clock = 0;
    MCP3201_CS = 1;
    //Força o retorno de apenas 12 bits.
    return (value & 0x0FFF);
}

//Converte um valor bruto em um valor de tensão.
float MCP3201_Convert(unsigned int raw, float vref) {
    return raw * vref / 4096.0;
}

void main() {
    char texto[15]; //15 caracteres para armazenar um float como texto.

    //Inicializa o MCP3201.
    MCP3201_Init();
    //Inicializa o LCD.
    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);

    while(1) {
        //Lê o valor.
        unsigned valor = MCP3201_Read();
        //Converte o valor lido em um valor de tensão com 5V de referência.
        float tensao = MCP3201_Convert(valor, 5);
        //Converte o valor da tensão em texto.
        FloatToStr(tensao, texto);
        //Escreve o texto no LCD.
        Lcd_Out(2, 1, texto);
        Lcd_Chr_CP('V');
        //Faz nada por 1s.
        Delay_ms(1000);
    }
}