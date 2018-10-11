[DS1631](https://datasheets.maximintegrated.com/en/ds/DS1631-DS1731.pdf) é um termômetro/termostato digital que realiza a leitura de temperaturas entre -55°C e 125°C com resolução ajustável de 9, 10, 11 ou 12 bits.

A comunicação é feita através da inteface I2C.

### Características

- Acurácia de ±0,5°C;
- Tensão de alimentação: 3V a 5,5V;
- Baixo consumo de energia.

### Registros

Possui 4 registros, `TH` e `TL` (ambos 2 bytes) usados no modo Termostato, o registro `T` (2 bytes) usado no modo Termômetro e o registro de Configuração `CONFIG` (1 byte).

Os registros `TH`, `TL` e `T` são formados pelos seguintes bits:

![](http://www.microcontrolandos.com.br/github/PIC_DS1631/2.png)

### Modo Termômetro

Por meio de um conversor analógico Delta-Sigma coverte a temperatura num valor digital de 9, 10, 11 ou 12 bits, configurados pelos bits `R0` e `R1` do registro de `CONFIG`. A conversão demora um certo tempo dependendo da resolução, veja a tabela abaixo:

![](http://www.microcontrolandos.com.br/github/PIC_DS1631/3.png)

O DS1631 inicia em *stand-by* (modo de baixo consumo), e através de um comando enviado, realiza a conversão da temperatura em modo *contínuo* ou *one-shot*.

No modo *one-shot* a conversão da temperatura é feita sempre que recebe o comando `START CONVERT`.

No modo *contínuo* a conversão é realiza continuamente. É necessário enviar `START CONVERT` pelo menos uma vez só pra dar aquele empurrão inicial.

Podemos configurá-los através do bit `ONE-SHOT` do registro `CONFIG`. Setando com `1` estaremos definindo com o modo *one-shot* e com `0` para o modo *contínuo*.

Para encerrar a conversão usamos o comando `STOP CONVERT`. Isto faz com que entre no modo *stand-by*. Isto é útil quando está no modo *contínuo* e não queremos mais ler o valor de temperatura, fazendo o dispositivo economizar em energia. No modo *one-shot* não é necessário enviar este comando, pois após a conversão automaticamente o dispositivo entra em *stand-by*.

Para ler a temperatura use o comando `READ TEMPERATURE`.

### Modo Termostato

No modo termostato temos dois registros, `TH` e `TL`, que são usados para armazenar os dois valores de temperatura. Quando a temperatura ultrapassar o valor de `TH` o pino `TOUT` é `ativado`. O pino `TOUT`permanece ativado até que a temperatura caia e fique abaixo de `TL`.

Podemos configurar se o pino `TOUT` será ativado com nível lógico `HIGH` ou `LOW`, através do bit `POL` do registro `CONFIG`. Defina com `1` para ativá-lo com nivel lógico `HIGH` ou `0` para ativá-lo com nível lógico `LOW`.

### Endereçamento

O DS1631 possui três pinos, `A2`, `A1` e `A0`, responsáveis por alterar seu endereço no barramento e são representados pelos bit `3`, `2` e `1`, respectivamente.

O bit `0` define se nós iremos ler(1) ou escrever(0) no dispositivo.

O endereço é composto por:

![](http://www.microcontrolandos.com.br/github/PIC_DS1631/4.png)

### Código-Fonte

```c
/*
   DS1631 - Termômetro e Termostato.

   Autor: Tiago Melo
   Blog: Microcontrolandos
   Compilador: MikroC PRO PIC
   Bibliotecas: Conversions, C_String, Lcd, Lcd_Constants, Soft_I2C
*/

//Pinos do LCD
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
//Pinos do DS1631
sbit Soft_I2C_Scl at RC6_bit;
sbit Soft_I2C_Sda at RC7_bit;
sbit Soft_I2C_Scl_Direction at TRISC6_bit;
sbit Soft_I2C_Sda_Direction at TRISC7_bit;

//Comandos
#define DS1631_CMD_START_CONVERT 0x51
#define DS1631_CMD_STOP_CONVERT 0x22
#define DS1631_CMD_READ 0xAA
#define DS1631_CMD_ACCESS_TH 0xA1
#define DS1631_CMD_ACCESS_TL 0xA2
#define DS1631_CMD_ACCESS_CFG 0xAC
#define DS1631_CMD_SOFT_POR 0x54

typedef union {
    char ONE_SHOT : 1; // 1 - One-Shot Mode, 0 - Continuous Conversion Mode
    char POL : 1;  // 1 - TOUT is active high, 0 - TOUT is active low
    char R : 2;    //00 -> 9 bits, 01 -> 10 bits, 10 -> 11 bits, 11 -> 12 bits
    char NVB : 1;  //A write to EEPROM memory is in progress
    char TLF : 1;
    char THF : 1;
    char DONE : 1; //Temperature conversion is complete.
} DS1631_Config;

typedef union {
    char : 4;
    char decimal : 4;
    signed char inteiro : 8;
} DS1631_Temperature;

#define DS1631_ADDRESS(addr) (0x90 | (addr << 1))

//Lê dois bytes de um registro.
static unsigned DS1631_Read(char address, char reg) {
    unsigned value;
    Soft_I2C_Start(); //sinal de start
    Soft_I2C_Write(DS1631_ADDRESS(address)); //envia o endereço do dispositivo + modo de escrita
    Soft_I2C_Write(reg); //envia o comando de acesso ao registro
    Soft_I2C_Start(); //sinal de start
    Soft_I2C_Write(DS1631_ADDRESS(address) | 1); //envia o endereço do dispositivo + modo de leitura
    ((char*)&value)[1] = Soft_I2C_Read(1); //le o byte mais significativo
    ((char*)&value)[0] = Soft_I2C_Read(0); //le o byte menos significativo
    Soft_I2C_Stop(); //sinal de stop
    return value;
}

//Escreve dois bytes em um registro.
static void DS1631_Write(char address, char reg, unsigned value) {
    Soft_I2C_Start();
    Soft_I2C_Write(DS1631_ADDRESS(address));
    Soft_I2C_Write(reg);
    Soft_I2C_Write(((char*)&value)[1]); //MSB
    Soft_I2C_Write(((char*)&value)[0]); //LSB
    Soft_I2C_Stop();
}

//Inicializa o DS1631 com determinadas configurações.
void DS1631_Init(char address, DS1631_Config * config) {
    Soft_I2C_Start(); //sinal de start
    Soft_I2C_Write(DS1631_ADDRESS(address)); //envia o endereço do dispositivo + modo de escrita
    Soft_I2C_Write(DS1631_CMD_ACCESS_CFG); //envia o comando de acesso ao registro de configuração
    Soft_I2C_Write(*(char*)config); //envia o valor da configuração
    Soft_I2C_Stop(); //sinal de stop
    Delay_ms(10); //EEPROM Write Cycle Time
}

//Inicia a conversão.
void DS1631_Start(char address) {
    Soft_I2C_Start(); //sinal de start
    Soft_I2C_Write(DS1631_ADDRESS(address)); //envia o endereço do dispositivo + modo de escrita
    Soft_I2C_Write(DS1631_CMD_START_CONVERT);  //envia o comando para iniciar a conversão
    Soft_I2C_Stop(); //sinal de stop
}

//Encerra a conversão.
void DS1631_Stop(char address) {
    Soft_I2C_Start(); //sinal de start
    Soft_I2C_Write(DS1631_ADDRESS(address)); //envia o endereço do dispositivo + modo de escrita
    Soft_I2C_Write(DS1631_CMD_STOP_CONVERT);  //envia o comando para parar a conversão
    Soft_I2C_Stop(); //sinal de stop
}

void DS1631_Reset(char address) {
    Soft_I2C_Start(); //sinal de start
    Soft_I2C_Write(DS1631_ADDRESS(address)); //envia o endereço do dispositivo + modo de escrita
    Soft_I2C_Write(DS1631_CMD_SOFT_POR); //envia o comando de reset
    Soft_I2C_Stop(); //sinal de stop
}

//Lê a temperatura.
DS1631_Temperature DS1631_Read_Temperature(char address) {
    DS1631_Temperature temp;
    *((unsigned*)&temp) = DS1631_Read(address, DS1631_CMD_READ);
    return temp;
}

//Lê o limite inferior do termostato.
DS1631_Temperature DS1631_Read_Temperature_Low(char address) {
    DS1631_Temperature temp;
    *((unsigned*)&temp) = DS1631_Read(address, DS1631_CMD_ACCESS_TL);
    return temp;
}

//Lê o limite superior do termostato.
DS1631_Temperature DS1631_Read_Temperatura_High(char address) {
    DS1631_Temperature temp;
    *((unsigned*)&temp) = DS1631_Read(address, DS1631_CMD_ACCESS_TH);
    return temp;
}

//Escreve os limites inferiores e superiores do termostato.
void DS1631_Write_Temperature(char address, int low, int high) {
    DS1631_Write(address, DS1631_CMD_ACCESS_TH, high << 8);
    //EEPROM Write Cycle Time.
    Delay_ms(10);
    DS1631_Write(address, DS1631_CMD_ACCESS_TL, low << 8);
    //EEPROM Write Cycle Time.
    Delay_ms(10);
}

void main() {
    DS1631_Config config;
    DS1631_Temperature temp;
    char msg[7];

    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);

    config.ONE_SHOT = 1; //modo one-shot.
    config.POL = 1; //Tout ativado com nível lógico alto.
    config.R = 3; //Resolucao de 12 bits.
    //Inicializa o DS1631.
    DS1631_Init(0, &config);
    //Define os limites inferiores e superiores do termostato.
    DS1631_Write_Temperature(0, 25, 30);

    while(1) {
        //Inicia a conversao.
        DS1631_Start(0);

        //Espera 750ms, pois a resolucao é de 12 bits.
        Delay_ms(750);

        //Escreve no display LCD.
        Lcd_Cmd(_LCD_CLEAR);
        Lcd_Out(1, 5, "DS1631");
        Lcd_Out(2, 1, "Temp: ");

        //Realiza a leitura da temperatura e converte para string.
        temp = DS1631_Read_Temperature(0);
        IntToStr(temp.inteiro, msg);
        Lcd_Out_CP(msg + 3);
        Lcd_Chr_CP('.');
        IntToStrWithZeros(temp.decimal * 625, msg);
        Lcd_Out_CP(msg + 2);
        Lcd_Chr_CP(223); //°
        Lcd_Chr_CP('C');
    }
}
```

### Download

[Baixe o projeto aqui!](http://www.microcontrolandos.com.br/link/microcontrolandos-github)