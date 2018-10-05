[PCF8574](http://www.ti.com/lit/ds/symlink/pcf8574.pdf) é um expansor de portas de entrada e saída de 8 bits que se comunica com o microcontrolador através da interface I2C.

Um diferencial dele em relação ao 74HC595, é que suas portas também funcionam como entrada.

![](http://www.microcontrolandos.com.br/github/PIC_PCF8574/2.png)

Devido a interface I2C, cada dispositivo tem um endereço no barramento. No caso do PCF8574 seu endereço é `0x40` e no caso do PCF8574A é o `0x70`. Em ambos, os bits 3, 2 e 1 do endereço são controlados pelos pinos `A2`, `A1` e `A0`, respectivamente. O bit 0 tem o papel de definir se será feito uma leitura(1) ou escrita(0) no dispositivo.

### Funcionamento

**Sempre** que realizarmos a escrita no dispositivo estaremos definindo os valores dos pinos (`P0` – `P7`).

**Sempre** que realizarmos a leitura do dispositivo estaremos lendo os valores dos pinos (`P0` – `P7`).

Então:

1. Enviamos um pulso de START;
2. Enviamos um byte com o endereço do dispositivo.
3. Se estivermos querendo escrever, devemos enviar mais um byte com os valores dos pinos. Se estivermos querendo ler, devemos ler um byte pra obter os valores dos pinos. 
4. Por fim, enviamos um pulso de STOP.

> Os pinos do PCF8574 são considerados pinos Quasi-Bidirecionais, pois podem funcionar tanto como entrada quanto como saída sem a necessidade de um sinal de controle de sua direção.

### Código-Fonte

```c
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
```

### Download

[Baixe o projeto aqui!](http://www.microcontrolandos.com.br/link/microcontrolandos-github)