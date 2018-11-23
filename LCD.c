#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))

#define GPIO_PORTA_DATA_R       (*((volatile unsigned long *)0x400043FC))
#define GPIO_PORTA_DIR_R        (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_AFSEL_R      (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_PDR_R        (*((volatile unsigned long *)0x40004514))
#define GPIO_PORTA_DEN_R        (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_LOCK_R       (*((volatile unsigned long *)0x40004520))
#define GPIO_PORTA_CR_R         (*((volatile unsigned long *)0x40004524))
#define GPIO_PORTA_AMSEL_R      (*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_PCTL_R       (*((volatile unsigned long *)0x4000452C))
#define LCDRS                   (*((volatile unsigned long *)0x40004020)) // PA3
#define LCDEN                   (*((volatile unsigned long *)0x40004010)) // PA2

#define GPIO_PORTB_DATA_R       (*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_PDR_R        (*((volatile unsigned long *)0x40005514))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_LOCK_R       (*((volatile unsigned long *)0x40005520))
#define GPIO_PORTB_CR_R         (*((volatile unsigned long *)0x40005524))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
#define LCDDB                   (*((volatile unsigned long *)0x400050F0)) // PB2-5

void SysTick_WaitMS(unsigned long delayms);
void SysTick_Wait50ns(unsigned long delay50ns);
void SysTick_WaitUS(unsigned long delayus);
unsigned char Normalise(unsigned char v);

void PortA_Init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000001;      // 1) A clock
  delay = SYSCTL_RCGC2_R;            // delay   
  GPIO_PORTA_LOCK_R = 0x4C4F434B;    // 2) unlock PortA
  GPIO_PORTA_CR_R |= 0x0C;           // allow changes to PA2-3
  GPIO_PORTA_AMSEL_R &= 0x00;        // 3) disable analog function
  GPIO_PORTA_PCTL_R &= 0x00000000;   // 4) GPIO clear bit PCTL  
	GPIO_PORTA_DIR_R |=  0x0C;         // 5.2) PA2-3 output
  GPIO_PORTA_AFSEL_R &= 0x00;        // 6) no alternate function
  GPIO_PORTA_DEN_R |= 0x0C;          // 7) enable digital pins PA2-3
}

void PortB_Init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000002;      // 1) B clock
  delay = SYSCTL_RCGC2_R;            // delay   
  GPIO_PORTB_LOCK_R = 0x4C4F434B;    // 2) unlock PortB
  GPIO_PORTB_CR_R |= 0x3C;           // allow changes to PB2-5
  GPIO_PORTB_AMSEL_R &= 0x00;        // 3) disable analog function
  GPIO_PORTB_PCTL_R &= 0x00000000;   // 4) GPIO clear bit PCTL  
	GPIO_PORTB_DIR_R |=  0x3C;         // 5.2) PB2-5 output
  GPIO_PORTB_AFSEL_R &= 0x00;        // 6) no alternate function
  GPIO_PORTB_DEN_R |= 0x3C;          // 7) enable digital pins PB2-5
}

void Set_RS(unsigned char v){
	LCDRS = Normalise(v) << 3;
}

void Set_EN(unsigned char v){
	LCDEN = Normalise(v) << 2;
}

void Set_DB(unsigned char v){
	LCDDB = v << 2; //2
}

void Pulse_EN(){
	Set_EN(0x01);
	SysTick_Wait50ns(10);
	Set_EN(0x00);
}

void Send_Command(unsigned char rs, unsigned char db){
	Set_RS(rs);
	Set_DB(db);
	Pulse_EN();
}

void Write_Command(unsigned char data){
	Set_RS(0x00);
	SysTick_WaitMS(40);
	Set_DB((data & 0xF0) >> 4);
	Pulse_EN();
	SysTick_WaitMS(5);
	Set_DB(data & 0x0F);
	Pulse_EN();
	SysTick_WaitMS(5);
}

void lcdWriteData(char data){
	Set_RS(0x01);
	SysTick_Wait50ns(4);
	Set_DB((data & 0xF0) >> 4);
	Pulse_EN();
	SysTick_Wait50ns(4);
	Set_DB(data & 0x0F);
	Pulse_EN();
	SysTick_Wait50ns(4);
}

void lcdClearScreen(void){
	Write_Command(0x01); // Clear Display
}

void lcdGoto(unsigned char line, unsigned char pos){
	unsigned char address = line == 1 ? 0x40 + pos : pos;
	//unsigned char address = (line * 0x40) + pos;
	address |= 0x80;
	Write_Command(address);
}


void Init_LCD(void){
	// init ports
	PortA_Init();
	PortB_Init();
	// reset pins
	Set_EN(0x00);
	Set_RS(0x00);
	Set_DB(0x03);
	// setup sequence
	SysTick_WaitMS(40);
	Pulse_EN();
	SysTick_WaitUS(4100);
	Pulse_EN();
	SysTick_WaitUS(100);
	Pulse_EN();
	SysTick_WaitUS(100);
	Set_DB(0x02);
	Pulse_EN();
	
	SysTick_WaitUS(100);
	Write_Command(0x28); // Function set, 4 bit, 2 lines, 5x8 font
	Write_Command(0x0F); // Display on, Cursor on, Cursor blink on
	lcdClearScreen();
	Write_Command(0x06); // Cursor increment, display shift off
}
