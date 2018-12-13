#include "EEPROM.h"

void WaitEEPROM(){
	while ((EE_DONE & 0x1) != 0x00);
}

void EEPROM_Init(){
  if ((RCGC_EEPROM & 0x1) == 0x00){
		RCGC_EEPROM |= 0x1;          // EEPROM enable
  	WaitEEPROM();
		EE_PROT |= 0x1;		     // Set EEPROM to password protect reading and writing
		WaitEEPROM();
  }
  EE_BLOCK &= 0x0000; // Use BLOCK0
	WaitEEPROM();

}

char IsLocked(){
  if (EE_UNLOCK & 0x1){ // Check if BLOCK0 is locked
     return 0x0;
  } else {
     return 0xFF;
	}
}

char Unlock(unsigned int pass){
  EE_UNLOCK = pass;  // Unlock BLOCK0
  if (IsLocked()){    // If the password failed
    EE_UNLOCK |= 0xFFFFFFFF; // UNSURE IF NEEDED Resets unlock register
    return 0x0;
  } else {
    return 0xFF;
  }
}

char SetPass(unsigned int pass){ //Sets 32 bit pin
  if (EE_PASS & 0x1){ // Checks if a password is already set
    return 0x0;     // There is a password already set
  } else {
    EE_PASS = pass;  // Sets a password
    return 0xFF;
  }
}