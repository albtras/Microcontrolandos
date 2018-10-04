/*
   TH02 - Sensor de temperatura e umidade.
   Autor: Tiago Melo
   Blog: Microcontrolandos
   Compilador: MikroC PRO PIC
   Bibliotecas: Soft_I2C, Lcd, Lcd_Constants, Conersions, C_String
*/

//Pinos do TH02.
sbit Soft_I2C_Scl at RB0_bit;
sbit Soft_I2C_Sda at RB1_bit;
sbit Soft_I2C_Scl_Direction at TRISB0_bit;
sbit Soft_I2C_Sda_Direction at TRISB1_bit;
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

//Resolução.
#define RES_14_12 0 //fast = 0
#define RES_13_11 1 //fast = 1
//Endereço.
#define TH02_ADDRESS(rw) (0x80 | rw)
//Registros.
#define TH02_STATUS_REG 0
#define TH02_DATAH_REG 1
#define TH02_DATAL_REG 2
#define TH02_CONFIG_REG 3
#define TH02_ID_REG 4

typedef union {
    char start : 1; //1 = inicia a conversão.
    char heat : 1; //habilita o heater(aquecedor)
    char : 2;
    char temp : 1; //1 = temperatura, 0 = umidade
    char fast : 1; //0 = modo lento, 1 = modo rápido
} TH02_Config_Data;

unsigned TH02_Read(TH02_Config_Data config) {
    unsigned valor;
    Soft_I2C_Start();
    //Inicia a conversão da temperatura.
    Soft_I2C_Write(TH02_ADDRESS(0));
    Soft_I2C_Write(TH02_CONFIG_REG);
    Soft_I2C_Write(*(char*)&config);
    Soft_I2C_Stop();
    //Aguarda o fim da conversão.
    if(config.fast) Delay_ms(20);
    else Delay_ms(40);
    //Lê os 2 bytes.
    Soft_I2C_Start();
    Soft_I2C_Write(TH02_ADDRESS(0));
    Soft_I2C_Write(TH02_DATAH_REG);
    Soft_I2C_Stop();
    Soft_I2C_Start();
    Soft_I2C_Write(TH02_ADDRESS(1));
    ((char*)&valor)[1] = Soft_I2C_Read(1);
    ((char*)&valor)[0] = Soft_I2C_Read(0);
    Soft_I2C_Stop();
    return valor;
}

float TH02_Read_Temperature(char fast) {
    TH02_Config_Data config;
    unsigned valor;
    //Lê o valor da temperatura.
    config.temp = 1;
    config.fast = fast;
    config.start = 1;
    valor = TH02_Read(config) >> 2;
    return valor / 32.0 - 50;
}

float TH02_Read_Humidity(char fast) {
    TH02_Config_Data config;
    unsigned valor;
    //Lê o valor da umidade.
    config.temp = 0;
    config.fast = fast;
    config.start = 1;
    valor = TH02_Read(config) >> 4;
    return valor / 16.0 - 24;
}

void main() {
    char texto[15];

    Soft_I2C_Init();
    //Inicializa o LCD.
    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);

    while(1) {
        //Temperatura. Modo lento (maior resolução).
        float valor = TH02_Read_Temperature(RES_14_12);
        //Converte em texto.
        FloatToStr(valor, texto);
        //Escreve no LCD.
        Lcd_Out(2, 1, texto);
        //Umidade. Modo rápido (menor resolução).
        valor = TH02_Read_Humidity(RES_13_11);
        //Converte em texto.
        FloatToStr(valor, texto);
        //Escreve no LCD.
        Lcd_Out(1, 1, texto);
        //Faz nada durante 1s.
        Delay_ms(1000);
    }
}