#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))

#define GPIO_PORTE_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_PDR_R        (*((volatile unsigned long *)0x40024514))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_LOCK_R       (*((volatile unsigned long *)0x40024520))
#define GPIO_PORTE_CR_R         (*((volatile unsigned long *)0x40024524))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
#define KEYPAD_RECIEVE          (*((volatile unsigned long *)0x40024078))
#define KEYPAD_SEND_E           (*((volatile unsigned long *)0x40024080))

#define GPIO_PORTD_DATA_R       (*((volatile unsigned long *)0x400073FC))
#define GPIO_PORTD_DIR_R        (*((volatile unsigned long *)0x40007400))
#define GPIO_PORTD_AFSEL_R      (*((volatile unsigned long *)0x40007420))
#define GPIO_PORTD_PUR_R        (*((volatile unsigned long *)0x40007510))
#define GPIO_PORTD_DEN_R        (*((volatile unsigned long *)0x4000751C))
#define GPIO_PORTD_LOCK_R       (*((volatile unsigned long *)0x40007520))
#define GPIO_PORTD_CR_R         (*((volatile unsigned long *)0x40007524))
#define GPIO_PORTD_AMSEL_R      (*((volatile unsigned long *)0x40007528))
#define GPIO_PORTD_PCTL_R       (*((volatile unsigned long *)0x4000752C))
#define KEYPAD_SEND             (*((volatile unsigned long *)0x40007038))


#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SW_BUTTONS              (*((volatile unsigned long *)0x40025044))
#define LED                     (*((volatile unsigned long *)0x40025038))


void SysTick_WaitMS(unsigned long delayms);

void PortE_Init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000010;      // 1) E clock
  delay = SYSCTL_RCGC2_R;            // delay   
  GPIO_PORTE_LOCK_R = 0x4C4F434B;    // 2) unlock PortE 
  GPIO_PORTE_CR_R |= 0x3E;           // allow changes to PE1-4
  GPIO_PORTE_AMSEL_R &= 0x00;        // 3) disable analog function
  GPIO_PORTE_PCTL_R &= 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTE_DIR_R &= ~0x1E;         // 5.1) PE1-4 input
	GPIO_PORTE_DIR_R |=  0x20;         // 5.2) PE5 output
  GPIO_PORTE_AFSEL_R &= 0x00;        // 6) no alternate function
  GPIO_PORTE_PDR_R |= 0x1E;          // enable pulldown resistors on PE1-4       
  GPIO_PORTE_DEN_R |= 0x3E;          // 7) enable digital pins PE1-4
}

void PortD_Init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x0000008;       // 1) D clock
  delay = SYSCTL_RCGC2_R;            // delay
  GPIO_PORTD_LOCK_R = 0x4C4F434B;    // 2) unlock PortD
  GPIO_PORTD_CR_R |= 0x0E;           // allow changes to PD1-3
  GPIO_PORTD_AMSEL_R &= 0x00;        // 3) disable analog function
  GPIO_PORTD_PCTL_R &= 0x00000000;   // 4) GPIO clear bit PCTL
  GPIO_PORTD_DIR_R |= 0x0E;          // 5.2) PD1-3 output
  GPIO_PORTD_AFSEL_R &= 0x00;        // 6) no alternate function
  GPIO_PORTD_DEN_R |= 0x0E;          // 7) enable digital pins PD1-3
}

void PortF_Init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;      // 1) F clock
  delay = SYSCTL_RCGC2_R;            // delay
  GPIO_PORTF_LOCK_R = 0x4C4F434B;    // 2) unlock PortF PF0
  GPIO_PORTF_CR_R = 0x1F;            // allow changes to PF4-0
  GPIO_PORTF_AMSEL_R = 0x00;         // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;    // 4) GPIO clear bit PCTL
  GPIO_PORTF_DIR_R = 0x0E;           // 5) PF4,PF0 input, PF3,PF2,PF1 output
  GPIO_PORTF_AFSEL_R = 0x00;         // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;           // enable pullup resistors on PF4,PF0
  GPIO_PORTF_DEN_R = 0x1F;           // 7) enable digital pins PF4-PF0
}



void Init_Keypad(void){
	PortE_Init();
	PortD_Init();
	PortF_Init();
}


unsigned char Read_SW_Buttons(void){
	unsigned char result = (SW_BUTTONS & 0x10) >> 3 | (SW_BUTTONS & 0x01);
	result ^= 0x3;
	return result;
}

unsigned char Normalise(unsigned char v){
	if(v){
		return 0x1;
	}
	else{
		return 0x0;
	}
}

void Set_LEDs(unsigned char r, unsigned char g, unsigned char b){
	LED = (Normalise(r) << 1) | (Normalise(g) << 3) | (Normalise(b) << 2);
}

void Send_Keypad(unsigned char i){
	if(i == 3){
		KEYPAD_SEND_E = (1 << 5);
		KEYPAD_SEND = 0x00;
	}
	else if(i < 3){
		KEYPAD_SEND_E = 0x00;
		KEYPAD_SEND = (1 << (i + 1));
	}
}

unsigned short Scan_Keypad(void){
	unsigned short result = 0x00;
	unsigned char i;
	KEYPAD_SEND = 0x00;
	for(i = 0; i < 4; i++){
		Send_Keypad(i);
		SysTick_WaitMS(5);
		result |= (KEYPAD_RECIEVE << (i * 4)) >> 1;
		SysTick_WaitMS(5);
	}
	return result;
}

unsigned long Read_Input(void){
	return ((unsigned long)Read_SW_Buttons() << 16) | (unsigned long)Scan_Keypad();
}
