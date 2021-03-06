#include "keypad.h"

void PortE_Init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000010;      // E clock
  delay = SYSCTL_RCGC2_R;            // delay   
  GPIO_PORTE_LOCK_R = 0x4C4F434B;    // unlock PortE 
  GPIO_PORTE_CR_R |= 0x3E;           // allow changes to PE1-4
  GPIO_PORTE_AMSEL_R &= 0x00;        // disable analog function
  GPIO_PORTE_PCTL_R &= 0x00000000;   // GPIO clear bit PCTL  
  GPIO_PORTE_DIR_R &= ~0x1E;         // PE1-4 input
	GPIO_PORTE_DIR_R |=  0x20;         // PE5 output
  GPIO_PORTE_AFSEL_R &= 0x00;        // no alternate function
  GPIO_PORTE_PDR_R |= 0x1E;          // enable pulldown resistors on PE1-4       
  GPIO_PORTE_DEN_R |= 0x3E;          // enable digital pins PE1-5
}

void PortD_Init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x0000008;       // D clock
  delay = SYSCTL_RCGC2_R;            // delay
  GPIO_PORTD_LOCK_R = 0x4C4F434B;    // unlock PortD
  GPIO_PORTD_CR_R |= 0x0E;           // allow changes to PD1-3
  GPIO_PORTD_AMSEL_R &= 0x00;        // disable analog function
  GPIO_PORTD_PCTL_R &= 0x00000000;   // GPIO clear bit PCTL
  GPIO_PORTD_DIR_R |= 0x0E;          // PD1-3 output
  GPIO_PORTD_AFSEL_R &= 0x00;        // no alternate function
  GPIO_PORTD_DEN_R |= 0x0E;          // enable digital pins PD1-3
}

void PortF_Init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;      // F clock
  delay = SYSCTL_RCGC2_R;            // delay
  GPIO_PORTF_LOCK_R = 0x4C4F434B;    // unlock PortF PF0
  GPIO_PORTF_CR_R = 0x1F;            // allow changes to PF4-0
  GPIO_PORTF_AMSEL_R = 0x00;         // disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;    // GPIO clear bit PCTL
  GPIO_PORTF_DIR_R = 0x0E;           // PF4,PF0 input, PF3,PF2,PF1 output
  GPIO_PORTF_AFSEL_R = 0x00;         // no alternate function
  GPIO_PORTF_PUR_R = 0x11;           // enable pullup resistors on PF4,PF0
  GPIO_PORTF_DEN_R = 0x1F;           // enable digital pins PF4-PF0
}

void Keypad_Init(void){ // init ports
	PortE_Init();
	PortD_Init();
	PortF_Init();
}


unsigned char ReadSpecialButtons(void){ // read extra buttons
	unsigned char result = ((MODE & 0x10) >> 4) | ((EQUALS & 0x40) >> 5);
	return result;
}

void SetLEDs(unsigned char r, unsigned char g, unsigned char b){ // set rgb leds
	LED = (Normalise(r) << 1) | (Normalise(g) << 3) | (Normalise(b) << 2);
}

void SendKeypadPulse(unsigned char i){ // send a pulse to the ith col
	if(i == 3){
		KEYPAD_SEND_E = (1 << 5);
		KEYPAD_SEND = 0x00;
	}
	else if(i < 3){
		KEYPAD_SEND_E = 0x00;
		KEYPAD_SEND = (1 << (i + 1));
	}
}

unsigned short ReadKeypad(void){ // read the current state of the keypad
	unsigned short result = 0x00;
	unsigned char i;
	KEYPAD_SEND = 0x00;
	for(i = 0; i < 4; i++){                         // for each col
		SendKeypadPulse(i);                           // send pulse
		SysTickWaitMS(5);                             // wait for debounce
		result |= (KEYPAD_RECIEVE << (i * 4)) >> 1;   // read results and concatenate it to result
		SysTickWaitMS(5);
	}
	return result;
}

unsigned short ReadInput(void){ // read combined input
	return ((unsigned long)ReadSpecialButtons() << 16) | (unsigned long)ReadKeypad();
}
