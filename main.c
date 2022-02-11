/*-------------------------------------------------------------------------
					Technika Mikroprocesorowa 2 - Projekt: Vending machine
					autorzy: Herman Jaroslaw, Rosol Jan
----------------------------------------------------------------------------*/
					
#include "MKL05Z4.h"
#include "ADC.h"
#include "DAC.h"
#include "frdm_bsp.h"
#include "lcd1602.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RED_LED_POS 8
#define GREEN_LED_POS 9
#define BLUE_LED_POS 10 // definicja pinow diod LED

#define BUTTON_1_POS 1
#define BUTTON_2_POS 2
#define BUTTON_3_POS 7 // definicja przyciskow


void delay_ms( int n) {
volatile int i;
volatile int j;
		for( i = 0 ; i < n; i++)
		for(j = 0; j < 3500; j++) {}
} // funkcja realizujaca opoznienie programu


float volt_coeff = ((float)(((float)2.91) / 4095) ); // stala napieciowa
char display[] = {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
uint8_t wynik_ok = 0;
uint16_t temp;
float wynik;
int i = 0; // iterator

void ADC0_IRQHandler()
{	
	temp = ADC0->R[0];		// Odczyt danej i skasowanie flagi COCO
	
	if(!wynik_ok)					// Sprawdź, czy wynik skonsumowany przez petlę główną
	{
		wynik = temp;				// Wyślij wynik do pętli głównej
		wynik_ok=1;
	}
	
	DAC_Load_Trig(temp);	// Załadowanie nowej danej i wyzwolenie przetwornika C/A
	NVIC_ClearPendingIRQ(ADC0_IRQn);
}

void obsluga(int zeton){
	while(1)
	{
		if(wynik_ok){
			wynik = wynik*volt_coeff;			// Dostosowanie wyniku do zakresu napięciowego
			if (wynik>=0.1){
				delay_ms(500);
				LCD1602_ClearAll();
				LCD1602_Print("Wrzuc zeton");
			}
			else if (wynik<0.1){
				LCD1602_ClearAll();
				LCD1602_Print("Przetwarzanie...");
				delay_ms(3000);
				i++;
			}
			wynik_ok=0;
			if (i>zeton-1){
				if (i == 1){
				PTB->PTOR|=(1<<RED_LED_POS); /* Wlaczenie czerwonej diody */
				LCD1602_ClearAll();
				LCD1602_SetCursor(3,0);
				LCD1602_Print("Wydawanie");
				LCD1602_SetCursor(3,1);
				LCD1602_Print("produktu...");
				delay_ms(10000);
				PTB->PSOR|=(1<<RED_LED_POS); /* Wylaczenie czerwonej diody */
				LCD1602_ClearAll();
				LCD1602_Print("Wybierz produkt");
				i=0;
				break;
				}
				else if (i == 2){
				PTB->PTOR|=(1<<GREEN_LED_POS); /* Wlaczenie zielonej diody */
				LCD1602_ClearAll();
				LCD1602_SetCursor(3,0);
				LCD1602_Print("Wydawanie");
				LCD1602_SetCursor(3,1);
				LCD1602_Print("produktu...");
				delay_ms(10000);
				PTB->PSOR|=(1<<GREEN_LED_POS); /* Wylaczenie zielonej diody */
				LCD1602_ClearAll();
				LCD1602_Print("Wybierz produkt");
				i=0;
				break;
				}
				else if (i == 3){
				PTB->PTOR|=(1<<BLUE_LED_POS); /* Wlaczenie niebieskiej diody */
				LCD1602_ClearAll();
				LCD1602_SetCursor(3,0);
				LCD1602_Print("Wydawanie");
				LCD1602_SetCursor(3,1);
				LCD1602_Print("produktu...");
				delay_ms(10000);
				PTB->PSOR|=(1<<BLUE_LED_POS); /* Wylaczenie niebieskiej diody */
				LCD1602_ClearAll();
				LCD1602_Print("Wybierz produkt");
				i=0;
				break;
				}
			}
		}
	}
}

int main(){
	
	uint8_t	kal_error;
	kal_error=ADC_Init();
	
	if(kal_error)
	{
		while(1);							// Kalibracja się nie powiodła
	}
	
	DAC_Init();		// Inicjalizacja przetwornika C/A
	ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(12);		// Pierwsze wyzwolenie przetwornika ADC0 w kanale 12 i odblokowanie przerwania
	
	
	
	LCD1602_Init();		 // Inicjalizacja wyświetlacza LCD
	LCD1602_Backlight(TRUE);

	LCD1602_Print("Wybierz produkt");
	
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(1); 
	PORTB->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(1);
	PORTB->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(1); 
	
	PORTB->PCR[BUTTON_1_POS] |= PORT_PCR_MUX(1); 
	PORTB->PCR[BUTTON_2_POS] |= PORT_PCR_MUX(1); 
	PORTB->PCR[BUTTON_3_POS] |= PORT_PCR_MUX(1); 
		
	PORTB->PCR[BUTTON_1_POS] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; 
	PORTB->PCR[BUTTON_2_POS] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; 
	PORTB->PCR[BUTTON_3_POS] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; 

	PTB->PDDR |= (1<<RED_LED_POS); 
	PTB->PSOR |= (1<<RED_LED_POS);
	
	PTB->PDDR |= (1<<GREEN_LED_POS);
	PTB->PSOR |= (1<<GREEN_LED_POS);
	
	PTB->PDDR |= (1<<BLUE_LED_POS);
	PTB->PSOR |= (1<<BLUE_LED_POS);
	
	
	while(1)
	{
					if ( ( PTB->PDIR & (1<<BUTTON_2_POS) ) ==0 ){ /* Sprawdzanie czy przycisk zostal wcisniety */
							LCD1602_ClearAll();
							LCD1602_SetCursor(3,0);
							LCD1602_Print("Wybrales");
							LCD1602_SetCursor(3,1);
							LCD1602_Print("produkt 2");
							delay_ms(2000);
							LCD1602_ClearAll();
							LCD1602_SetCursor(3,0);
							LCD1602_Print("Cena:");
							LCD1602_SetCursor(3,1);
							LCD1602_Print("2 zetony");
							delay_ms(2000);
							while( ( PTB->PDIR & (1<<BUTTON_2_POS) ) == 0 )
									delay_ms(100);
							obsluga(2);
						}
					
					else if ( ( PTB->PDIR & (1<<BUTTON_3_POS) ) ==0 ){ /* Sprawdzanie czy przycisk zostal wcisniety */
							LCD1602_ClearAll();
							LCD1602_SetCursor(3,0);
							LCD1602_Print("Wybrales");
							LCD1602_SetCursor(3,1);
							LCD1602_Print("produkt 3");
							delay_ms(2000);
							LCD1602_ClearAll();
							LCD1602_SetCursor(3,0);
							LCD1602_Print("Cena:");
							LCD1602_SetCursor(3,1);
							LCD1602_Print("3 zetony");
							delay_ms(2000);
							while( ( PTB->PDIR & (1<<BUTTON_3_POS) ) == 0 )
									delay_ms(100);
							obsluga(3);
						}
	}
}
