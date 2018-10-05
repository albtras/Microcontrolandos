[LM35](http://www.ti.com/lit/ds/symlink/lm35.pdf) é um sensor de temperatura com saída analógica.

Mostrarei como ler tanto temperaturas positivas quanto negativas com esse sensor.

LM35 possui apenas 3 pinos: `VCC`, `VOUT` e `GND`. Pode ser alimentado com 4V até 20V. Consegue medir temperaturas que variam de -55°C até 150°C com precisão de ±1°C.

O pino `VOUT` é a saída analógica. Este, por sua vez, deve ser conectado a uma entrada do canal analógico do microcontrolador.

### Lendo temperaturas positivas

O pino `VOUT` gera em sua saída uma tensão linearmente proporcional à temperatura,  de 0V + 10mV por °C. 

Assim, devemos ler o valor do canal analógico, que varia de 0 a 1023 (para microcontroladores com 10 bits de resolução), onde 0 corresponde a 0V e 1023 corresponde a 5V. Então, temos que `tensao = [valor lido] * 5 / 1023`. Como cada °C corresponde a 10mV, isto é, `temperatura = tensao / 0,01`, temos que `temperatura = ([valor lido] * 5 / 1023) / 0,01` que é equivalente à `[valor lido] * 500 / 1023`. Como a precisão gira em torno de 1°C, é desnecessário trabalhar com número flutuantes.

### Lendo temperaturas negativas

Para ler valores negativos de temperatura é necessário acrescentar um pequeno circuito ao sensor.

![](http://microcontrolandos.com.br/github/PIC_LM35/2.png)

Veja que o circuito faz com que tenhamos duas saídas analógicas. Só teremos que ler o valor de cada saída e calcular sua diferença.

### Código-Fonte

```c
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
```

### Download

[Baixe o projeto aqui!](http://www.microcontrolandos.com.br/link/microcontrolandos-github)
