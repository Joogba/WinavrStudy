#include <avr/io.h> 
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#define EX_LED  (*(volatile unsigned char *)0x8008)
#define EX_SS_DATA (*(volatile unsigned char *)0x8002)
#define EX_SS_SEL (*(volatile unsigned char *)0x8003)
#define CLI() cli()
#define SEI() sei()

unsigned char segment_data[11] = {0x3f, 0x06, 0x5b, 0x4f, 0x66,
								0x6d, 0x7d, 0x27, 0x7f, 0x6f, 0}; //0,1,2,3,4,5,6,7,8,9, blank
float temp_time = 0.0f;
int cur_time_arr[4] = { 1,7,5,1 };
int stop_time_arr[4] = { 0, };
int alram_time_arr[4] = { 1,8,0,0 };
int blank_arr[4] = { 10, }; // 깜빡일때 사용
int print_arr[4] = { 0,};

int alram_count = 0; // 알람 열번깜빡
int alram_state = 0; // 알람 on off
	
int setDigitCount = 0;//현재시간,알람 설정(자리수 이동)용
int setNumCount = 0; //현재시간,알람 설정(숫자 올리기)용
	
int stopwatch_stop = 0; // 0 시간흐름 1 멈춤
int state = 0;
	
int count_int;
	
unsigned char keydata, key_old;

int printDigit=0;



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
	TCCR0 = 0x05;
	TCNT0 = 0x00;
	ASSR = 0x00;
	TIMSK = 0x01;
}



// m0~s1 을 배열로만들어서 1.현재시간, 2.스탑워치 배열 2개를 만든다.
//상태 1 : 현재시간 표시 시간흐름
//상태 2 : 현재시간 조절 시간멈춤
//상태 3 : 스탑워치기능 3번버튼 누르면 0000부터 자동으로 시간이흐른다
//상태 4 : 알람 시간을 현재시간 조절과 같은방법으로 설정하고 bool 변수로 알람이 설정되면 시간은 흐르지만 led만 5번 깜빡인다.

// 1번버튼	누르면 현재시간 처음엔우선 자리수 별로 나누어 m0,1s0,1 에 넣어주고 그다음엔 m0~s1변수로 시간 올리기
// 2번버튼	시간 조절 2번누르면 시간멈추고 5번누르면 이동 1번누르거나 s1까지 입력완료시 1번상태로
//			누르면 각각 1부터4 자리수까지 숫자 올리기 1,3자리(0~5) 2,4자리(0~9) 
// 3번버튼	스탑워치
// 4번버튼	알람설정

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
	arr[3]++;

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
	
	EX_SS_SEL=~(0x02);
	EX_SS_DATA=segment_data[arr[1]];
	EX_SS_SEL=~(0x04);
	EX_SS_DATA=segment_data[arr[2]];
	EX_SS_SEL=~(0x08);
	EX_SS_DATA=segment_data[arr[3]];
}

void init_devices(void)
{ 
	CLI();
	port_init();
	timer0_init();
	MCUCR = 0x80;
	EX_SS_SEL = 0x0f; // 초기값으로 디지트 OFF
	PORTB = 0x00;
	DDRB = 0x00;
    PORTD = 0xff; // PORTD 초기값 설정	
	DDRD = 0xff; // PORTD  모두 출력으로 설정	 
	SEI();
}

ISR(TIMER0_OVF_vect) // oc0 & ctc mode ? ISR(TIMER0_COMP_vect)
{ 
	TCNT0 = 0x00; //reload counter value
	
	printDigit++;
	printDigit = printDigit %4;
	
	count_int++;
	if(count_int == 488) 
	{	
		switch (state) // 변수값 변경부
		{			
			case 0: // 현재시간
				RunningTime(cur_time_arr ,&temp_time);
				
				if (alram_state == 1 && alram_count < 5) // 열번 깜빡이고 끝
				{					
					alram_count++;					
				}
				else
				{
					alram_state = 0; alram_count = 0;
				}
				break;
			case 1: // 시간조절
				break;
			case 2: // 스탑워치
				RunningTime(cur_time_arr,&temp_time);
								
				if(stopwatch_stop == 0) 
					RunningTime(stop_time_arr,&temp_time);
								
					
				break;
			case 3: // 알람
				// 키입력으로 시간
				RunningTime(cur_time_arr,&temp_time);
				
				break;
		};
		count_int = 0;
	}
	
	switch (state) // 출력부
		{			
			case 0: // 현재시간
				TimeCopy(print_arr, cur_time_arr);
				//PrintTime(cur_time_arr);
				
				if (alram_state == 1 && alram_count < 10) // 열번 깜빡이고 끝
				{
					PrintTime(blank_arr);				
				}
				//break;
			case 1: // 시간조절
				TimeCopy(print_arr, cur_time_arr);
				//PrintTime(cur_time_arr);
				break;
			case 2: // 스탑워치
				//PrintTime(stop_time_arr);
				TimeCopy(print_arr, stop_time_arr);
					
				break;
			case 3: // 알람
				// 키입력으로 시간
				//PrintTime(alram_time_arr);
				TimeCopy(print_arr, alram_time_arr);
				break;
		};
		
	EX_SS_DATA =
	segment_data[print_arr[printDigit]];
	EX_SS_SEL = ~(0x01 << printDigit);
}


int main(void)
{
    
	init_devices();	
	
	
		
	while (1)
	{		
		keydata = PINB & 0xff;
		if (keydata != key_old)
		{
			if (keydata == 0x01) state = 0;  // 현재시간
			else if (keydata == 0x02) // 시간조절
			{
				state = 1; 
				setDigitCount = 0;
				setNumCount = 0; 
				
				
			}
			else if(key_old == 0x01 && keydata == 0x10)  //자리수변경
			{
					setDigitCount++;
					setDigitCount %= 4;
			}
			else if(key_old == 0x01 && keydata == 0x20) // 
			{					
					cur_time_arr[setDigitCount]++;
					if(setDigitCount == 0 || setDigitCount == 2)
						cur_time_arr[setDigitCount] %= 6;
					else 
						cur_time_arr[setDigitCount] %= 10;
			}
			else if (keydata == 0x04) 
			{
				state = 2; // 스탑워치
				SetTimeZero(stop_time_arr);
				
				
			}
			else if(key_old == 0x04 && keydata == 0x10 && stopwatch_stop == 0)	// 시간 가고있으면 멈춤				
					stopwatch_stop = 1;
			else if(key_old == 0x04 && keydata == 0x10 && stopwatch_stop == 1) // 안가고있으면 다시 흐름
					stopwatch_stop = 0;
			else if (keydata == 0x08) 
			{
				state = 3;// 알람설정
				TimeCopy(alram_time_arr, cur_time_arr);
				setDigitCount = 0;
				setNumCount = 0;
				
				
				
			}
			else if(key_old == 0x08 && keydata == 0x10)  //자리수변경
				{
					setDigitCount++;
					setDigitCount %= 4;
				}
				else if(keydata == 0x20) // 숫자 변경
				{
					alram_time_arr[setDigitCount]++;
					if(setDigitCount == 0 || setDigitCount == 2)
					alram_time_arr[setDigitCount] %= 6;
					else
					alram_time_arr[setDigitCount] %= 10;
				}
			
		}
		
		if (CompareTime(cur_time_arr, alram_time_arr) == 1) // 알람 on
			alram_state = 1;
		
		
		
		
		key_old = keydata;
	}
	
	return 0;
  
}
