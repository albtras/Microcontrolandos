Fuçando na net, encontrei este incrível trabalho de um cara chamado Roman Black, na qual utiliza um 74HC595 conectado a apenas 1 fio.

Clique [aqui](http://www.romanblack.com/shift1.htm) para conhecê-lo.

Um simples circuito RC proporciona os atrasos necessários para o seu funcionamento.

![](https://github.com/tiagohm/Microcontrolandos/raw/master/PIC_LCD_1WIRE_74HC595/2.gif?raw=true)

![](https://github.com/tiagohm/Microcontrolandos/raw/master/PIC_LCD_1WIRE_74HC595/3.gif?raw=true)

### Observações
1. É necessário um clock superior ou igual a `8MHz`.
2. O pino `Q0` será sempre zero, por isso não é utilizado.
3. O projeto original utiliza capacitores de `2,2nF`. Nos testes feitos no Proteus, só funcionou usando capacitores de `4,7nF`. No caso de não funcionar corretamente, teste valores próximos.

### Código-Fonte

```c
sbit RomanLCD_Data at RB0_bit;
sbit RomanLCD_Data_Direction at TRISB0_bit;

static void RomandLCD_Send_ZeroBit() {
    RomanLCD_Data = 1;
    Delay_us(5);
    RomanLCD_Data = 0;
    Delay_us(15);
    RomanLCD_Data = 1;
    Delay_us(30);
}

static void RomandLCD_Send_OneBit() {
    RomanLCD_Data = 1;
    Delay_us(5);
    RomanLCD_Data = 0;
    Delay_us(1);
    RomanLCD_Data = 1;
    Delay_us(15);
}

static void RomanLCD_Send_Bit(char value) {
    if(value.B0) RomandLCD_Send_OneBit();
    else RomandLCD_Send_ZeroBit();
}

static void RomandLCD_Send_Latch() {
    RomanLCD_Data = 1;
    Delay_us(5);
    RomanLCD_Data = 0;
    Delay_us(200);
    RomanLCD_Data = 1;
    Delay_us(300);
}

static void RomanLCD_Send(char nibble) {
    char i, k, aux = nibble;
    RomanLCD_Data = 1;
    /* Enviar duas vezes para pulsar o pino E */
    for(k = 0; k < 2; k++) {
        /* Pino E */
        RomanLCD_Send_Bit(!k.B0);
        /* Pino RS */
        RomanLCD_Send_Bit(nibble.B4);
        /* Pinos D4 - D7 */
        for(i = 0; i < 4; i++) {
            RomanLCD_Send_Bit(nibble.B3);
            nibble <<= 1;
        }
        /* Atualiza as saídas do 74HC595 */
        RomandLCD_Send_Latch();
        //De novo...
        nibble = aux;
    }
    Delay_us(100);
}

void RomanLCD_Cmd(char cmd) {
    RomanLCD_Send(cmd >> 4);
    RomanLCD_Send(cmd & 0x0F);
    Delay_ms(5);
}

void RomanLCD_Init() {
    RomanLCD_Data_Direction = 0; //pino de saída.
    RomanLCD_Data = 1;

    Delay_ms(50);

    RomanLCD_Send(3);
    Delay_us(4500);
    RomanLCD_Send(3);
    Delay_us(4500);
    RomanLCD_Send(3);
    Delay_us(150);
    RomanLCD_Send(2);
    //modo 4bits, 5x8, 2 linhas
    RomanLCD_Cmd(0x28);
    //Display ON
    RomanLCD_Cmd(12);
    //left, shift decrement
    RomanLCD_Cmd(6);
}

void RomanLCD_Set_Cursor(char row, char col) {
    switch(row) {
    case 2:
        row = 191;
        break;
    case 3:
        row = 147;
        break;
    case 4:
        row = 211;
        break;
    default:
        row = 127;
    }
    RomanLCD_Cmd(row + col);
}

void RomanLCD_Chr_CP(char c) {
    RomanLCD_Send((c >> 4) | 0x10);
    RomanLCD_Send((c & 0x0F) | 0x10);
}

void RomanLCD_Chr(char row, char col, char c) {
    RomanLCD_Set_Cursor(row, col);
    RomanLCD_Chr_CP(c);
}

void RomanLCD_Out_CP(char * text) {
    while(*text) {
        RomanLCD_Chr_CP(*text++);
    }
}

void RomanLCD_Out(char row, char col, char * text) {
    RomanLCD_Set_Cursor(row, col);
    RomanLCD_Out_CP(text);
}

void main() {
    char i = 0;
    char texto[4];
    RomanLCD_Init();
    RomanLCD_Cmd(_LCD_CLEAR);
    RomanLCD_Out(1, 3, "Microcontrolandos");
    RomanLCD_Out(2, 2, "LCD 1-Wire 74HC595");
    RomanLCD_Out(3, 7, "Contador");

    while (1) {
        ByteToStr(i++, texto);
        RomanLCD_Out(4, 9, texto);
        Delay_ms(1000);
    }
}
```

### Download

[Baixe o projeto aqui!](https://github.com/tiagohm/Microcontrolandos/tree/master/PIC_LCD_1WIRE_74HC595)