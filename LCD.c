#include "LCD.h"

void PortA_Init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000001;      // A clock
  delay = SYSCTL_RCGC2_R;            // delay   
  GPIO_PORTA_LOCK_R = 0x4C4F434B;    // Unlock PortA
  GPIO_PORTA_CR_R |= 0x1C;           // Allow changes to PA2-4
  GPIO_PORTA_AMSEL_R &= 0x00;        // Disable analog function
  GPIO_PORTA_PCTL_R &= 0x00000000;   // GPIO clear bit PCTL  
	GPIO_PORTA_DIR_R |=  0x0C;         // PA2-3 output
	GPIO_PORTA_DIR_R &=  ~0x10;        // PA4 output
	GPIO_PORTA_PDR_R |= 0x10;					 // PA4 Pulldown resistor
  GPIO_PORTA_AFSEL_R &= 0x00;        // no alternate function
  GPIO_PORTA_DEN_R |= 0x1C;          // enable digital pins PA2-4
}

void PortB_Init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000002;      // B clock
  delay = SYSCTL_RCGC2_R;            // delay   
  GPIO_PORTB_LOCK_R = 0x4C4F434B;    // Unlock PortB
  GPIO_PORTB_CR_R |= 0x7C;           // Allow changes to PB2-5
  GPIO_PORTB_AMSEL_R &= 0x00;        // Disable analog function
  GPIO_PORTB_PCTL_R &= 0x00000000;   // GPIO clear bit PCTL  
	GPIO_PORTB_DIR_R |= 0x3C;          // PB2-5 output
	GPIO_PORTB_DIR_R &= ~0x40;				 // PB6 Input
	GPIO_PORTB_PDR_R |= 0x40;					 // PB6 pulldown resistor
  GPIO_PORTB_AFSEL_R &= 0x00;        // no alternate function
  GPIO_PORTB_DEN_R |= 0x7C;          // enable digital pins PB2-5
}

void SetRS(unsigned char v){ // shorthand for setting the rs pin
	LCDRS = Normalise(v) << 3;
}

void SetEN(unsigned char v){ // shorthand for setting the en pin
	LCDEN = Normalise(v) << 2;
}

void SetDB(unsigned char v){ // shorthand for setting the db pins all at once
	LCDDB = v << 2;
}

void PulseEN(){ // send 500ns pulse on the en pin
	SetEN(0x01);
	SysTickWait50ns(10);
	SetEN(0x00);
}

void SendCommand(unsigned char rs, unsigned char db){ // set rs and db pins and send pulse
	SetRS(rs);
	SetDB(db);
	PulseEN();
}

void WriteCommand(unsigned char data){ // write full command
	SetRS(0x00);                // reset rs
	SysTickWaitMS(10);          // wait 10ms
	SetDB((data & 0xF0) >> 4);  // send first nibble to db pins
	PulseEN();                  // pulse en
	SysTickWaitMS(5);           // wait 5ms
	SetDB(data & 0x0F);         // send second nibble to db pins
	PulseEN();                  // pulse en
	SysTickWaitMS(5);           // wait 5 ms
}

void LcdWriteData(char data){ // write data (same as command, but different timing and rs)
	SysTickWaitUS(100);         // wait 100 us
	SetRS(0x01);                // set rs
	SysTickWait50ns(4);         // wait 200ns
	SetDB((data & 0xF0) >> 4);  // send first nibble to the db pins
	PulseEN();                  // pulse en
	SysTickWait50ns(4);         // wait 200 ns
	SetDB(data & 0x0F);         // send second nibble to the db pins
	PulseEN();                  // pulse en
	SysTickWait50ns(4);         // wait 200 ns
}

void LcdWriteString(char* str){ // write every char in a null ended string
	while(*str){
		LcdWriteData(*str);
		str++;
	}
}

void LcdClearScreen(void){ // send clear display command
	WriteCommand(0x01);
}

void LcdGoto(unsigned char line, unsigned char pos){ // set the cursor position
	unsigned char address = (line == 1 ? 0x40 : 0) + pos; // line sets the 0x40 bit, pos sets the rest
	address |= 0x80; // set the 0x80 bit
	WriteCommand(address); // send command
}


void Lcd_Init(void){ // lcd init, as specified in the datasheet
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
