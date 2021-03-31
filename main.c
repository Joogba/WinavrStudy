#include <stdio.h>
#include <avr/io.h> 
#include <util/delay.h>
#include <stdbool.h>

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
	unsigned char keydata, led_data, key_old, direction;
	
	led_data = 0x01;
	key_old = 0x00;
	direction = 0;	
	

    port_init();

	PORTB = 0x00;
	DDRB = 0x00;
	MCUCR = 0x80;
    PORTD = 0xff; // PORTD ÃÊ±â°ª ¼³Á¤	
	DDRD = 0xff; // PORTD  ¸ðµÎ Ãâ·ÂÀ¸·Î ¼³Á¤	   
	
	while (1) 
   {
		keydata = PINB & 0xff;
		if (keydata != key_old)
		{ 
			if (keydata == 0x02) direction = 0;  // ÁÂ -> ¿ì
			else if (keydata == 0x04) direction = 1; // ¿ì -> ÁÂ
			else if (keydata == 0x80) direction = 2; // ±ôºý
			else if (key_old == 0x80 && keydata == 0x00) direction = 3;// ²¨Áü
		}
		
		switch(direction)
		{
			case 0:
				led_data = led_data >>1;
				if (led_data == 0x00) 
					led_data = 0x80; 
				break;
				
			case 1:
				led_data = led_data << 1;
				if (led_data == 0x00) 
					led_data = 0x01; 
				break;
				
			case 2:
				if( led_data == 0x00)
					led_data = 0xff;
				else
					led_data = 0x00;
				break;
				
			case 3:
				led_data = 0x00;
				break;
				
			default:
				break;
		
		}
		
		EX_LED = led_data;
		_delay_ms(500);
		key_old = keydata;
	}   
	
	return 0;
  
}