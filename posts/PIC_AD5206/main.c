/*
   AD5206 - Potenciometro Digital de 6 canais.

   Autor: Tiago Melo
   Blog: Microcontrolandos
   Compilador: MikroC PRO PIC
   Bibliotecas: Soft_SPI
*/

//Pinos do AD5206.
sbit SoftSpi_SDI at RB1_bit;
sbit SoftSpi_SDO at RB1_bit;
sbit SoftSpi_CLK at RB0_bit;
sbit SoftSpi_SDI_Direction at TRISB1_bit;
sbit SoftSpi_SDO_Direction at TRISB1_bit;
sbit SoftSpi_CLK_Direction at TRISB0_bit;
sbit AD5206_CS at RB2_bit;
sbit AD5206_CS_Direction at TRISB2_bit;

void AD5206_Init() {
    //Inicia o pino CS como saída e valor inicial = 1.
    AD5206_CS_Direction = 0;
    AD5206_CS = 1;
}

void AD5206_Write(char canal, char valor) {
    //Envia 16 bits (somente os últimos 11 bits serão utilizados).
    AD5206_CS = 0;
    Soft_SPI_Write(canal);
    Soft_SPI_Write(valor);
    AD5206_CS = 1;
}

void main() {
char canal= 0;
char valor = 0;
   //Inicia a comunicação SPI.
   Soft_SPI_Init();
   //Inicializa o AD5206.
   AD5206_Init();

   while(1) {
       //Define o valor do potenciômetro de um canal específico.
       AD5206_Write(canal, valor++);
       //Faz nada por 1s.
       Delay_ms(100);
   }
}