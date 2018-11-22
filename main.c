void Set_LEDs(unsigned char r, unsigned char g, unsigned char b);
unsigned long Read_Input(void);
void Init_Keypad(void);
void Delay(int ms);
unsigned long Read_SW_Buttons(void);

int main(void){
	unsigned long input = 0;
	Init_Keypad();
	while(1){
		input = Read_Input();
		Set_LEDs(input & (1 << 0), input & (1 << 1), input & (1 << 2));
		//Set_D_High();
//		Set_LEDs(0x0, 0x1, 0x0);
		Delay(10);
	}
}
