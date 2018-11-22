#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))

#define GPIO_PORTA_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTA_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTA_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTA_PDR_R        (*((volatile unsigned long *)0x40024514))
#define GPIO_PORTA_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTA_LOCK_R       (*((volatile unsigned long *)0x40024520))
#define GPIO_PORTA_CR_R         (*((volatile unsigned long *)0x40024524))
#define GPIO_PORTA_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTA_PCTL_R       (*((volatile unsigned long *)0x4002452C))


#define GPIO_PORTB_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTB_PDR_R        (*((volatile unsigned long *)0x40024514))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTB_LOCK_R       (*((volatile unsigned long *)0x40024520))
#define GPIO_PORTB_CR_R         (*((volatile unsigned long *)0x40024524))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4002452C))
	
void PortA_Init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000001;      // 1) A clock
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