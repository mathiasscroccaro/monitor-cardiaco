/*
 * main.c
 *
 * Created: 11/02/2017 18:08:10
 * Author : User
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

/* 

Configurações iniciais:

- Retirar o fuse que divide o clock por 8
- Considera-se um cristal de 20Mhz

*/

#define buffer_tam 10

unsigned short temporizador[2];

ISR(TIMER0_COMPA_vect)
{
	char i;
	for (i=0;i<2;i++)
		if (temporizador[i] > 0)
			temporizador[i]--;
}

void configura_portas()
{
	DDRD |= (1<<DDD3) | (1<<DDD4);
}

void configura_uart()
{
	// Setando o baud rate para clock externo de 20MH
	UBRR0H = 0;
	UBRR0L = 0;	
	UBRR0L = 4;	//250k bits/s
	
	// Habilitando Tx e Rx uart
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	
	// 8bits
	UCSR0C = (1<<UCSZ00) | (1<<UCSZ01);
}

void configura_spi()
{
	PRR |= (0<<PRSPI);
	
	// Configura o pino do portb como saida para Slave Select (SS)
	DDRB |= (1<<DDB5) | (1<<DDB3) | (1<<DDB2) | (1<<DDB0);
	
	// Configura SPI: habilita spi; modo mestre; clock on rise; freq = fosc/64
	SPCR |= (1<<SPE) | (1<<MSTR) | (1<<CPOL) | (1<<SPR1);
}

void configura_timer()
{
	TIMSK0	= (1<<OCIE0A);	// Habilita interrupção de comparação do Timer0
	TCCR0A	= (1<<WGM01);	// Modo comparação e clean
	TCCR0B	= (1<<CS01);	// f_prescaler = 20M/8 = 2.5MHz
	OCR0A	= 250;			// Configura valor da comparação do timer = 100us
}

struct b
{	
	unsigned short derivacao_1[buffer_tam];
	unsigned short derivacao_2[buffer_tam];
	unsigned short derivacao_3[buffer_tam];
	unsigned char tamanho;			
};

int main(void)
{
	
	unsigned char temp,temp1,temp2,temp3;
	
	unsigned char fLeitura = 0;
	unsigned char contador = 0;
	
	struct b buffer[2];

	unsigned char amostraMaquina = 0;
	unsigned char comunicaMaquina = 0;
	
	unsigned char leitura[3];
	
	unsigned char escrevendoNoBuffer = 0;
	unsigned char lendoNoBuffer = 0;
	
	unsigned char indexEscrita = 0;
	unsigned char indexLeitura = 0;
	
	buffer[0].tamanho = 0;
	buffer[1].tamanho = 0;
	
	configura_portas();
	configura_spi();
	configura_timer();
	configura_uart();
	sei();
	
    while (1) 
    {
		if ((temporizador[0] == 0 || fLeitura == 1) && temporizador[1] == 0) // amostragem
		{
			
			switch (amostraMaquina)
			{
				case 0:
					PORTB &= ~(1<<PORTB2);
					for (temp = 0;temp < 10; temp++) {}
					SPDR = 0x00;
					amostraMaquina++;
					fLeitura = 1;
					temporizador[0] = 20;
					break;
				case 1:
					if (!(SPSR & (1<<SPIF)))
						continue;
					else
					{
						leitura[0] = SPDR;
						SPDR = 0x00;
						amostraMaquina++;
					}
					break;
				case 2:
				if (!(SPSR & (1<<SPIF)))
						continue;
					else
					{
						leitura[1] = SPDR;
						SPDR = 0x00;
						amostraMaquina++;
					}
					break;
				case 3:
					if (!(SPSR & (1<<SPIF)))
						continue;
					else
					{
						PORTB |= (1<<PORTB2);
						
						PORTD |= (1<<PORTD4);	// S1 = 1
						PORTD &= ~(1<<PORTD3);	// S0 = 0
												
						leitura[2] = SPDR;
														
						temp1 =  ((leitura[0])<<7) | ((leitura[1])>>1) ;
						temp2 =  ((leitura[1])<<7) | ((leitura[2])>>1) ;
						
						buffer[escrevendoNoBuffer].derivacao_1[indexEscrita] = (temp1<<8) | temp2;
						
						temporizador[1] = 3;
						
						amostraMaquina++;
					}
					break;
				case 4:
					PORTB &= ~(1<<PORTB2);
					for (temp = 0;temp < 10; temp++) {}
					SPDR = 0x00;
					amostraMaquina++;
					break;
				case 5:
					if (!(SPSR & (1<<SPIF)))
						continue;
					else
					{
						leitura[0] = SPDR;
						SPDR = 0x00;
						amostraMaquina++;
					}
					break;
				case 6:
					if (!(SPSR & (1<<SPIF)))
						continue;
					else
					{
						leitura[1] = SPDR;
						SPDR = 0x00;
						amostraMaquina++;
					}
					break;
				case 7:
				if (!(SPSR & (1<<SPIF)))
					continue;
				else
				{
					PORTB |= (1<<PORTB2);
					
					PORTD &= ~(1<<PORTD3);	// S0 = 0
					PORTD &= ~(1<<PORTD4);	// S0 = 0
										
					leitura[2] = SPDR;
										
					temp1 =  ((leitura[0])<<7) | ((leitura[1])>>1) ;
					temp2 =  ((leitura[1])<<7) | ((leitura[2])>>1) ;
					
					buffer[escrevendoNoBuffer].derivacao_2[indexEscrita] = (temp1<<8) | temp2;
					
					temporizador[1] = 3;				
					
					amostraMaquina++;
				}
				break;
				case 8:
					PORTB &= ~(1<<PORTB2);
					for (temp = 0;temp < 10; temp++) {}
					SPDR = 0x00;
					amostraMaquina++;
					break;
				case 9:
					if (!(SPSR & (1<<SPIF)))
						continue;
					else
					{
						leitura[0] = SPDR;
						SPDR = 0x00;
						amostraMaquina++;
					}
					break;
				case 10:
					if (!(SPSR & (1<<SPIF)))
						continue;
					else
					{
						leitura[1] = SPDR;
						SPDR = 0x00;
						amostraMaquina++;
					}
					break;
				case 11:
					if (!(SPSR & (1<<SPIF)))
						continue;
					else
					{
						PORTB |= (1<<PORTB2);
						
						PORTD |= (1<<PORTD3);	// S0 = 1
						PORTD &= ~(1<<PORTD4);	// S1 = 0
						
						leitura[2] = SPDR;
										
						temp1 =  ((leitura[0])<<7) | ((leitura[1])>>1) ;
						temp2 =  ((leitura[1])<<7) | ((leitura[2])>>1) ;
					
						buffer[escrevendoNoBuffer].derivacao_3[indexEscrita++] = (temp1<<8) | temp2;
						buffer[escrevendoNoBuffer].tamanho++;
						
						if (indexEscrita >= buffer_tam)
						{
							if (escrevendoNoBuffer)
								escrevendoNoBuffer = 0;
							else
								escrevendoNoBuffer = 1;
							indexEscrita = 0;
						}					
					
						amostraMaquina = 0;
						fLeitura = 0;
					}
					break;
			}				
		}
		
		if (UCSR0A & (1<<UDRE0) )
		{
			switch (comunicaMaquina)
			{
				case 0:
					if (buffer[0].tamanho >= buffer_tam)
					{
						lendoNoBuffer = 0;
						comunicaMaquina++;
					}
					else if (buffer[1].tamanho >= buffer_tam)
					{
						lendoNoBuffer = 1;
						comunicaMaquina++;
					}
					else
					{}						
					break;
				case 1:
					temp = (buffer[lendoNoBuffer].derivacao_1[indexLeitura]>>8);
					UDR0 = temp;
					comunicaMaquina++;
					break;
				case 2:
					temp = buffer[lendoNoBuffer].derivacao_1[indexLeitura];
					UDR0 = temp;
					comunicaMaquina++;
					break;
				case 3:
					temp = (buffer[lendoNoBuffer].derivacao_2[indexLeitura]>>8);
					UDR0 = temp;
					comunicaMaquina++;
					break;
				case 4:
					temp = buffer[lendoNoBuffer].derivacao_2[indexLeitura];
					UDR0 = temp;
					comunicaMaquina++;
					break;
				case 5:
					temp = (buffer[lendoNoBuffer].derivacao_3[indexLeitura]>>8);
					UDR0 = temp;
					comunicaMaquina++;
					break;
				case 6:
					temp = buffer[lendoNoBuffer].derivacao_3[indexLeitura++];
					UDR0 = temp;
					if (indexLeitura == buffer_tam)
					{
						comunicaMaquina++;
					}
					else
					{
						comunicaMaquina = 1;
					}
					break;
				case 7:
					UDR0 = '\r';
					comunicaMaquina++;
					break;
				case 8:
					UDR0 = '\r';
					comunicaMaquina=0;
					indexLeitura = 0;
					buffer[lendoNoBuffer].tamanho = 0;
					break;
			}							
		}
    }
}

