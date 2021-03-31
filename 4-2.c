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

    port_init();

    MCUCR = 0x80;
    PORTD = 0xff; // PORTD 초기값 설정	
	DDRD = 0xff; // PORTD  모두 출력으로 설정	   
   while (1) 
   {
     
	 while(i<256) {
	      EX_LED = i++;
		  PORTD = 0xc0&i;
	     _delay_ms(500);
		  
	    }
	 i=0;
	
   } 
  
}