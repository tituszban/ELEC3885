#ifndef LCD_H_
#define LCD_H_

#include "PLL.h"
#include "util.h"

void Lcd_Init(void);
void LcdGoto(unsigned char line, unsigned char pos);
void LcdClearScreen(void);
void LcdWriteString(char* str);
void LcdWriteData(char data);
void WriteCommand(unsigned char data);

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
#define GPIO_PORTA_PDR_R        (*((volatile unsigned long *)0x40004514))
#define LCDRS                   (*((volatile unsigned long *)0x40004020)) // PA3
#define LCDEN                   (*((volatile unsigned long *)0x40004010)) // PA2
#define MODE	  								(*((volatile unsigned long *)0x40004040)) // PA4


#define GPIO_PORTB_DATA_R       (*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_PDR_R        (*((volatile unsigned long *)0x40005514))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_LOCK_R       (*((volatile unsigned long *)0x40005520))
#define GPIO_PORTB_CR_R         (*((volatile unsigned long *)0x40005524))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
#define GPIO_PORTB_PDR_R        (*((volatile unsigned long *)0x40005514))
#define LCDDB                   (*((volatile unsigned long *)0x400050F0)) // PB2-5
#define EQUALS                  (*((volatile unsigned long *)0x40005100)) // PB6

#endif
