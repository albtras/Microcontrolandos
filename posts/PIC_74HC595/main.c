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