#include "stc12c5608ad.h"
#include "funtions.h"
#include <intrins.h>
volatile int trigger_logic = 1;   //1:positive trigger   0: negtive trigger
WORD time_seq[]={100,200,400,600,800};
volatile BYTE rise_time_index = 1;
volatile BYTE fall_time_index = 1;

void main()
{
	Init_PCA();
	Init_Timer0();
	Init_GPIO();
	Init_UART();
	SendString("use r[x]f[y] to set rise and fall time.\n each time have 5 levels, for example, \n if you want to set rise time level 2 and fall time level 3, use: r2f3\n");
	rise_time_index = ISPReadByte(ISP_ADDRESS);
	fall_time_index = ISPReadByte(ISP_ADDRESS+1);
	if (rise_time_index == 0xff || fall_time_index == 0xff || rise_time_index > 0x04 ||  rise_time_index > 0x04){
		rise_time_index = 1;
		fall_time_index = 1;
	}
	
	while(1)
	{
		if(P32 == 0){
			trigger_logic = 0;
		}
		else{
			trigger_logic = 1;
		}
		if(P33==0)
		{
			debounce();
			if(P33==0)
			{
				direction = (trigger_logic ==1 )?0:1;
			}
		}
		if(P33==1)
		{
			debounce();
			if(P33==1)
			{
				direction = (trigger_logic ==1 )?1:0;
			}
		}
	}
}

void debounce()    //20ms delay
{
	unsigned char i, j, k;
	i = 2;
	j = 103;
	k = 147;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
 
void Init_GlobalVariables()
{
}

void Init_GPIO()
{
  //P3M0 = 0x03;
	//P3M1 = 0x0f; 
  P3M0 = 0x00; 
	P3M1 = 0x0c; 

	//P0M0 |= 0x01;
	//P0M1 |= 0x00;   //P01 input
}

void Init_PCA()
{
	CMOD=0x04;   //timer0 overflow
	CCON=0x00;
	CCAPM0=0x42;
	CCAPM1=0x42;
	CCAPM2=0x42;
	CCAPM3=0x42;
	CL=CH=0;
	CCAP0L=CCAP0H=0;
	CCAP1L=CCAP1H=0;
	CCAP2L=CCAP2H=0;
	CCAP3L=CCAP3H=0;
	CCAP0H=0x90;
	CCAP1H=0x90;
	CCAP2H=0x90;
	CCAP3H=0x90;
	CR=1;
}

void Init_UART()
{
	PCON &= 0x7F;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器时钟1T模式
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设置定时器模式
	TMOD |= 0x20;		//设置定时器模式
	TL1 = 0xD9;		//设置定时初始值
	TH1 = 0xD9;		//设置定时重载值
	ET1 = 0;		//禁止定时器%d中断
	TR1 = 1;		//定时器1开始计时
	ES = 1;
}
	
void SendData(BYTE dat)
{
	SBUF=dat;
	while(TI==0);
	TI=0;
}

void SendString(BYTE *ps)
{
  EA= 0;
  while((*ps)!='\0')
  {
		SBUF= *ps;
    while(TI==0);
    TI= 0;
    ps++;
  }
    EA= 1;
}

void ClearRX_BUF()
{
	BYTE i=0;
	for (i=0; i<BUF_LEN; i++)
	{
		RX_BUFFER[i]=0;
	}
}

void Init_Timer0()
{
	 // AUXR = AUXR|0x80;
  TMOD |= 0x01;
  TH0 = 0xFF;
  TL0 = 0xEE;
  EA = 1;
  ET0 = 1;
  TR0 = 1;
}

void Timer0Interrupt(void) interrupt 1
{
	rise_counter++;
	fall_counter++;
	test_counter++;
	if(fall_counter>time_seq[fall_time_index] && direction == 0)
	{
		fall_counter=0;
		if(CCAP1H<0xf9)
		{
			CCAP1H+=0x02;
		}
		else{
			CCAP1H=0xff;
			PCA_PWM1=0x03;
		}
		if(CCAP0H<0xf9)
		{
			CCAP0H+=0x02;
		}
		else{
			CCAP0H=0xff;
			PCA_PWM0=0x03;
		}
	}
	
		if(rise_counter>time_seq[rise_time_index] && direction == 1)
	{
		rise_counter=0;
		PCA_PWM1=0x00;

		if(CCAP1H == 0){
			CCAP1H = 0;
		}
		else if(CCAP1H >= 0x01 && CCAP1H<0x13)
		{
			CCAP1H-=1;
		}
		else if(CCAP1H >= 0x13)
		{
			CCAP1H-=2;
		}
		if(CCAP1H < 0x40){
			PCA_PWM0=0x00;
			if(CCAP0H == 0){
				CCAP0H = 0;
			}
			else if(CCAP0H >= 0x01 && CCAP0H<0x23)
			{
				CCAP0H-=1;
			}
			else if(CCAP0H >= 0x23)
			{
				CCAP0H-=2;
			}
		}	
	}
	if(test_counter>10000){
		test_counter=0;
	}
	TH0 = 0x0FF;
  TL0 = 0x0EE;
}

void UARTInterrupt(void) interrupt 4
{
	if(RI){
		static BYTE BUF_COUNTER=0;
		
			if (BUF_COUNTER<BUF_LEN)
			{
				RX_BUFFER[BUF_COUNTER]=SBUF;
				BUF_COUNTER++;
			}

			if(BUF_COUNTER >= BUF_LEN) 	//处理数据帧
			{
				BUF_COUNTER = 0;
				//dealing with codes
				if(RX_BUFFER[0]==0x72 && RX_BUFFER[2]==0x66){          // rxfx 格式
					if(RX_BUFFER[1]>=0x31 && RX_BUFFER[1]<=0x35 && RX_BUFFER[3]>=0x31 && RX_BUFFER[3]<=0x35){
						if(rise_time_index != RX_BUFFER[1]-0x31 || fall_time_index != RX_BUFFER[3]-0x31 ){
							rise_time_index = RX_BUFFER[1]-0x31 ;
							fall_time_index = RX_BUFFER[3]-0x31 ;			
							ISPEraseSector(ISP_ADDRESS);
							ISPProgramByte(ISP_ADDRESS,rise_time_index);
							ISPProgramByte(ISP_ADDRESS+1,fall_time_index);
							SendString("reconfigure rise/fall time!\n");
						}
						SendString("set!\n");
						SendData('\n');
					}
					else{
						SendString("invalid level\n");
					}
				}
				else{
					SendString("invalid cmd\n");
				}
			ClearRX_BUF();
		}
	RI=0;
}
	if(TI){
		TI = 0;
	}
}

/*----------------------------
Software delay function
----------------------------*/
void Delay(BYTE n)
{
    WORD x;

    while (n--)
    {
        x = 0;
        while (++x);
    }
}

/*----------------------------
Disable ISP/ISP/EEPROM function
Make MCU in a safe state
----------------------------*/
void ISPIdle()
{
    ISP_CONTR = 0;                  //Close ISP function
    ISP_CMD = 0;                    //Clear command to standby
    ISP_TRIG = 0;                   //Clear trigger register
    ISP_ADDRH = 0x80;               //Data ptr point to non-EEPROM area
    ISP_ADDRL = 0;                  //Clear ISP address to prevent misuse
}

/*----------------------------
Read one byte from ISP/ISP/EEPROM area
Input: addr (ISP/ISP/EEPROM address)
Output:Flash data
----------------------------*/
BYTE ISPReadByte(WORD addr)
{
    BYTE dat;                       //Data buffer

    ISP_CONTR = ENABLE_ISP;         //Open ISP function, and set wait time
    ISP_CMD = CMD_READ;             //Set ISP/ISP/EEPROM READ command
    ISP_ADDRL = addr;               //Set ISP/ISP/EEPROM address low
    ISP_ADDRH = addr >> 8;          //Set ISP/ISP/EEPROM address high
    ISP_TRIG = 0x46;                //Send trigger command1 (0x46)
    ISP_TRIG = 0xb9;                //Send trigger command2 (0xb9)
    _nop_();                        //MCU will hold here until ISP/ISP/EEPROM operation complete
    dat = ISP_DATA;                 //Read ISP/ISP/EEPROM data
    ISPIdle();                      //Close ISP/ISP/EEPROM function

    return dat;                     //Return Flash data
}

/*----------------------------
Program one byte to ISP/ISP/EEPROM area
Input: addr (ISP/ISP/EEPROM address)
       dat (ISP/ISP/EEPROM data)
Output:-
----------------------------*/
void ISPProgramByte(WORD addr, BYTE dat)
{
    ISP_CONTR = ENABLE_ISP;         //Open ISP function, and set wait time
    ISP_CMD = CMD_PROGRAM;          //Set ISP/ISP/EEPROM PROGRAM command
    ISP_ADDRL = addr;               //Set ISP/ISP/EEPROM address low
    ISP_ADDRH = addr >> 8;          //Set ISP/ISP/EEPROM address high
    ISP_DATA = dat;                 //Write ISP/ISP/EEPROM data
    ISP_TRIG = 0x46;                //Send trigger command1 (0x46)
    ISP_TRIG = 0xb9;                //Send trigger command2 (0xb9)
    _nop_();                        //MCU will hold here until ISP/ISP/EEPROM operation complete
    ISPIdle();
}

/*----------------------------
Erase one sector area
Input: addr (ISP/ISP/EEPROM address)
Output:-
----------------------------*/
void ISPEraseSector(WORD addr)
{
    ISP_CONTR = ENABLE_ISP;         //Open ISP function, and set wait time
    ISP_CMD = CMD_ERASE;            //Set ISP/ISP/EEPROM ERASE command
    ISP_ADDRL = addr;               //Set ISP/ISP/EEPROM address low
    ISP_ADDRH = addr >> 8;          //Set ISP/ISP/EEPROM address high
    ISP_TRIG = 0x46;                //Send trigger command1 (0x46)
    ISP_TRIG = 0xb9;                //Send trigger command2 (0xb9)
    _nop_();                        //MCU will hold here until ISP/ISP/EEPROM operation complete
    ISPIdle();
}

