#include "calculator.h"

char _buffer[BUFFER_SIZE];
unsigned short _buffer_index = 0;
char _result[15];

unsigned char _prev_button_count = 0;
unsigned char _mode_pressed = 0;
unsigned char _eq_pressed = 0;

unsigned char _mode = 0;

unsigned char _store = 0;
unsigned short _back_hold_counter = 0;

unsigned char InputToNum(unsigned short input, unsigned char mode, unsigned char eq){
	unsigned int inp = input | (mode << 16) | (eq << 17);
	
	switch(inp){
			case 0x01000: return 0;  // 1
			case 0x02000: return 1;  // 2
			case 0x04000: return 2;  // 3
			case 0x00100: return 3;  // 4
			case 0x00200: return 4;  // 5
			case 0x00400: return 5;  // 6
			case 0x00010: return 6;  // 7
			case 0x00020: return 7;  // 8
			case 0x00040: return 8;  // 9
			case 0x00001: return 9;  // *
			case 0x00002: return 10; // 0
			case 0x00004: return 11; // #
			case 0x08000: return 12; // A
			case 0x00800: return 13; // B
			case 0x00080: return 14; // C
			case 0x00008: return 15; // D
			case 0x10000: return 16; // mode
			case 0x20000: return 17; // eq
			default: return 0x00;
		}
}

char CharInputLookup(unsigned char input){		
	char lookup[][4] = {
		{'1', 's', '1', 0x0},
		{'2', 'd', '2', 0x0},
		{'3', '!', '3', 0x0},
		{'4', 'c', '4', 0x0},
		{'5', 'v', '5', 0x0},
		{'6', 'R', '6', 0x0},
		{'7', 't', '7', 0x0},
		{'8', 'y', '8', 0x0},
		{'9', 'S', '9', 0x0},
		{'.', '(', '.', 0x0},
		{'0', ')', '0', 0x0},
		{'\b', 'P', '\b', 0x0},
		{'+', 'r', 0x0, 'A'},
		{'-', 'l', 0x0, 'B'},
		{'*', '^', 0x0, 'C'},
		{'/', 'E', 'E', 'D'},
		{'>', '<', 0x0, '<'},
		{'=', 'a', 'e', 'a'}};
	
	if(input == 0xFF){
		return 0x00;
	}
	
	return lookup[input][_mode];
}

char* ExpandChar(char c){
	char r[2];
	switch(c){
		case '*': return "x";
		case '/': return "\xfd";
		case 's': return "Sin";
		case 'd': return "Sin\xe9";
		case 'r': return "\xe8";
		case 'c': return "Cos";
		case 'v': return "Cos\xe9";
		case 'l': return "Log";
		case 't': return "Tan";
		case 'y': return "Tan\xe9";
		case 'a': return "ANS";
		case 'e': return "(";
		case 'P': return "\xf7";
		case 'E': return "e";
	}
	r[0] = c;
	r[1] = '\0';
	return r;
}

unsigned char CountPressed(unsigned short p){
	unsigned char s = 0;
	unsigned char i = 0;
	for(i = 0; i < 16; i++){
		s += NormaliseShort(p & (1 << i));
	}
	return s;
}

void Render(void){
	char str[15];
	char *res;
	unsigned short strI = 0;
	unsigned short i;
	unsigned short j;
	for(i = 0; i < 15; i++){
		str[i] = ' ';
	}
	WriteCommand(0x0C);
	LcdGoto(1, 15);
	if(_mode == 0){
		LcdWriteData(' ');
	}
	else if(_mode == 1){
		LcdWriteData('\xff');
	}
	else if(_mode == 2){
		LcdWriteData('(');
	}
	else if(_mode == 3){
		if(_store){
			LcdWriteData('S');
		}
		else{
			LcdWriteData('R');
		}
	}
	
	for(i = 0; i < _buffer_index; i++){
		res = ExpandChar(_buffer[i]);
		while(*res){
			if(strI >= 15){
				for(j = 0; j < strI; j++){
					str[j] = str[j + 1];
				}
				strI--;
			}
			str[strI] = *res;
			strI++;
			res++;
		}
	}
	j = 15;
	LcdGoto(0, 0);
	for(i = 0; i < 15; i++){
		LcdWriteData(str[i]);
		if(str[i] == 0x20 && i < j){
			j = i;
		}
	}
	LcdGoto(0, j);
	WriteCommand(0x0E);
		
}

void DisplayError(char err){
	_mode = 4;
	WriteCommand(0x0C);
	LcdClearScreen();
	LcdGoto(0, 3);
	LcdWriteString(  "- ERROR: -");
	if(err == ERR_BUFFER_OVERFLOW){
		LcdGoto(1, 0);
		LcdWriteString("Buffer  overflow");
	}
	else if(err == ERR_INTERNAL){
		LcdGoto(1, 0);
		LcdWriteString("Internal error");
	}
	else if(err == ERR_SYNTAX){
		LcdGoto(1, 2);
		LcdWriteString( "Syntax error");
	}
	else if(err == ERR_MATHS){
		LcdGoto(1, 2);
		LcdWriteString( "Maths error");
	}
	else if(err == ERR_NUMBER_TOO_LARGE){
		LcdGoto(1, 0);
		LcdWriteString("Number overflow");
	}
	else if(err == ERR_NUMBER_TOO_SMALL){
		LcdGoto(1, 0);
		LcdWriteString("Number underflow");
	}
	else{
		LcdGoto(1, 1);
		LcdWriteString( "Unknown error");
	}
}

void DisplayResult(char row){
	short i;
	char count_before_decimal = 0;
	char decimal_index = 0;
	LcdGoto(row, 0);
	for(i = 14; i >= 0; i--){
		if(_result[i] == '.'){
			decimal_index = i;
		}
		else if(i > decimal_index){
			if(count_before_decimal > 0){
				count_before_decimal++;
			}
			else{
				if(_result[i] != '0' && _result[i] != 0x00){
					count_before_decimal = 1;
				}
			}
		}
	}
	
	if(count_before_decimal > 0){
		LcdGoto(row, 16 - (decimal_index + 1 + count_before_decimal));
	}
	else{
		LcdGoto(row, 16 - decimal_index);
	}
	LcdWriteString(_result);
	
	for(i = 0; i < 16; i++){
		_result[i] = '0';
	}
}


void Update(void){
	unsigned short keypad = ReadKeypad();
	unsigned char special_buttons = ReadSpecialButtons();
	unsigned char button_count = CountPressed(keypad) + Normalise(special_buttons & MODE_BIT) + Normalise(special_buttons & EQ_BIT);
	char charPressed;
	char *res;
	double result;
	unsigned char err;
	
	// if only one button has just been pressed, Update, otherwise, continue
	if(!(button_count == 1 && _prev_button_count != button_count) ){ // 0
		if(button_count == 1){
			charPressed = CharInputLookup(InputToNum(keypad, _mode_pressed, _eq_pressed));
			if(charPressed == '\b'){
				_back_hold_counter++;
				if(_back_hold_counter > 40){
					_back_hold_counter = 0;
					_buffer_index = 0;
					Render();
				}
			}
		}
		else{
			_back_hold_counter = 0;
		}
		_prev_button_count = button_count;
		_mode_pressed = (special_buttons & MODE_BIT) != 0;
		_eq_pressed = (special_buttons & EQ_BIT) != 0;
		return;
	}
	_back_hold_counter = 0;
	_prev_button_count = button_count;
	_mode_pressed = (special_buttons & MODE_BIT) != 0;
	_eq_pressed = (special_buttons & EQ_BIT) != 0;
	
	// In const state, button pressed
	if(_mode == 4){
		// was displaying const
		// clean screen
		_mode = 0;
		_buffer_index = 0;
		LcdClearScreen();
		if(_eq_pressed){
			Render();
		  return;
		}
	}
	
	charPressed = CharInputLookup(InputToNum(keypad, _mode_pressed, _eq_pressed));
	
	if(charPressed == 'a' && _store){
		charPressed = 0x0;
	}
	if(charPressed == 0x00){
		return;
	}
	
	// return to mode0 after valid input
	if(_mode == 1){
		_mode = 0;
	}
	
	if(charPressed == 'A' || charPressed == 'B' || charPressed == 'C' || charPressed == 'D' || charPressed == 'a'){//  _mode == 1 || _mode == 3){
		_mode = 0;
		// store value
		if(_store){
			_store = 0x00;
			_mode = 4;
			WriteCommand(0x0C);
			err = Evaluate(_buffer, _buffer_index, &result);
			if(err == SUCCESS){
				int ret = snprintf(_result, sizeof(_result), "%.15f", result);
				DisplayResult(0x1);
				LcdGoto(1, 0);
				
				res = ExpandChar(charPressed);
				while(*res){
					LcdWriteData(*res);
					res++;
				}
				LcdWriteData('\x7f');
				StoreMemory(charPressed, result);
			}
			else{
				DisplayError(err);
			}	
			// evaluate _buffer now.
			//if success:
			// put eval result to the first line
			return;
		}
	}
	// switch to mode2
	else if(charPressed == 'r' || charPressed == 'l'){
		_mode = 2;
	}
	else if(charPressed == 'e'){
		_mode = 0;
	}
	
	
	if(charPressed == '>'){
		_mode = 1;
	}
	else if(charPressed == '<'){
		_mode = 0;
	}
	else if(charPressed == '='){
		_mode = 4;
		WriteCommand(0x0C);
		err = Evaluate(_buffer, _buffer_index, &result);
		if(err == SUCCESS){
			int ret = snprintf(_result, sizeof(_result), "%.15f", result);
			DisplayResult(0x01);
			LcdGoto(1, 0);
			LcdWriteData('=');
		}
		else{
			DisplayError(err);
		}	
		
		return;
	}
	// delete
	else if(charPressed == '\b'){
		if(_buffer_index > 0){
			_buffer_index--;
			// special case for root and log
			if(_buffer[_buffer_index] == 'e'){
				_mode = 2;
			}
			if(_buffer[_buffer_index] == 'r' || _buffer[_buffer_index] == 'l'){
				_mode = 0;
			}
			if(_buffer_index > 0 &&
				(_buffer[_buffer_index - 1] == 's' || _buffer[_buffer_index - 1] == 'd' ||
			   _buffer[_buffer_index - 1] == 'c' || _buffer[_buffer_index - 1] == 'v' ||
			   _buffer[_buffer_index - 1] == 't' || _buffer[_buffer_index - 1] == 'y'    )){
				_buffer_index--;
			}
		}
	}
	// switch to mode3
	else if(charPressed == 'R'){
		_mode = 3;
		_store = 0x0;
	}
	else if(charPressed == 'S'){
		_mode = 3;
		_store = 0x1;
	}
	// write to buffer
	else{
		_buffer[_buffer_index] = charPressed;
		_buffer_index++;
	}
	
	if(charPressed == 's' || charPressed == 'd' || charPressed == 'c' || charPressed == 'v' || charPressed == 't' || charPressed == 'y'){
		if(_buffer_index < BUFFER_SIZE){
			_buffer[_buffer_index] = '(';
			_buffer_index++;
		}
	}
	
	// check buffer overflow
	if(_buffer_index >= BUFFER_SIZE){
		DisplayError(ERR_BUFFER_OVERFLOW);
		return;
		// or just _buffer_index--;
	}
	
	Render();
}
