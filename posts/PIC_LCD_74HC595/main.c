/*
   LCD595 - LCD com 74HC595
   Autor: Tiago Melo
   Blog: Microcontrolandos
   Compilador: MikroC PRO PIC
   Bibliotecas: Lcd_Constants, Conversions, C_String
*/

sbit LCD595_Data at GP2_bit;
sbit LCD595_Clock at GP1_bit;
sbit LCD595_Latch at GP4_bit;
sbit LCD595_Data_Direction at TRISIO2_bit;
sbit LCD595_Clock_Direction at TRISIO1_bit;
sbit LCD595_Latch_Direction at TRISIO4_bit;

static void LCD595_Send_Bit(char value) {
    LCD595_Data = value;
    //Pulso de clock desloca e carrega o bit no 74HC595
    LCD595_Clock = 1;
    Delay_us(1);
    LCD595_Clock = 0;
}

//Envia 4 bits dos dados + o pino RS
static void LCD595_Send(char nibble) {
    char i, k, aux = nibble;
    LCD595_Clock = 0;
    //Enviar duas vezes para pulsar o pino E
    for(k = 0; k < 2; k++) {
        //Pino E
        LCD595_Send_Bit(!k.B0);
        //Pino RS
        LCD595_Send_Bit(nibble.B4);
        //Pinos D4 - D7
        for(i = 0; i < 4; i++) {
            LCD595_Send_Bit(nibble.B3);
            nibble <<= 1;
        }
        //Atualiza as saídas do 74HC595
        LCD595_Latch = 1;
        Delay_us(1);
        LCD595_Latch = 0;
        LCD595_Data = 0;
        //De novo...
        nibble = aux;
    }
    Delay_us(100);
}

//Envia um comando
void LCD595_Cmd(char cmd) {
    LCD595_Send(cmd >> 4);
    LCD595_Send(cmd & 0x0F);
    Delay_ms(5);
}

//Define a posição do cursor. Começa no 1.
void LCD595_Set_Cursor(char row, char col) {
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
    LCD595_Cmd(row + col);
}

//Envia um dado ou escreve um caracter na posição atual do cursor
void LCD595_Chr_CP(char character) {
    LCD595_Send((character >> 4 ) | 0x10);
    LCD595_Send((character & 0x0F ) | 0x10);
}

//Envia um dado ou escreve um caracter na posição especificada
void LCD595_Chr(char row, char col, char character) {
    LCD595_Set_Cursor(row, col);
    LCD595_Chr_CP(character);
}

//Escreve um texto na posição atual do cursor
void LCD595_Out_CP(char *text) {
    while(*text) {
        LCD595_Chr_CP(*text++);
    }
}

//Escreve um texto na posição especificada
void LCD595_Out(char row, char col, char *text) {
    LCD595_Set_Cursor(row, col);
    LCD595_Out_CP(text);
}

//Inicializa o LCD
void LCD595_Init() {
    //Define a direção dos pinos. Tudo como saída.
    LCD595_Data_Direction = 0;
    LCD595_Clock_Direction = 0;
    LCD595_Latch_Direction = 0;

    Delay_ms(50);

    LCD595_Send(3);
    Delay_us(4500);
    LCD595_Send(3);
    Delay_us(4500);
    LCD595_Send(3);
    Delay_us(150);
    LCD595_Send(2);
    //modo 4bits, 5x8, 2 linhas
    LCD595_Cmd(0x28);
    //Display ON
    LCD595_Cmd(12);
    //left, shift decrement
    LCD595_Cmd(6);
}

void main() {
    char texto[5], contador = 0;
    //Desabilita o comparador analogico
    CMCON = 7;
    //Desabilita o canal analogico
    ANSEL = 0;
    //Iniciliza o display LCD
    LCD595_Init();
    LCD595_Cmd(_LCD_CLEAR);
    //Escreve o texto no LCD na linha 1, coluna 3
    LCD595_Out(1, 3, "LCD 74HC595");

    while(1) {
        //Converte o numero em string
        ByteToStr(contador++, texto);
        //Escreve o texto no LCD na linha 2, coluna 7
        LCD595_Out(2, 7, texto);
        //Faz nada por 1s
        Delay_ms(1000);
    }
}