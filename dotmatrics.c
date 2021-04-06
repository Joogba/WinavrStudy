#include <avr/io.h> 
#include <util/delay.h>
#include <stdbool.h>


#define EX_DM_DATA (*(volatile unsigned int *)0x8006)
#define EX_DM_SEL (*(volatile unsigned int *)0x8004)


#define EX_SS_DATA (*(volatile unsigned char *)0x8002)
#define EX_SS_SEL (*(volatile unsigned char *)0x8003)

unsigned char seg_dmi = 0, seg_scroll = 0, seg_idx=0;
unsigned char dot_dmi = 0, dot_scroll = 0, dot_idx=0;
\
\
int name_data_f[50] = {
						0,0,0,0,0,0,0,0,0,0,
						0x0f2, 0x012,0x012,0x012, 0, 0x01e,0x012,0x012,0x01e,0,		// ��
						0x022, 0x06e, 0x0a2, 0x122, 0, 0x038, 0x044, 0x044, 0x038, 0,	// ��
						0, 0x1fe, 0x048, 0x084, 0x102, 0, 0x1fe, 0x010, 0x010, 0,		// ��
						0,0,0,0,0,0,0,0,0,0,
					};


unsigned char segment_data[11] = {0x3f, 0x06, 0x5b, 0x4f, 0x66,
								0x6d, 0x7d, 0x27, 0x7f, 0x6f, 0};
								
unsigned char display_num[17] = {10,10,10,2,0,1,7,1,5,4,0,0,9,10,10,10,10}; // �й�

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

void dot_matrix(void)
{
	EX_DM_SEL = (1<<dot_dmi);
	EX_DM_DATA = name_data_f[dot_dmi+dot_idx];
	dot_dmi++;
	if (dot_dmi>9)
	{
		dot_dmi = 0;
		dot_scroll++;
		if (dot_scroll > 300) {
			dot_scroll = 0;
			dot_idx++;
			if (dot_idx > 40)
				dot_idx = 0;
		}
	}
	
	_delay_ms(10);
}

void print_segment(void)
{
	EX_SS_SEL = 0xfe;
		EX_SS_DATA = segment_data[display_num[seg_idx %17]];
		_delay_ms(5);
		EX_SS_SEL = 0xfd;
		EX_SS_DATA = segment_data[display_num[seg_idx+1 %17]];
		_delay_ms(5);
		EX_SS_SEL = 0xfb;
		EX_SS_DATA = segment_data[display_num[seg_idx+2 %17]];
		_delay_ms(5);
		EX_SS_SEL = 0xf7;
		EX_SS_DATA = segment_data[display_num[seg_idx+3 %17]];
		_delay_ms(5);
		
		seg_scroll++;
		if (seg_scroll == 25) {
			seg_idx++;
			if (seg_idx == 13)
				seg_idx = 0;
			seg_scroll = 0;
		}   
}

int main(void)
{
	port_init();
	
	EX_SS_SEL = 0x0f; // �ʱⰪ���� ����Ʈ OFF
	MCUCR = 0x80;
	PORTB = 0x00;
	DDRB = 0x00;
    PORTD = 0xff; // PORTD �ʱⰪ ����	
	DDRD = 0xff; // PORTD  ��� ������� ����	 
	
	int state = 0;
	unsigned char keydata, key_old;
	
	
	while(1)
	{
		keydata = PINB & 0xff;
		if (keydata != key_old)
		{
			if (keydata == 0x01) // �̸�
			{
				state = 0;
			}
			else if (keydata == 0x02) // �й� 
			{
				state = 1; 
			}
			else if (keydata == 0x04)  // �Ѵ� 
			{
				state = 2; 
			}
		}
	
	
		switch(state)
		{
		case 0:
			dot_matrix(); //�� ��Ʈ���� ���
			
			break;
		case 1:
			print_segment();
			
			break;
		case 2:
			dot_matrix(); //�� ��Ʈ���� ���
			print_segment();
			break;
		default:
			break;
		}
		
		key_old = keydata;
	}

	return 0;
}
