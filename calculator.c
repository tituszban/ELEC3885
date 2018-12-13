#include "calculator.h"

char _buffer[BUFFER_SIZE]; // internal buffer for storing the inputed chars
unsigned short _buffer_index = 0;
char _result[15]; // holds the value of the result as a char*

unsigned char _prev_button_count = 0; // previous state trackers for detecting changes
unsigned char _mode_pressed = 0;
unsigned char _eq_pressed = 0;

unsigned char _mode = 0;
	// _mode 0: Normal input mode
	// _mode 1: Special operator input mode
	// _mode 2: Numerical input only (following root or log)
	// _mode 3: Memory select mode (store or recall)
	// _mode 4: Constant display state (result or error)

unsigned char _store = 0;		// flag if _mode 3 is store or recall
unsigned short _back_hold_counter = 0; // counter so the buffer is reset if the delete button is held

unsigned char InputToNum(unsigned short input, unsigned char mode, unsigned char eq){
	// Take all button inputs encoded in a short with a single bit high, and index it into a set of consecutive integers
	// It assumes that precisely one bit is high
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
			default: return 0xFF;    // error
		}
}

char CharInputLookup(unsigned char input){
	// Based on the current mode, return the char (token) corresponding to the input pressed
	char lookup[][4] = {
		{'1', 's', '1', 0x0}, // 1, sin,    1, x
		{'2', 'd', '2', 0x0}, // 2, asin,   2, x
		{'3', '!', '3', 0x0}, // 3, fact,   3, x
		{'4', 'c', '4', 0x0}, // 4, cos,    4, x
		{'5', 'v', '5', 0x0}, // 5, acos,   5, x
		{'6', 'R', '6', 0x0}, // 6, recall, 6, x
		{'7', 't', '7', 0x0}, // 7, tan,    7, x
		{'8', 'y', '8', 0x0}, // 8, atan,   8, x
		{'9', 'S', '9', 0x0}, // 9, store,  9, x
		{'.', '(', '.', 0x0}, // ., (,      ., x
		{'0', ')', '0', 0x0}, // 0, ),      0, x
		{'\b', 'P', '\b', 0x0},//backspace, Pi, backspace, x
		{'+', 'r', 0x0, 'A'}, // +, root,   x, A
		{'-', 'l', 0x0, 'B'}, // -, log,    x, B
		{'*', '^', 0x0, 'C'}, // *, pow,    x, C
		{'/', 'E', 'E', 'D'}, // /, e       e, D
		{'>', '<', 0x0, '<'}, // mode0, mode1, x, mode0
		{'=', 'a', 'e', 'a'}  // equal, ans, special close bracket, ans
	};
	
	if(input == 0xFF){
		return 0x00;
	}
	
	return lookup[input][_mode];
}

char* ExpandChar(char c){
	// internall buffer single char to readable char*
	// \xfd: ÷
	// \xe9: ^-1
	// \xe8: root symbol
	// \xf7: pi symbol
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
	// count number of pits 1 in p -> count number of buttons pressed
	unsigned char s = 0;
	unsigned char i = 0;
	for(i = 0; i < 16; i++){
		s += NormaliseShort(p & (1 << i));
	}
	return s;
}

void Render(void){
	// Take the internall buffer and render it to the lcd
	char str[15];
	char *res;
	unsigned short strI = 0;
	unsigned short i;
	unsigned short j;
	for(i = 0; i < 15; i++){ // clear str
		str[i] = ' ';
	}
	WriteCommand(0x0C);	// disable cursor
	// put mode signaling character
	LcdGoto(1, 15);
	if(_mode == 0){
		LcdWriteData(' ');
	}
	else if(_mode == 1){
		LcdWriteData('\xff'); // \xff: full black character
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
	
	// Turn the internal buffer characters to their rendered versions
	for(i = 0; i < _buffer_index; i++){
		res = ExpandChar(_buffer[i]);
		while(*res){
			if(strI >= 15){ // shift str if there are more than 15 symbols to render
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
	// Print all chars to the lcd
	j = 15;
	LcdGoto(0, 0);
	for(i = 0; i < 15; i++){
		LcdWriteData(str[i]);
		if(str[i] == 0x20 && i < j){
			j = i;
		}
	}
	LcdGoto(0, j); // go to the end of the output
	WriteCommand(0x0E); // re-enable the cursor
		
}

void DisplayError(char err){
	// display error with specific error code
	_mode = 4;
	WriteCommand(0x0C); // disable cursor
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
	// Display the value stored in _result in the specified row
	short i;
	char count_before_decimal = 0;
	char decimal_index = 0;
	LcdGoto(row, 0);
	// find number of characters before and after the decimal, to make sure all values before the decimal are displayed
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
	// find the place to start printing the result from
	if(count_before_decimal > 0){
		LcdGoto(row, 16 - (decimal_index + 1 + count_before_decimal));
	}
	else{
		LcdGoto(row, 16 - decimal_index);
	}
	LcdWriteString(_result);	// print the result
	
	for(i = 0; i < 16; i++){ // reset the result
		_result[i] = '0';
	}
}


void Update(void){
	// take the inputs from the keypad, add it to the buffer and display the result
	unsigned short keypad = ReadKeypad();
	unsigned char special_buttons = ReadSpecialButtons();
	unsigned char button_count = CountPressed(keypad) + Normalise(special_buttons & MODE_BIT) + Normalise(special_buttons & EQ_BIT);
	char charPressed;
	char *res;
	double result;
	unsigned char err;
	
	// if only one button has just been pressed Update, otherwise continue
	if(!(button_count == 1 && _prev_button_count != button_count) ){ // 0
		if(button_count == 1){ // if a button is being held
			charPressed = CharInputLookup(InputToNum(keypad, _mode_pressed, _eq_pressed));
			if(charPressed == '\b'){
				_back_hold_counter++;
				if(_back_hold_counter > BACK_HOLD_THRESHOLD){	// if the back button is held for long enough
					_back_hold_counter = 0;			// reset the entire buffer
					_buffer_index = 0;
					Render();
				}
			}
		}
		else{
			_back_hold_counter = 0; // reset the counter if backspace is not pressed
		}
		_prev_button_count = button_count; // update the previous state
		_mode_pressed = (special_buttons & MODE_BIT) != 0;
		_eq_pressed = (special_buttons & EQ_BIT) != 0;
		return; // run the loop again
	}
	_back_hold_counter = 0;
	_prev_button_count = button_count; // update the previous state
	_mode_pressed = (special_buttons & MODE_BIT) != 0;
	_eq_pressed = (special_buttons & EQ_BIT) != 0;
	
	// In const state, button pressed
	if(_mode == 4){
		// was displaying const
		_mode = 0; // return to _mode 0
		_buffer_index = 0; // reset buffer
		LcdClearScreen();  // clear lcd
		if(_eq_pressed){	 // if equals was pressed, don't run button evaluation, just render empty screen and wait for next button pressed
			Render();
		  return;
		}
	}
	
	// get char based on button pressed
	charPressed = CharInputLookup(InputToNum(keypad, _mode_pressed, _eq_pressed));
	
	// if user tries to store to ans, escape
	if(charPressed == 'a' && _store){
		charPressed = 0x00;
	}
	// if the char is invalid (0x00) return and wait for new input
	if(charPressed == 0x00){
		return;
	}
	
	// return to _mode 0 after valid input
	if(_mode == 1){
		_mode = 0;
	}
	
	// if Sotring or recalling a value
	if(charPressed == 'A' || charPressed == 'B' || charPressed == 'C' || charPressed == 'D' || charPressed == 'a'){//  _mode == 1 || _mode == 3){
		_mode = 0; // return to _mode 0
		// store value
		if(_store){
			_store = 0x00; // reset store flag
			_mode = 4;     // enter const mode
			WriteCommand(0x0C); // disable cursor
			err = Evaluate(_buffer, _buffer_index, &result); // evaluate the value of the buffer now
			if(err == SUCCESS){ // if buffer successfully evaluated
				int ret = snprintf(_result, sizeof(_result), "%.15f", result); // put result into a string
				DisplayResult(0x1); // display the value of the result
				LcdGoto(1, 0); // go to the bottor left corner
				
				res = ExpandChar(charPressed); // display the selected memory id
				while(*res){
					LcdWriteData(*res);
					res++;
				}
				LcdWriteData('\x7f'); // \x7f: <- symbol
				StoreMemory(charPressed, result); // store memory
			}
			else{
				DisplayError(err); // if evaluation is unsuccessful, display the result
			}
			return;
		}
	}
	// switch to mode2 if the operator is root or log
	else if(charPressed == 'r' || charPressed == 'l'){
		_mode = 2;
	}
	// return to _mode 0 at the special open bracket
	else if(charPressed == 'e'){
		_mode = 0;
	}
	
	// execute mode switch chars
	if(charPressed == '>'){
		_mode = 1;
	}
	else if(charPressed == '<'){
		_mode = 0;
	}
	// run evaluation
	else if(charPressed == '='){
		_mode = 4;
		WriteCommand(0x0C); // disable cursor
		err = Evaluate(_buffer, _buffer_index, &result); // evaluate the result
		if(err == SUCCESS){ // if successfully evaluated
			int ret = snprintf(_result, sizeof(_result), "%.15f", result); // turn the double to a char*
			DisplayResult(0x01); // display the result
			LcdGoto(1, 0); // display the = character in the bottom left corner
			LcdWriteData('=');
		}
		else{
			DisplayError(err); // if unsuccessful display the error
		}	
		
		return; // wait for next input, before rendering
	}
	// if backspace pressed, delete the last char
	else if(charPressed == '\b'){
		if(_buffer_index > 0){
			_buffer_index--;
			// special case for root and log
			if(_buffer[_buffer_index] == 'e'){ // if the deleted char is the special open bracket, return to restricted input mode
				_mode = 2;
			}
			if(_buffer[_buffer_index] == 'r' || _buffer[_buffer_index] == 'l'){ // if log or root is deleted, return to standard mode
				_mode = 0;
			}
			if(_buffer_index > 0 && // if the character before the currently deleted one is a token that must be followed by a (, also delete the token
				(_buffer[_buffer_index - 1] == 's' || _buffer[_buffer_index - 1] == 'd' ||
			   _buffer[_buffer_index - 1] == 'c' || _buffer[_buffer_index - 1] == 'v' ||
			   _buffer[_buffer_index - 1] == 't' || _buffer[_buffer_index - 1] == 'y'    )){
				_buffer_index--;
			}
		}
	}
	// recall. switch to _mode 3
	else if(charPressed == 'R'){
		_mode = 3;
		_store = 0x0;
	}
	// store. switch to _mode 3
	else if(charPressed == 'S'){
		_mode = 3;
		_store = 0x1;
	}
	// write to buffer
	else{
		_buffer[_buffer_index] = charPressed;
		_buffer_index++;
	}
	
	// add a ( after functions that need it
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
	}
	
	Render();
}
