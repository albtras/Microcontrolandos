[TC1047A](http://ww1.microchip.com/downloads/en/DeviceDoc/21498D.pdf) é um sensor de temperatura.

### Características

- Tensão de operação: 2.5V até 5.5V;
- Acurácia de ±2°C;
- Faixa mensurável de temperatura: -40°C até 125°C;
- Baixo consumo de energia: 35uA.

### Funcionamento

O TC1047 converte o valor de temperatura em um valor de tensão linearmente proporcional. Cada `10mV` corresponde a `1°C`.

![img](http://www.microcontrolandos.com.br/github/PIC_TC1047A/2.png)

Através do canal analógico do microcontrolador, iremos ler o valor digital da temperatura. Numa temperatura de `-40°C` o TC1047A tem uma tensão de saída de `0,1V`. A `0°C` o TC1047A tem uma tensão de saída de `0,5V` e a `125°C` tem uma tensão de saída de `1,75V`. Precisamos converter o valor lido do canal analógico em um valor de temperatura. Para isso vamos converter este valor em um valor de tensão. No caso de um microcontrolador com um ADC de `10 bits`, o valor de tensão (em mV) será: `ADC * 5000 / 1024`. Ainda temos que descontar`500mV` (pois a 0°C a tensão de saída é 0,5V). Como a resolução da tensão de saída é de 10mV/°C, temos que dividir tudo por `10`. Então nossa fórmula fica assim: `ADC * 500 / 1024 - 50`.

### Código-Fonte

```c
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
```

### Download

[Baixe o projeto aqui!](http://www.microcontrolandos.com.br/link/microcontrolandos-github)