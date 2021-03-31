#include <avr/io.h> 
#include <util/delay.h>

int main(void)   // void 명시 
{	PORTD = 0xff; // PORTD 초기값 설정	
	DDRD = 0xff; // PORTD  모두 출력으로 설정	
	
	while(1) // 무한 루프(아무 의미 었음)
		{ PORTD = 0xc0; //PORTD.6, PORTD.7 LED ON	
          _delay_ms(500);
		  PORTD = 0x00; // LED OFF
		  _delay_ms(500);
		}
}

