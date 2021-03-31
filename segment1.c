#include <stdio.h>
#include <avr/io.h> 
#include <util/delay.h>
#include <stdbool.h>

#define EX_LED  (*(volatile unsigned char *)0x8008)
#define EX_SS_DATA (*(volatile unsigned char *)0x8002)
#define EX_SS_SEL (*(volatile unsigned char *)0x8003)


void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0xff;
 
 PORTB = 0x00;
 DDRB  = 0x00;
 
 PORTC = 0x00; //m103 output only
 DDRC  = 0x03;
 
 PORTD = 0x00;
 DDRD  = 0x00;
 
 PORTE = 0x00;
 DDRE  = 0x00;
 
 PORTF = 0x00;
 DDRF  = 0x00;
 
 PORTG = 0x00;
 DDRG  = 0x03;
}

unsigned char segment_data[11] = {0x3f, 0x06, 0x5b, 0x4f, 0x66,
								0x6d, 0x7d, 0x27, 0x7f, 0x6f, 0}; //0,1,2,3,4,5,6,7,8,9, blank
unsigned char display_num[17] = {10,10,10,2,0,1,7,1,5,4,0,0,9,10,10,10,10};


int main(void)
{
    port_init();

	EX_SS_SEL = 0x0f; // 초기값으로 디지트 OFF
	MCUCR = 0x80;
    PORTD = 0xff; // PORTD 초기값 설정	
	DDRD = 0xff; // PORTD  모두 출력으로 설정	   
	
	unsigned char scroll, idx;
	scroll = 0;
	idx = 0;
	
	while (1) 
	{
		EX_SS_SEL = 0xfe;
		EX_SS_DATA = segment_data[display_num[idx%17]];
		_delay_ms(5);
		EX_SS_SEL = 0xfd;
		EX_SS_DATA = segment_data[display_num[idx+1 %17]];
		_delay_ms(5);
		EX_SS_SEL = 0xfb;
		EX_SS_DATA = segment_data[display_num[idx+2 %17]];
		_delay_ms(5);
		EX_SS_SEL = 0xf7;
		EX_SS_DATA = segment_data[display_num[idx+3 %17]];
		_delay_ms(5);
		
		scroll++;
		if (scroll == 25) {
			idx++;
			if (idx == 13)
				idx = 0;
			scroll = 0;
		}   
	
	}
	return 0;
  
}