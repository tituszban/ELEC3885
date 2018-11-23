void Set_LEDs(unsigned char r, unsigned char g, unsigned char b);
unsigned long Read_Input(void);
void Init_Keypad(void);
void SysTick_WaitMS(unsigned long delayms);
unsigned long Read_SW_Buttons(void);
void PLL_Init(void);
void Init_LCD(void);
void lcdGoto(unsigned char line, unsigned char pos);
void lcdWriteData(char data);
void lcdClearScreen(void);

char charLookup(unsigned long input){
	switch(input){
		case 0x00001: return 0x2E;
		case 0x00002: return 0x30;
		case 0x00004: return 0xFF;
		case 0x00008: return 0x44;
		case 0x00010: return 0x37;
		case 0x00020: return 0x38;
		case 0x00040: return 0x39;
		case 0x00080: return 0x43;
		case 0x00100: return 0x34;
		case 0x00200: return 0x35;
		case 0x00400: return 0x36;
		case 0x00800: return 0x42;
		case 0x01000: return 0x31;
		case 0x02000: return 0x32;
		case 0x04000: return 0x33;
		case 0x08000: return 0x41;
	}
	return 0x00;
}

int main(void){
	unsigned long input = 0;
	char buffer[15];
	char index = 0;
	char lookup;
	float test = 0;
	Init_Keypad();
	PLL_Init();
	Init_LCD();
	lcdGoto(0, 0);
	
	
	
	
	while(1){
		input = Read_Input();
		lookup = charLookup(input);
		
		
		if(lookup != 0x00){
			lcdWriteData(lookup);
		}
		if(lookup == 0xFF){
			lcdClearScreen();
			lcdGoto(0, 0);
		}
		SysTick_WaitMS(100);
	}
}
