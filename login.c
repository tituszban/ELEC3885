#include "Login.h"

unsigned char _p_button_count = 0;

char _psw_buffer[4];
char _psw_index;

char LoginCharLookup(unsigned char input){ // convert input to displayable chars
	switch(input){
		case 0: return '1';
		case 1: return '2';
		case 2: return '3';
		case 3: return '4';
		case 4: return '5';
		case 5: return '6';
		case 6: return '7';
		case 7: return '8';
		case 8: return '9';
		case 9: return 0x00;
		case 10: return '0';
		case 11: return '\b';
		case 12: return 0x00;
		case 13: return 0x00;
		case 14: return 0x00;
		case 15: return 0x00;
		case 16: return 0x00;
		case 17: return '=';
		default: return 0x00;
	}	
}


char LoginKeypadRead(void){ // generic keypad read specified for login. retur a single char currently pressed
	unsigned short keypad = ReadKeypad();
	unsigned char special_buttons = ReadSpecialButtons();
	unsigned char button_count = CountPressed(keypad) + Normalise(special_buttons & MODE_BIT) + Normalise(special_buttons & EQ_BIT);
	unsigned char mode_pressed;
	unsigned char eq_pressed;
	
	if(!(button_count == 1 && _p_button_count != button_count) ){
		_p_button_count = button_count;
		return 0x00;
	}
	_p_button_count = button_count;
	mode_pressed = (special_buttons & MODE_BIT) != 0;
	eq_pressed = (special_buttons & EQ_BIT) != 0;
	return LoginCharLookup(InputToNum(keypad, mode_pressed, eq_pressed));
}

unsigned int BufferToPsw(){ // turn pass into a single 32 bit number
	unsigned int pass = 0x00;
	char i;
	for(i = 0; i < 4; i++){
		pass |= ((unsigned int)_psw_buffer[i]) << (i * 8);
	}
	return pass;
}

void ClearBuffer(){ // clear password buffer
	char j;
	for(j = 0; j < 4; j++){
		_psw_buffer[j] = 0;
	}
	_psw_index = 0;
}

unsigned char EnterPsw(unsigned int *result){
	char charPressed = 0x00;
	char i = 0;
	ClearBuffer();
	
	do{
		if(charPressed){
			if(charPressed == '\b'){ // delete last item in buffer
				if(_psw_index > 0){
					_psw_index--;
				}
			}
			else if(_psw_index < 4){ // add char to buffer
				_psw_buffer[_psw_index] = charPressed;
				_psw_index++;
			}
		}
		// display password
		LcdGoto(1, 4);
		for(i = 0; i < 4; i++){
			if(i < _psw_index){ // for characters entered display a *
				LcdWriteData('*');
				LcdWriteData(' ');
			}
			else{              // for characters missing display a _
				LcdWriteData('_');
				LcdWriteData(' ');
			}
		}
		charPressed = LoginKeypadRead();
	} while(charPressed != '=');
	if(_psw_index >= 4){
		*result = BufferToPsw();
		return 0xFF;
	}
	return 0x00;
}

void Login(){
	// intro text
	unsigned int psw;
	char locked = 0xFF;
	WriteCommand(0x0C);
	
	// INTRO TEXT
	LcdGoto(0, 3);
	LcdWriteString("d{ o_O }b");
	LcdGoto(1, 0);
	LcdWriteString("HI, I'M DR ST3V3");
	while(!LoginKeypadRead());
	LcdClearScreen();
	LcdGoto(0, 3);
	LcdWriteString("d{ o_O }b");
	LcdGoto(1, 0);
	LcdWriteString("HOW CAN I HELP?");
	while(!LoginKeypadRead());
	LcdClearScreen();
	
	if (IsLocked()){ // check if EEPROM is locked
		LcdGoto(0, 1);
		LcdWriteString("Enter password");
		while(locked){ // while correct password is inputed
			if (EnterPsw(&psw)){
				if(Unlock(psw)){
					locked = 0x0;
				}
			}
		}
		// login
		LcdClearScreen();
		LcdGoto(0, 4);
		LcdWriteString("Unlocked");
		SysTickWaitMS(500);
	}
	else { // if eeprom is unlocked, setup a new password
		LcdGoto(0, 2);
		LcdWriteString("Set password");
		while(locked){
			if(EnterPsw(&psw)){
				if (SetPass(psw)){
					locked = 0x0;
				}
			}
		}
		LcdGoto(0, 0);
		LcdWriteString("New password set");
		SysTickWaitMS(500);
	}
	LcdClearScreen();
	WriteCommand(0x0E);
}
