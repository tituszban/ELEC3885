//void Set_LEDs(unsigned char r, unsigned char g, unsigned char b);
void Init_Keypad(void);
//void SysTick_WaitMS(unsigned long delayms);
void PLL_Init(void);
void Init_LCD(void);
void lcdGoto(unsigned char line, unsigned char pos);
//void lcdWriteData(char data);
//void lcdClearScreen(void);
void update();

int main(void){
	Init_Keypad();
	PLL_Init();
	Init_LCD();
	lcdGoto(0, 0);
	
	while(1){
		update();
	}
}
