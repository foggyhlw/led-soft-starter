typedef unsigned char BYTE;
typedef unsigned int  WORD;	
#define FOSC 12000000L      //System frequency
#define BAUD 9600           //UART baudrate
#define BUF_LEN 4

#define RISE_TIME_ADDRESS 0x0000
#define FALL_TIME_ADDRESS 0x0001
#define ENABLE_ISP 0x84           //if SYSCLK<6MHz

#define CMD_IDLE    0               //Stand-By
#define CMD_READ    1               //Byte-Read
#define CMD_PROGRAM 2               //Byte-Program
#define CMD_ERASE   3               //Sector-Erase
#define ISP_ADDRESS 0x0400

WORD	rise_counter = 0;
WORD	fall_counter = 0;
WORD	test_counter = 0;
BYTE direction=0;

BYTE RX_BUFFER[BUF_LEN];

extern void Init_GlobalVariables();
extern void Init_PCA();
extern void Init_Timer0();
extern void delay();
extern void Init_ADC();
extern void Init_UART();
extern void UARTInterrupt();
extern void Init_GPIO();
extern void debounce();
extern void SendData(BYTE dat);
extern void SendString(BYTE *str);
extern void ClearRX_BUF();

extern void Delay(BYTE n);
extern void ISPIdle();
extern BYTE ISPReadByte(WORD addr);
extern void ISPProgramByte(WORD addr, BYTE dat);
extern void ISPEraseSector(WORD addr);