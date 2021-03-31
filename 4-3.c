#include <avr/io.h> 
#include <util/delay.h>


#define EX_LED  (*(volatile unsigned char *)0x8008)



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

int main(void)
{
    int i=0;
	unsigned char keydata;


    port_init();

    MCUCR = 0x80;
    PORTD = 0xff; // PORTD 초기값 설정	
	DDRD = 0xff; // PORTD  모두 출력으로 설정	
	PORTB = 0x00;
	DDRB  = 0x00;
   
   while (1) 
   {
         //EX_LED = i++;
		//PORTD = 0xc0&i;
			
		keydata = PINB;
		PORTD = (keydata<<3);
		
	    //_delay_ms(500);
	    switch(keydata) {
			case 0x01: EX_LED = 0x01;
				_delay_ms(500);
				break;
			case 0x02: EX_LED = 0x02;
				_delay_ms(500);
				break;
			case 0x04: EX_LED = 0x04;
				_delay_ms(500);
				break;
			case 0x08: EX_LED = 0x08;
				_delay_ms(500);
				break;
			case 0x10: EX_LED = 0x10;
				_delay_ms(500);
				break;
			case 0x20: EX_LED = 0x20;
				_delay_ms(500);
				break;
			case 0x40: EX_LED = 0x40;
				_delay_ms(500);
				break;
			case 0x80: EX_LED = 0x80;
				_delay_ms(500);
				break;
			default:
				break;
			}
			 
	   
	
   } 
  
}