Demonstração de como utilizar um [LCD Serial](http://www.farnell.com/datasheets/11357.pdf).

Através de apenas um fio e comunicando via UART, esse tipo de LCD se torna bem mais fácil de utilizar em relação aos demais.

O código abaixo permite que você possa escrever um texto na tela, mover o cursor ou desenhar seu próprio caractere.

### Código-Fonte
```c
/*
   SERIAL LCD - LCD comunicando via UART.
   Autor: Tiago Melo
   Blog: Microcontrolandos
   Compilador: MikroC PRO PIC
   Bibliotecas: UART, Lcd_Constants
*/

#define UART_Init(baud_rate) UART1_Init(baud_rate)
#define UART_Write(byte) UART1_Write(byte)
#define INSTRUCTION_MODE 254

static char ROW_ADDR[] = {128, 192, 148, 212};

//Envia um comando.
void SerialLCD_Cmd(char cmd) {
    UART_Write(INSTRUCTION_MODE);
    UART_Write(cmd);
    Delay_ms(5);
}

//Inicia o display LCD.
void SerialLCD_Init() {
    UART_Init(9600); //Altere aqui o baud rate do LCD.
    Delay_ms(500);
    SerialLCD_Cmd(_LCD_TURN_ON);
}

//Posiciona o cursor numa determinada linha e coluna.
void SerialLCD_Set_Cursor(char row, char col) {
    SerialLCD_Cmd(ROW_ADDR[row] + col);
}

//Escreve um texto na posição atual do cursor.
#define SerialLCD_Chr_CP(c) UART_Write(c)

//Escreve um caractere numa determinada linha e coluna.
void SerialLCD_Chr(char row, char col, char c) {
    SerialLCD_Set_Cursor(row, col);
    SerialLCD_Chr_CP(c);
}

//Escreve um texto na posição atual do cursor.
void SerialLCD_Out_CP(char * text) {
    while(*text) SerialLCD_Chr_CP(*text++);
}

//Escreve um texto numa determinada linha e coluna.
void SerialLCD_Out(char row, char col, char * text) {
    SerialLCD_Set_Cursor(row, col);
    SerialLCD_Out_CP(text);
}

//Cria um caractere em uma determinada posição.
//No máximo é posível criar até 8 caracteres.
void SerialLCD_Create_Char(char pos, char * map) {
    char i;
    SerialLCD_Cmd(64 + ((pos & 0x7) << 3));
    for(i = 0; i < 8; i++) SerialLCD_Chr_CP(map[i]);
}

void main() {
    char coracao[] = {0,10,31,31,31,14,4,0}; //desenho de um coração criado no LCD Custom Char.
    char i;
    //Inicia a comunicação com o LCD.
    SerialLCD_Init();
    //Use as mesmas constantes da biblioteca LCD do mikroc.
    SerialLCD_Cmd(_LCD_CLEAR); //Limpa o LCD.
    //Cria um caractere e coloca na posição 5.
    SerialLCD_Create_Char(5, coracao);
    for(i = 0; i < 4; i++) {
        //Desenha na linha i, coluna 0, o caractere que criei na posição 5.
        SerialLCD_Chr(i, 0, 5);
        //Escreve um texto na posição atual do cursor.
        SerialLCD_Out_CP("Microcontrolandos");
        //Escreve o caractere criado na posição atual do cursor.
        SerialLCD_Chr_CP(5);
    }
}
```

### Criando um caractere

Para criar seu próprio caractere, o Mikroc tem uma ferramenta só para isso. Vá no menu `Tools` e em seguida clique em `LCD Custom Char`.

![](http://www.microcontrolandos.com.br/github/PIC_LCD_SERIAL/2.png)

1. Escolha o tamanho da fonte do LCD que é `5x7`;
2. Desenhe o caractere desejado;
3. Clique no botão `Generate Code`;
4. Copie a primeira linha de código e cole no seu programa. Mude o nome da variável se desejar.

### Download

[Clique aqui para baixar o projeto](http://www.microcontrolandos.com.br/link/microcontrolandos-github)