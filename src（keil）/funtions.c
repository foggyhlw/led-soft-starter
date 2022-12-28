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
	P3M0 = 0x02;
	P3M1 = 0x00;   //P32设定为输入
	P0M0 = 0x01;
	P0M1 = 0x00;   //P01 input
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
  AUXR = AUXR | 0x40;
  TMOD |= 0x20;
  SCON = 0x50;
  TH1 = 0x88;
  TL1 = TH1;
  PCON = 0x80;
  EA = 1;
  ES = 1;
  TR1 = 1;
}
	
void Init_Timer0()
{
	 // AUXR = AUXR|0x80;
  TMOD |= 0x01;
  TH0 = 0xFF;
  TL0 = 0xDE;
  EA = 1;
  ET0 = 1;
  TR0 = 1;
}

void Timer0Interrupt(void) interrupt 1
{
	delay_counter++;
	if(delay_counter>500 && direction == 1 && bt_block==0)
	{
		delay_counter=0;
		if(CCAP1H<0xff)
		{
		//	CCAP0H+=0x01;
			CCAP1H+=0x01;
		//	CCAP2H+=0x01;
			//CCAP3H+=0x01;
		}
		if(CCAP1H>0x40 && CCAP3H<0xff)
		{
			CCAP3H+=1;
		}
		if(CCAP3H>0x40 && CCAP0H<0xff)
		{
			CCAP0H+=1;
		}
		if(CCAP1H==0xff)
		{
			PCA_PWM1=0x03;
		}
		if(CCAP2H==0xff)
		{
			PCA_PWM2=0x03;
		}
		if(CCAP3H==0xff)
		{
			PCA_PWM3=0x03;
		}
		if(CCAP0H==0xff)
		{
			PCA_PWM0=0x03;
		}
		
	}
	
		if(delay_counter>500 && direction == 0 && bt_block == 0)
	{
		delay_counter=0;
		PCA_PWM1=0x00;
		PCA_PWM2=0x00;
		PCA_PWM3=0x00;
		PCA_PWM0=0x00;
		if(CCAP1H >= 0x01 && CCAP1H<0x23)
		{
			CCAP1H-=1;
		}
		if(CCAP1H >= 0x23)
		{
			CCAP1H-=3;
		}
		if(CCAP0H >= 0x01 && CCAP0H<0x23)
		{
			CCAP0H-=1;
		}
		if(CCAP0H >= 0x23)
		{
			CCAP0H-=3;
		}
		if(CCAP2H >= 0x01 && CCAP2H<0x23)
		{
			CCAP2H-=1;
		}
		if(CCAP2H >= 0x23)
		{
			CCAP2H-=3;
		}
		if(CCAP3H >= 0x01 && CCAP3H<0x23)
		{
			CCAP3H-=1;
		}
		if(CCAP3H >= 0x23)
		{
			CCAP3H-=3;
		}	
	}
	TH0 = 0x0FF;
  TL0 = 0x0DE;
}

void UARTInterrupt(void) interrupt 4
{
	BYTE temp;
	if(RI)
	{
		RI=0;
		temp=SBUF;
		switch(temp)
		{
			case 0x00: bt_block=0;break;
			case 0x01: bt_block=1;break;
			default  : CCAP2H=temp;
		}
	}
	else
		TI=0;
			
}