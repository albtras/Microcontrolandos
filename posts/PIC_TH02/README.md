[TH02](http://www.hoperf.com/upload/sensor/TH02_V1.1.pdf) é um sensor de temperatura e umidade.

![](http://www.microcontrolandos.com.br/github/PIC_TH02/2.png)

Apesar de ser desconhecido para mim, ele aparenta ser um excelente sensor. Baixo consumo de energia, rapidez na medição e boa resolução são os pontos destacáveis desse dispositivo.

### Características

* Tensão de operação: 2.1V à 3.6V;
* Comunicação via I2C;
* Acurácia de 0.5°C para temperatura e 4.5% para umidade;
* Faixa de operação: -40°C à 85°C e 0% - 100%;
* Baixo consumo de energia: 240uA à 320uA durante a conversão;
* Resolução de 14 ou 13 bits para temperatura;
* Resolução de 12 ou 11 bits para umidade;
* Tempo de conversão: 35ms no modo normal e 18ms no modo rápido.

### Funcionamento

O TH02 possui um registro de configuração `CONFIG` (0x03) na qual é utilizado para definir se estamos querendo ler a temperatura ou a umidade (`TEMP`), selecionar a velocidade da conversão (`FAST`) e iniciar uma conversão (`START`).

![](http://www.microcontrolandos.com.br/github/PIC_TH02/3.png)

O bit `FAST` reduz o tempo de conversão de 35ms para 18ms e o consumo de energia. Consequentemente, reduz a resolução da medição.

![](http://www.microcontrolandos.com.br/github/PIC_TH02/4.png)

Então sempre que quisermos realizar uma leitura de temperatura ou umidade, devemos escrever no registro `CONFIG`.

O valor de 16 bits da temperatura e da umidade encontra-se nos registros `DATAH` (0x01) e `DATAL` (0x02).

![](http://www.microcontrolandos.com.br/github/PIC_TH02/5.png)

![](http://www.microcontrolandos.com.br/github/PIC_TH02/6.png)

A sequência para iniciar a conversão e ler os valores é:

* Iniciar a transmissão (pulso de START);
* Enviar o endereço do dispositivo (0x80) + modo de escrita (0);
* Enviar o endereço do registro `CONFIG`;
* Enviar o valor do registro `CONFIG`: Ler temperatura ou umidade, modo rápido ou normal, iniciar a conversão;
* Encerrar a transmissão (pulso de STOP);
* Aguardar o fim da conversão, de acordo com o modo;
* Iniciar a transmissão (pulso de START);
* Enviar o endereço do dispositivo (0x80) + modo de escrita (0);
* Enviar o endereço do registro `DATAH`;
* Encerrar a transmissão (pulso de STOP);
* Iniciar a transmissão (pulso de START);
* Enviar o endereço do dispositivo (0x80) + modo de leitura (1);
* Ler os dois bytes (MSB e LSB);
* Encerrar a transmissão (pulso de STOP);

### Código-Fonte

```c
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
```

### Download

[Baixe o projeto aqui!](http://www.microcontrolandos.com.br/link/microcontrolandos-github)