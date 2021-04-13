#include <avr/io.h> 
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#define EX_LED  (*(volatile unsigned char *)0x8008)
#define EX_SS_DATA (*(volatile unsigned char *)0x8002)
#define EX_SS_SEL (*(volatile unsigned char *)0x8003)
#define CLI() cli()
#define SEI() sei()

float temp_time = 0.0f;
int cur_time_arr[4] = { 1,7,5,1 };
int stop_time_arr[4] = { 0, };
int alram_time_arr[4] = { 1,8,0,0 };
int blank_arr[4] = { 10, }; // �����϶� ���

int alram_count = 0; // �˶� ��������
int alram_state = 0; // �˶� on off
	
int setDigitCount = 0;//����ð�,�˶� ����(�ڸ��� �̵�)��
int setNumCount = 0; //����ð�,�˶� ����(���� �ø���)��
	
int stopwatch_stop = 0; // 0 �ð��帧 1 ����
int state = 0;
	
int count_int;
	
unsigned char keydata, key_old;


ISR(TIMER0_COMP_vect) // oc0 & ctc mode ? ISR(TIMER0_COMP_vect)
{ 
	TCNT0 = 0x00; //reload counter value
	count_int++;
	if(count_int == 488) 
	{	
		switch (state)
		{			
			case 0: // ����ð�
				RunningTime(cur_time_arr ,&temp_time);
				PrintTime(cur_time_arr);
				if (alram_state == 1 && alram_count < 10) // ���� �����̰� ��
				{
					PrintTime(blank_arr);
					alram_count++;					
				}
				else
				{
					alram_state = 0; alram_count = 0;
				}
				break;
			case 1: // �ð�����
				PrintTime(cur_time_arr);
				if((PINB & 0xff) == 0x10)  //�ڸ�������
				{
					setDigitCount++;
					setDigitCount %= 4;
				}
				else if((PINB & 0xff) == 0x20) // 
				{					
					cur_time_arr[setDigitCount]++;
					if(setDigitCount == 0 || setDigitCount == 2)
						cur_time_arr[setDigitCount] %= 6;
					else 
						cur_time_arr[setDigitCount] %= 10;
				}
				break;
			case 2: // ��ž��ġ
				RunningTime(cur_time_arr,&temp_time);
				PrintTime(stop_time_arr);
				
				if(stopwatch_stop == 0) 
					RunningTime(stop_time_arr,&temp_time);
				
				if((PINB & 0xff) == 0x10 && stopwatch_stop == 0)	// �ð� ���������� ����				
					stopwatch_stop = 1;
				else if((PINB & 0xff) == 0x10 && stopwatch_stop == 1) // �Ȱ��������� �ٽ� �帧
					stopwatch_stop = 0;
					
				break;
			case 3: // �˶�
				// Ű�Է����� �ð�
				RunningTime(cur_time_arr,&temp_time);
				PrintTime(alram_time_arr);
				if((PINB & 0xff) == 0x10)  //�ڸ�������
				{
					setDigitCount++;
					setDigitCount %= 4;
				}
				else if((PINB & 0xff) == 0x20) // ���� ����
				{
					alram_time_arr[setDigitCount]++;
					if(setDigitCount == 0 || setDigitCount == 2)
					alram_time_arr[setDigitCount] %= 6;
					else
					alram_time_arr[setDigitCount] %= 10;
				}
				break;
		};
		count_int = 0;
	}
}


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

void timer0_init(void)
{ 	
	TCCR0 = 0x07;
	TCNT0 = 0x00;
	ASSR = 0x00;
	TIMSK = 0x01;
}

unsigned char segment_data[11] = {0x3f, 0x06, 0x5b, 0x4f, 0x66,
								0x6d, 0x7d, 0x27, 0x7f, 0x6f, 0}; //0,1,2,3,4,5,6,7,8,9, blank

// m0~s1 �� �迭�θ��� 1.����ð�, 2.��ž��ġ �迭 2���� �����.
//���� 1 : ����ð� ǥ�� �ð��帧
//���� 2 : ����ð� ���� �ð�����
//���� 3 : ��ž��ġ��� 3����ư ������ 0000���� �ڵ����� �ð����帥��
//���� 4 : �˶� �ð��� ����ð� ������ ����������� �����ϰ� bool ������ �˶��� �����Ǹ� �ð��� �帣���� led�� 5�� �����δ�.

// 1����ư	������ ����ð� ó�����켱 �ڸ��� ���� ������ m0,1s0,1 �� �־��ְ� �״����� m0~s1������ �ð� �ø���
// 2����ư	�ð� ���� 2�������� �ð����߰� 5�������� �̵� 1�������ų� s1���� �Է¿Ϸ�� 1�����·�
//			������ ���� 1����4 �ڸ������� ���� �ø��� 1,3�ڸ�(0~5) 2,4�ڸ�(0~9) 
// 3����ư	��ž��ġ
// 4����ư	�˶�����

void SetTimeZero(int *arr[4])
{
	arr[0] = 0;
	arr[1] = 0;
	arr[2] = 0;
	arr[3] = 0;
}

void TimeCopy(int* arr1, int* arr2)
{
	arr1[0] = arr2[0];
	arr1[1] = arr2[1];
	arr1[2] = arr2[2];
	arr1[3] = arr2[3];
}

void RunningTime(int* arr, float *temp_time)
{		
	*temp_time += 0.05f;
	if (*temp_time  > 1.0f)
	{
		arr[3]++;
		*temp_time = 0.0f;
	}

	if (arr[3] > 9)
	{
		arr[3] = 0;
		arr[2]++;
		if (arr[2] > 5)
		{
			arr[2] = 0;
			arr[1]++;
			if (arr[1] > 9)
			{
				arr[1] = 0;
				arr[0]++;
				if (arr[0] > 5)
				arr[0] = 0;
			}
		}
	}
}

int CompareTime(int* arr1, int* arr2)
{
	for (int i = 0; i < 4; i++)
	{
		if (arr1[i] != arr2[i])
		return 0;
	}
	return 1;
}

void PrintTime(int* arr)
{
	EX_SS_SEL=~(0x01);
	EX_SS_DATA=segment_data[arr[0]];
	_delay_ms(5);
	EX_SS_SEL=~(0x02);
	EX_SS_DATA=segment_data[arr[1]];
	_delay_ms(5);
	EX_SS_SEL=~(0x04);
	EX_SS_DATA=segment_data[arr[2]];
	_delay_ms(5);
	EX_SS_SEL=~(0x08);
	EX_SS_DATA=segment_data[arr[3]];
	_delay_ms(5);
}

void init_devices(void)
{ 
	CLI();
	port_init();
	timer0_init();
	MCUCR = 0x80;
	SEI();
}

int main(void)
{
    
	init_devices();
	
	EX_SS_SEL = 0x0f; // �ʱⰪ���� ����Ʈ OFF
	PORTB = 0x00;
	DDRB = 0x00;
    PORTD = 0xff; // PORTD �ʱⰪ ����	
	DDRD = 0xff; // PORTD  ��� ������� ����	 
	
	
	
	
	
	
	EX_SS_SEL = 0x0f;
		
	while (1)
	{		
		keydata = PINB & 0xff;
		if (keydata != key_old)
		{
			if (keydata == 0x01) state = 0;  // ����ð�
			else if (keydata == 0x02) // �ð�����
			{
				state = 1; 
				setDigitCount = 0;
				setNumCount = 0; 
			}
			else if (keydata == 0x04) 
			{
				state = 2; // ��ž��ġ
				SetTimeZero(stop_time_arr);
			}
			else if (keydata == 0x08) 
			{
				state = 3;// �˶�����
				TimeCopy(alram_time_arr, cur_time_arr);
				setDigitCount = 0;
				setNumCount = 0;
			}
		}
		
		if (CompareTime(cur_time_arr, alram_time_arr) == 1) // �˶� on
			alram_state = 1;
		
		
		
		
		key_old = keydata;
	}
	
	return 0;
  
}