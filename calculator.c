#define BUFFER_SIZE 256 // Up to 65,535

unsigned char Read_SW_Buttons(void);
unsigned short Scan_Keypad(void);
void lcdGoto(unsigned char line, unsigned char pos);
void lcdWriteData(char data);
void lcdClearScreen(void);
void SysTick_WaitUS(unsigned long delayus);
void Write_Command(unsigned char data);
void lcdWriteString(char* str);
unsigned char Normalise(unsigned char v);

char _buffer[BUFFER_SIZE];
unsigned short _buffer_index = 0;

unsigned char _prev_button_count = 0;
unsigned char _mode_bit = 0x01;
unsigned char _eq_bit = 0x02;
unsigned char _mode_pressed = 0;
unsigned char _eq_pressed = 0;

unsigned char _del_bit = 0x0004;

unsigned char _mode = 0;

unsigned char _store = 0;

unsigned char Evaluate(char* buffer, unsigned int index, float *out);

unsigned short NormaliseShort(unsigned short v){
	if(v){
		return 0x1;
	}
	else{
		return 0x0;
	}
}

unsigned char inputToNum(unsigned short input, unsigned char mode, unsigned char eq){
	unsigned int inp = input | (mode << 16) | (eq << 17);
	
	switch(inp){
			case 0x1000: return 0; // 1
			case 0x2000: return 1; // 2
			case 0x4000: return 2; // 3
			case 0x0100: return 3; // 4
			case 0x0200: return 4; // 5
			case 0x0400: return 5; // 6
			case 0x0010: return 6; // 7
			case 0x0020: return 7; // 8
			case 0x0040: return 8; // 9
			case 0x0001: return 9; // *
			case 0x0002: return 10; // 0
			case 0x0004: return 11; // #
			case 0x8000: return 12; // A
			case 0x0800: return 13; // B
			case 0x0080: return 14; // C
			case 0x0008: return 15; //D
			case 0x10000: return 16; // mode
			case 0x20000: return 17; // eq
			default: return 0x00;
		}
}

char charInputLookup(unsigned char input){		
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
		{'-', 'l', '-', 'B'},
		{'*', '^', 0x0, 'C'},
		{'/', 'E', 0x0, 'D'},
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
		case 's': return "Sin(";
		case 'd': return "Sin\xe9(";
		case 'r': return "\xe8";
		case 'c': return "Cos(";
		case 'v': return "Cos\xe9(";
		case 'l': return "Log";
		case 't': return "Tan(";
		case 'y': return "Tan\xe9(";
		case 'a': return "ANS";
		case 'e': return "(";
		case 'P': return "\xf7";
		case 'E': return "e";
	}
	r[0] = c;
	r[1] = '\0';
	return r;
}

unsigned char Count_Pressed(unsigned short p){
	unsigned char s = 0;
	unsigned char i = 0;
	for(i = 0; i < 16; i++){
		s += NormaliseShort(p & (1 << i));
	}
	return s;
}

void render(void){
	char str[15];
	char *res;
	unsigned short strI = 0;
	unsigned short i;
	unsigned short j;
	for(i = 0; i < 15; i++){
		str[i] = ' ';
	}
	Write_Command(0x0C);
	lcdGoto(1, 15);
	if(_mode == 0){
		lcdWriteData(' ');
	}
	else if(_mode == 1){
		lcdWriteData('\xff');
	}
	else if(_mode == 2){
		lcdWriteData('(');
	}
	else if(_mode == 3){
		if(_store){
			lcdWriteData('S');
		}
		else{
			lcdWriteData('R');
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
	lcdGoto(0, 0);
	for(i = 0; i < 15; i++){
		lcdWriteData(str[i]);
		if(str[i] == 0x20 && i < j){
			j = i;
		}
	}
	lcdGoto(0, j);
	Write_Command(0x0E);
		
}

void update(void){
	unsigned short keypad = Scan_Keypad();
	unsigned char special_buttons = Read_SW_Buttons();
	unsigned char button_count = Count_Pressed(keypad) + Normalise(special_buttons & _mode_bit) + Normalise(special_buttons & _eq_bit);
	char charPressed;
	char *res;
	float result;
	unsigned char err;
	
	// if only one button has just been pressed, update, otherwise, continue
	if(!(button_count == 1 && _prev_button_count != button_count) ){ // 0
		_prev_button_count = button_count;
		_mode_pressed = (special_buttons & _mode_bit) != 0;
		_eq_pressed = (special_buttons & _eq_bit) != 0;
		return;
	}
	_prev_button_count = button_count;
	_mode_pressed = (special_buttons & _mode_bit) != 0;
	_eq_pressed = (special_buttons & _eq_bit) != 0;
	
	// In const state, button pressed
	if(_mode == 4){
		// was displaying const
		// clean screen
		_mode = 0;
		_buffer_index = 0;
		lcdClearScreen();
		render();
		return;
	}
	
	charPressed = charInputLookup(inputToNum(keypad, _mode_pressed, _eq_pressed));
	
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
			// evaluate _buffer now.
			//if success:
			// put eval result to the first line
			Write_Command(0x0C);
			lcdGoto(1, 0);
			lcdWriteData('\x7e');
			res = ExpandChar(charPressed);
			while(*res){
				
				lcdWriteData(*res);
				res++;
			}
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
		Write_Command(0x0C);
		err = Evaluate(_buffer, _buffer_index, &result);
		
		
		// evaluate _buffer now. if success:
		lcdGoto(1, 0);
		lcdWriteData('=');
		
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
	
	// check buffer overflow
	if(_buffer_index >= BUFFER_SIZE){
		_mode = 4;
		Write_Command(0x0C);
		lcdClearScreen();
		lcdGoto(0, 2);
		lcdWriteString(  "- ERROR: -");
		lcdGoto(1, 0);
		lcdWriteString("Buffer  overflow");
		return;
		// or just _buffer_index--;
	}
	
	render();
}
