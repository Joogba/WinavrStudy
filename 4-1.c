#include <avr/io.h> 
#include <util/delay.h>

int main(void)   // void ��� 
{	PORTD = 0xff; // PORTD �ʱⰪ ����	
	DDRD = 0xff; // PORTD  ��� ������� ����	
	
	while(1) // ���� ����(�ƹ� �ǹ� ����)
		{ PORTD = 0xc0; //PORTD.6, PORTD.7 LED ON	
          _delay_ms(500);
		  PORTD = 0x00; // LED OFF
		  _delay_ms(500);
		}
}

