#ifndef EEPROM_H_
#define EEPROM_H_

#include "PLL.h"

void EEPROM_Init(void);
char IsLocked(void);
char Unlock(unsigned int pass);
char SetPass(unsigned int pass);
	
#define RCGC_EEPROM (*((volatile unsigned long *)0x400FE658))
#define EE_DONE     (*((volatile unsigned long *)0x400AF018))
#define EE_UNLOCK   (*((volatile unsigned long *)0x400AF020))
#define EE_BLOCK    (*((volatile unsigned long *)0x400AF004))
#define EE_PASS     (*((volatile unsigned long *)0x400AF034))
#define EE_PROT     (*((volatile unsigned long *)0x400AF030))
	
#endif
