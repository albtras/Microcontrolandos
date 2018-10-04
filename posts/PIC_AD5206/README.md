[AD5206](http://www.analog.com/media/en/technical-documentation/data-sheets/AD5204_5206.pdf) é um potenciômetro digital de 6 canais.

### Características

- Comunicação via SPI;
- 256 posições;
- Tensão de operação: 2.7V à 5.5V.

### Funcionamento

O AD5206 possui 6 canais. Cada canal contém um potênciômetro de `10k`, `50k` ou `100k`. Cada potênciômetro possui três terminais: 2 fixos representados pelas letras `A` e `B` e um móvel representado pela letra `W`.

Para definir o valor do potenciômetro de um determinado canal, devemos enviar pelo menos `11 bits` via comunicação SPI. Se mais de 11 bits forem enviados, somente os últimos 11 bits serão utilizados.

![](http://www.microcontrolandos.com.br/github/PIC_AD5206/2.png)

- Os três bits mais significativos definem o endereço de um canal (de 0 à 5);
- Os 8 bits menos significativos definem o valor do potenciômetro (de 0 à 255).

Antes de iniciar a transmissão dos dados é necessário colocar o pino `CS` em nível lógico baixo e logo após o fim da transmissão, colocar em nível lógico alto, assim os 11 bits enviados serão armazenados num registrador interno e usados por um decodificador para selecionar o canal e definir o valor do potenciômetro.

### Código-Fonte

```c
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
    //Inicializa a comunicação SPI.
    Soft_SPI_Init();
    //Inicializa o AD5206.
    AD5206_Init();

    while(1) {
        //Define o valor do potenciômetro de um canal específico.
        AD5206_Write(canal, valor++);
        //Faz nada por 100ms.
        Delay_ms(100);
    }
}
```

### Download

[Baixe o projeto aqui!](http://www.microcontrolandos.com.br/link/microcontrolandos-github)