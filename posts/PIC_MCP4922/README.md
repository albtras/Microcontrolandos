[MCP4922](http://ww1.microchip.com/downloads/en/DeviceDoc/22250A.pdf) é um conversor digital-analógico (DAC) de 12-bits da Microchip.

### Características

* Tensão de operação: 2.7V até 5.5V;
* Comunicação via SPI;
* Resolução de 12-bits;
* 2 canais de saída;
* Ganho de saída (1x ou 2x).

### Funcionamento

O MCP4922 converte um valor digital de 12 bits em um valor de tensão de `0V` até `VREF`. Ainda é possível ter um ganho de saída de `2 * VREF`. Cada canal possui um pino `VREF` externo.

O pino `SHDN` tem a finalidade de colocar ambas as saídas no modo *shutdown*. Também é possível configurá-las individualmente junto com os 16 bits enviados. No modo *shutdown* a maioria dos circuitos internos são desligados e a saída é configurada para ter uma resistência de 500k.

O cálculo para a tensão de saída é: `((VREF * Valor) / 4096) * Ganho`.

Para definir o valor de um determinado canal:

* Definimos o pino `CS` com nível lógico baixo para habilitar a transmissão dos dados;
* Enviamos 16 bits: 

![image](https://github.com/tiagohm/Microcontrolandos/blob/master/PIC_MCP4922/2.png?raw=true)

* O bit 15 (A/B) seleciona o canal: `0` é o canal A e `1` é o canal B;
* O bit 13 (GA) indica o ganho de saída: `0` = ganho de 2x e `1` = não há ganho;
* O bit 12 (SHDN) define se o canal selecionado será colocado no modo *shutdown*. `0` = modo shutdown e `1` = modo normal;
* Os bit 11-0 (D11-D0) sãos os 12 bits que definem o valor de saída do canal selecionado.
* Definimos o pino `CS` com nível lógico alto para finalizar a transmissão dos dados.

>  Um projeto legal que poderíamos montar é um player de músicas no formato WAV (Mono ou Stereo) lidos de um SD Card.

### Código-Fonte

```c
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
```

### Download

[Baixe o projeto aqui!](https://github.com/tiagohm/Microcontrolandos/tree/master/PIC_MCP4922)