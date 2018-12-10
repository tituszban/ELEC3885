#include "LCD.h"

void PortA_Init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000001;      // 1) A clock
  delay = SYSCTL_RCGC2_R;            // delay   
  GPIO_PORTA_LOCK_R = 0x4C4F434B;    // 2) unlock PortA
  GPIO_PORTA_CR_R |= 0x1C;           // allow changes to PA2-4
  GPIO_PORTA_AMSEL_R &= 0x00;        // 3) disable analog function
  GPIO_PORTA_PCTL_R &= 0x00000000;   // 4) GPIO clear bit PCTL  
	GPIO_PORTA_DIR_R |=  0x0C;         // 5.2) PA2-3 output
	GPIO_PORTA_DIR_R &=  ~0x10;         // 5.2) PA4 output
	GPIO_PORTA_PDR_R |= 0x10;						// PA4 Pulldown resistor
  GPIO_PORTA_AFSEL_R &= 0x00;        // 6) no alternate function
  GPIO_PORTA_DEN_R |= 0x1C;          // 7) enable digital pins PA2-4
}

void PortB_Init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000002;      // 1) B clock
  delay = SYSCTL_RCGC2_R;            // delay   
  GPIO_PORTB_LOCK_R = 0x4C4F434B;    // 2) unlock PortB
  GPIO_PORTB_CR_R |= 0x7C;           // allow changes to PB2-5
  GPIO_PORTB_AMSEL_R &= 0x00;        // 3) disable analog function
  GPIO_PORTB_PCTL_R &= 0x00000000;   // 4) GPIO clear bit PCTL  
	GPIO_PORTB_DIR_R |= 0x3C;          // 5.2) PB2-5 output
	GPIO_PORTB_DIR_R &= ~0x40;					 // PB6 Input
	GPIO_PORTB_PDR_R |= 0x40;						// PB6 pulldown resistor
  GPIO_PORTB_AFSEL_R &= 0x00;        // 6) no alternate function
  GPIO_PORTB_DEN_R |= 0x7C;          // 7) enable digital pins PB2-5
}

void SetRS(unsigned char v){
	LCDRS = Normalise(v) << 3;
}

void SetEN(unsigned char v){
	LCDEN = Normalise(v) << 2;
}

void SetDB(unsigned char v){
	LCDDB = v << 2; //2
}

void PulseEN(){
	SetEN(0x01);
	SysTickWait50ns(10);
	SetEN(0x00);
}

void SendCommand(unsigned char rs, unsigned char db){
	SetRS(rs);
	SetDB(db);
	PulseEN();
}

void WriteCommand(unsigned char data){
	SetRS(0x00);
	SysTickWaitMS(10);
	SetDB((data & 0xF0) >> 4);
	PulseEN();
	SysTickWaitMS(5);
	SetDB(data & 0x0F);
	PulseEN();
	SysTickWaitMS(5);
}

void LcdWriteData(char data){
	SysTickWaitUS(100);
	SetRS(0x01);
	SysTickWait50ns(4);
	SetDB((data & 0xF0) >> 4);
	PulseEN();
	SysTickWait50ns(4);
	SetDB(data & 0x0F);
	PulseEN();
	SysTickWait50ns(4);
}

void LcdWriteString(char* str){
	while(*str){
		LcdWriteData(*str);
		str++;
	}
}

void LcdClearScreen(void){
	WriteCommand(0x01); // Clear Display
}

void LcdGoto(unsigned char line, unsigned char pos){
	unsigned char address = line == 1 ? 0x40 + pos : pos;
	//unsigned char address = (line * 0x40) + pos;
	address |= 0x80;
	WriteCommand(address);
}


void Lcd_Init(void){
	// init ports
	PortA_Init();
	PortB_Init();
	// reset pins
	SetEN(0x00);
	SetRS(0x00);
	SetDB(0x03);
	// setup sequence
	SysTickWaitMS(40);
	PulseEN();
	SysTickWaitUS(4100);
	PulseEN();
	SysTickWaitUS(100);
	PulseEN();
	SysTickWaitUS(100);
	SetDB(0x02);
	PulseEN();
	
	SysTickWaitUS(100);
	WriteCommand(0x28); // Function set, 4 bit, 2 lines, 5x8 font
	WriteCommand(0x0E); // Display on, Cursor on, Cursor blink on
	LcdClearScreen();
	WriteCommand(0x06); // Cursor increment, display shift off
	LcdGoto(0, 0);
}
