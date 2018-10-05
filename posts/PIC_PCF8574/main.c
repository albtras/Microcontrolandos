/*
   PCF8574 - Expansor de portas I/O de 8 bits.

   Autor: Tiago Melo
   Blog: Microcontrolandos
   Compilador: MikroC PRO PIC
   Bibliotecas: Soft_I2C
*/

//Pinos do PCF8574
sbit Soft_I2C_Scl at RA0_bit;
sbit Soft_I2C_Sda at RA1_bit;
sbit Soft_I2C_Scl_Direction at TRISA0_bit;
sbit Soft_I2C_Sda_Direction at TRISA1_bit;

#define DISPOSITIVO_0 (0x40 | (7 << 1))  //PCF8574
#define DISPOSITIVO_1 (0x70 | (0 << 1))  //PCF8574A

//Le os valores dos pinos.
char PCF8574_Read(char address) {
    char value;
    Soft_I2C_Start();
    Soft_I2C_Write(address | 1);
    value = Soft_I2C_Read(0);
    Soft_I2C_Stop();
    return value;
}

//Define os valores dos pinos.
void PCF8574_Write(char address, char value) {
    Soft_I2C_Start();
    Soft_I2C_Write(address & 0xFE);
    Soft_I2C_Write(value);
    Soft_I2C_Stop();
}

void main() {
    char pinosDeSaida = 0, pinosDeEntrada;
    //Desativa o canal analógico.
    ADCON1 = 0x6;
    //Desativa o comparador analógico.
    CMCON = 0x07;
    //Inicializa a comunicação I2C.
    Soft_I2C_Init();

    while(1) {
        //Vamos ler o estado dos botões no dispositivo 0...
        pinosDeEntrada = PCF8574_Read(DISPOSITIVO_0);
        //E atribuir o estado do botão conectado no pino 0
        //ao led conectado no pino 4 do dispostivo 1.
        pinosDeSaida.B4 = pinosDeEntrada.B0;
        PCF8574_Write(DISPOSITIVO_1, pinosDeSaida);
        //Faz nada por 100ms.
        Delay_ms(100);
    }
}