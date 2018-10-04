/*
   MCP4922 - Conversor Digital-Analogico de 12-bits e 2 canais.

   Autor: Tiago Melo
   Blog: Microcontrolandos
   Compilador: MikroC PRO PIC
   Bibliotecas: Soft_SPI
*/

//Pinos do MCP4922.
sbit SoftSpi_SDI at RB2_bit;
sbit SoftSpi_SDO at RB2_bit;
sbit SoftSpi_CLK at RB0_bit;
sbit MCP4922_CS at RB1_bit;
sbit SoftSpi_SDI_Direction at TRISB2_bit;
sbit SoftSpi_SDO_Direction at TRISB2_bit;
sbit SoftSpi_CLK_Direction at TRISB0_bit;
sbit MCP4922_CS_Direction at TRISB1_bit;

void MCP4922_Init() {
    MCP4922_CS_Direction = 0;
    MCP4922_CS = 1;
}

void MCP4922_Write(char channel, unsigned value, char gain) {
    ((char*)&value)[1].B7 = channel; //A/B
    ((char*)&value)[1].B6 = 0; //BUF
    ((char*)&value)[1].B5 = !gain.B0; //GA
    ((char*)&value)[1].B4 = 1; //SHDN
    //Habilita a transferencia de dados.
    MCP4922_CS = 0;
    //Enviamos primeiro o byte MSB.
    Soft_SPI_Write(((char*)&value)[1]);
    Soft_SPI_Write(((char*)&value)[0]);
    //Desabilita a transferencia de dados.
    MCP4922_CS = 1;
}

void main() {
unsigned valor = 0;

   //Inicializa o MCP4922.
   Soft_SPI_Init();
   MCP4922_Init();

   while(1) {
      MCP4922_Write(0, valor, 0); //canal 0, sem ganho.
      MCP4922_Write(1, valor, 1); //canal 1, com ganho.
      //Incrementa o valor.
      valor++;
      //Máximo 12 bits.
      valor &= 0xFFF;
      //Faz nada por 100ms.
      Delay_ms(100);
   }
}