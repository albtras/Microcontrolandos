Muitas vezes precisamos ter mais saídas digitais do que nosso microcontrolador possui. Displays de LCD, displays de 7 segmentos ou matrizes de LEDs necessitam de muitos pinos e por isso acabam esgotando o número de pinos disponíveis.

O [74HC595](http://www.ti.com/lit/ds/symlink/sn74hc595.pdf) vem pra solucionar esse problema. Com apenas 3 pinos ligados ao microcontrolador, é possíveis obter dezenas ou até centenas de pinos de saída.

![](http://www.microcontrolandos.com.br/github/PIC_74HC595/2.png)

Os 3 pinos que se comunicam com o microcontrolador são: `DS` (dados), `SH_CP` ou `SHIFT CLOCK` (pulso de clock) e o `ST_CP` ou `LATCH CLOCK` (pulso de latch).

Os pinos de saída são os pinos `Q0` ou `QA` até o pino `Q7` ou `QH`.

Ainda possui mais 2 pinos: 

* `MR` (Master Reset) que zera os pinos de saída ao receber `0`(LOW);
* `OE` (Output Enable) que desabilita as saídas ao receber `1`(HIGH) e habilita as saídas ao receber `0`.

### Funcionamento

O 74HC595 possui um registrador interno de 8 bits que armazena os bits que chegam serialmente no pino `DS`. Ao enviar um bit, um pulso de clock (LOW para HIGH) deve ser aplicado para armazená-lo como o bit menos significativo do registrador interno e deslocar os demais. O bit excedente é colocado no pino `Q7'` ou `SQH` , podendo este ser encadeado com outro 74HC595 (O pino `Q7'` é ligado ao pino `DS` de outro 74HC595).

No final do processo, queremos que os bits enviados sejam colocados nos pinos de saída do 74HC595. Para isso devemos enviar um pulso de latch (LOW para HIGH).

### Código-Fonte

```c
/*
   74HC595 - Registrador de Deslocamento
   Autor: Tiago Melo
   Blog: Microcontrolandos
   Compilador: MikroC PRO PIC
   Bibliotecas:
*/

sbit SR74HC595_Data at RB1_bit;
sbit SR74HC595_Clock at RB0_bit;
sbit SR74HC595_Latch at RB2_bit;
sbit SR74HC595_Data_Direction at TRISB1_bit;
sbit SR74HC595_Clock_Direction at TRISB0_bit;
sbit SR74HC595_Latch_Direction at TRISB2_bit;

//Envia o pulso de latch.
static void SR74HC595_Send_Latch() {
    //Pulso de latch para atualizar as saídas.
    SR74HC595_Latch = 0;
    Delay_us(1);
    SR74HC595_Latch = 1;
    SR74HC595_Latch = 0;
}

//Envia um bit.
static void SR74HC595_Send_Bit(char value) {
    SR74HC595_Data = value != 0;
    //Pulso de clock (LOW para HIGH) carrega e desloca o bit no 74HC595.
    SR74HC595_Clock = 0;
    Delay_us(1);
    SR74HC595_Clock = 1;
    SR74HC595_Clock = 0;
}

//Envia 8 bits de dados.
static void SR74HC595_Send(char byte) {
    char i;
    SR74HC595_Clock = 0;
    //Envia os 8 bits.
    for(i = 0; i < 8; i++) {
        SR74HC595_Send_Bit(byte.B7);
        byte <<= 1;
    }
    SR74HC595_Data = 0;
}

//Inicializa os pinos conectados com o 74HC595.
void SR74HC595_Init() {
    SR74HC595_Data_Direction = 0;
    SR74HC595_Clock_Direction = 0;
    SR74HC595_Latch_Direction = 0;
    SR74HC595_Data = 0;
    SR74HC595_Clock = 0;
    SR74HC595_Latch = 0;
}

//Envia 8 bits.
void SR74HC595_Send_Char(char c) {
    SR74HC595_Send(c);
    //Atualiza as saídas do 74HC595.
    SR74HC595_Send_Latch();
}

//Envia 16 bits.
void SR74HC595_Send_Int(unsigned int i) {
    SR74HC595_Send(((char*)&i)[1]); //Envia os 8 bits mais significativos.
    SR74HC595_Send(((char*)&i)[0]); //Envia os 8 bits menos significativos.
    //Atualiza as saídas do 74HC595.
    SR74HC595_Send_Latch();
}

//Envia 32 bits.
void SR74HC595_Send_Long(unsigned long l) {
    SR74HC595_Send(((char*)&l)[3]); //Envia os 8 bits mais significativos.
    SR74HC595_Send(((char*)&l)[2]);
    SR74HC595_Send(((char*)&l)[1]);
    SR74HC595_Send(((char*)&l)[0]); //Envia os 8 bits menos significativos.
    //Atualiza as saídas do 74HC595.
    SR74HC595_Send_Latch();
}

//Envia n bytes.
void SR74HC595_Send_N_Bytes(void * bytes, int nbytes) {
    char * p = (char*)bytes;
    while(nbytes) {
        SR74HC595_Send(p[--nbytes]);
    }
    //Atualiza as saídas do 74HC595.
    SR74HC595_Send_Latch();
}

//Envia n bits.
void SR74HC595_Send_N_Bits(void * bytes, int nbits) {
    int byte = nbits / 8;
    char b = 1 << ((nbits - 1) % 8);
    char * p = (char*)bytes;
    //Para cada byte.
    while(byte >= 0) {
        //Para cada bit.
        while(b != 0) {
            SR74HC595_Send_Bit(p[byte] & b);
            b >>= 1;
        }
        b = 0x80;
        byte--;
    }
    //Atualiza as saídas do 74HC595.
    SR74HC595_Send_Latch();
}

#define Delay_5s() Delay_ms(5000)

void main() {
    long numeroCom32bits = 0x711755AA;
    
    //Inicia os pinos conectados ao 74HC595.
    SR74HC595_Init();

    while(1) {
        SR74HC595_Send_N_Bits(&numeroCom32bits, 11);
        Delay_5s();
        SR74HC595_Send_N_Bytes(&numeroCom32bits, 3);
        Delay_5s();
        SR74HC595_Send_Long(numeroCom32bits);
        Delay_5s();
        SR74HC595_Send_Int(numeroCom32bits);
        Delay_5s();
        SR74HC595_Send_Char(numeroCom32bits);
        Delay_5s();
    }
}
```

### Download

[Baixe o projeto aqui!](http://www.microcontrolandos.com.br/link/microcontrolandos-github)