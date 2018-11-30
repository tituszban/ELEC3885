#define SUCCESS 0x00
#define ERR_INTERNAL 0x01
#define ERR_BUFFER_OVERFLOW 0x02
#define ERR_SYNTAX 0x03
#define ERR_MATHS 0x04
#define BUFFER_SIZE 256 // Up to 65,535

#define E  2.7182818284590452353602874
#define PI 3.1415926535897932384626433

float _memoryA = 0;
float _memoryB = 0;
float _memoryC = 0;
float _memoryD = 0;
float _memoryANS = 0;

char _valid_chars[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
																'.', '+', '-', '*', '/', '!', '(', ')', '^',
                                's', 'd', 'c', 'v', 't', 'y', 'r', 'l', 'e',
																'E', 'P', 'A', 'B', 'C', 'D', 'a'};

char _in_atomic[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '.'};
char _spacer = 0;
char _named_atomic[] = {'E', 'P', 'A', 'B', 'C', 'D', 'a'};
char _spacer2 = 0;
char _open_bracket[] = {'(', 'e', 's', 'd', 'c', 'v', 't', 'y'};
char _spacer3 = 0;
char _operation_priority[] = {'-', '+', '/', '*', 'n', '^', 'r', 'l', 'y', 'v', 'd', 't', 'c', 's', '!'};
char _spacer4 = 0;
char _middle_operator[] = {'-', '+', '/', '*', '^'};
char _spacer5 = 0;
char _left_one_operator[] = {'n', 'y', 'v', 'd', 't', 'c', 's'};
char _spacer6 = 0;
char _right_one_operator[] = {'!'};
char _spacer7 = 0;
char _left_two_operator[] = {'r', 'l'};


float charToInt(char c){
	switch(c){
		case '1': return 1;
		case '2': return 2;
		case '3': return 3;
		case '4': return 4;
		case '5': return 5;
		case '6': return 6;
		case '7': return 7;
		case '8': return 8;		
		case '9': return 9;
		case '0': return 0;
		default: return 0;
	}
}

float identifyAtomic(char a){
	switch(a){
		case 'E': return E;
		case 'P': return PI;
		case 'A': return _memoryA;
		case 'B': return _memoryB;
		case 'C': return _memoryC;
		case 'D': return _memoryD;
		case 'a': return _memoryANS;
		default: return 0;
	}
}

char in(char c, char* arr){
	unsigned short i = 0;
	while(*arr){
		if(c == *arr){
			return 0xFF;
		}
		arr++;
	}
	return 0x0;	
}

unsigned short indexOf(char c, char* arr){
	unsigned short i = 0;
	while(*arr){
		if(c == *arr){
			return i;
		}
		i++;
		arr++;
	}
	
	return 0xFFFF;
}

// forward typedef
typedef struct Token Token;

struct Token{
	unsigned char t;
	float value;
	Token* left;
	Token* right;
};

Token _tokens[BUFFER_SIZE];
unsigned short _token_index = 0;
Token _empty_token;



char BuildTree(unsigned short from, unsigned short to, Token** out){ // [from, to)
	int bracket_count = 0;
	int i;
	Token* selectedOperand;
	unsigned short selectedPriority = 15;
	int selectedIndex;
	unsigned short priority;
	unsigned short atomic_count = 0;
	Token* foundAtomic;
	//Token* left;
	//Token* right;
	unsigned char leftFrom;
	unsigned char leftTo;
	unsigned char rightFrom;
	unsigned char rightTo;
	char err;
	
	
	// repace starting - with negative sign
	if(_tokens[from].t == '-'){
		_tokens[from].t = 'n';
	}
	
	while(_tokens[from].t == '(' && _tokens[to - 1].t == ')'){
		from++;
		to--;
	}
	
	if(from >= to){
		return ERR_SYNTAX;
	}
	if(to - 1 == from){
		if(_tokens[from].t == 'a'){
			*out = &_tokens[from];
			return SUCCESS;
		}
		else{
			return ERR_SYNTAX;
		}
	}
	
	for(i = to - 1; i >= from;){
		if(_tokens[i].t == ')'){
			bracket_count++;
		}
		else if(in(_tokens[i].t, _open_bracket)){
			bracket_count--;
		}
		
		if(bracket_count <= 0){
			if(in(_tokens[i].t, _operation_priority)){
				// check operand validity
				if(( (i <= from) || (_tokens[i - 1].t == 'a') || (_tokens[i - 1].t == ')') || (_tokens[i - 1].t == '!')) && // left validate
					 (i >= to - 1 || in(_tokens[i + 1].t, _open_bracket) || _tokens[i + 1].t == 'l' || _tokens[i + 1].t == 'r' || _tokens[i + 1].t == 'a' || _tokens[i + 1].t == '-')){ // right validate
					priority = indexOf(_tokens[i].t, _operation_priority);
					if(priority < selectedPriority){
						selectedPriority = priority;
						selectedOperand = &_tokens[i];
						selectedIndex = i;
					}
				}
			}
			else if(_tokens[i].t == 'a'){
				atomic_count++;
				foundAtomic = &_tokens[i];
			}
		}
		 i--;
	}
	
	// no valid operand found
	if(selectedPriority >= 15){
		if(atomic_count == 1){
			*out = foundAtomic;
			return SUCCESS;
		}
		return ERR_SYNTAX;
	}
	*out = selectedOperand;
	if(in((*out)->t, _middle_operator)){
		leftFrom = from;
		leftTo = selectedIndex;
		rightFrom = selectedIndex + 1;
		rightTo = to;

		err = BuildTree(leftFrom, leftTo, &((*out)->left));
		if(err){
			return err;
		}
		err = BuildTree(rightFrom, rightTo, &((*out)->right));
		if(err){
			return err;
		}
	}
	else if(in((*out)->t, _left_one_operator)){
		leftFrom = selectedIndex + 1;
		leftTo = to;

		err = BuildTree(leftFrom, leftTo, &((*out)->left));
		if(err){
			return err;
		}
		*((*out)->right) = _empty_token;
	}
	else if(in((*out)->t, _right_one_operator)){
		leftFrom = from;
		leftTo = selectedIndex;

		err = BuildTree(leftFrom, leftTo, &((*out)->left));
		if(err){
			return err;
		}
		*((*out)->right) = _empty_token;
	}
	else if(in((*out)->t, _left_two_operator)){
		leftFrom = selectedIndex + 1;
		i = leftFrom;
		while(_tokens[i].t != 'e' && i < to){
			i++;
		}
		if(i >= to){
			return ERR_SYNTAX;
		}
		leftTo = i;
		rightFrom = i + 1;
		rightTo = to - 1;
		
		if(leftFrom - leftTo > 1){		
			err = BuildTree(leftFrom, leftTo, &((*out)->left));
			if(err){
				return err;
			}
		}
		else{
			*((*out)->left) = _empty_token;
		}
		err = BuildTree(rightFrom, rightTo, &((*out)->right));
		if(err){
			return err;
		}
	}
	
	
	return SUCCESS;	
}



char AddToken(unsigned char t, float value){
	_tokens[_token_index].t = t;
	_tokens[_token_index].value = value;
	_token_index++;
	if(_token_index > BUFFER_SIZE){
		return ERR_BUFFER_OVERFLOW;
	}
	return SUCCESS;
}

unsigned char Evaluate(char *buffer, unsigned short index, float *out){
	unsigned short i = 0;
	unsigned char j = 0;
	short atomicState = -1;
	float atomicValue = 0;
	char in_atomic;
	char in_named_atomic;
	char err;
	int open_bracket_count = 0;
	Token* root;
	
	_token_index = 0;
	_empty_token.t = '_';
	//_empty_token.value = NAN; // TODO
	
	*out = 0;
	if(index <= 0){
		_memoryANS = *out;
		return SUCCESS;
	}
	
	// validate chars
	for(i = 0; i < index; i++){
		if(!in(buffer[i], _valid_chars)){
			return ERR_INTERNAL;
		}
		
		in_atomic = in(buffer[i], _in_atomic);
		if(buffer[i] == '.'){
			if(atomicState == 0){
					atomicState = 1;
			}
			else{
				return ERR_SYNTAX;
			}
		}
		else if(in_atomic){
			if(atomicState <= 0){
				atomicValue = atomicValue * 10 + charToInt(buffer[i]);
				atomicState = 0;
			}
			else{
				for(j = 0; j < atomicState; j++){
					atomicValue *= 10;
				}
				atomicValue += charToInt(buffer[i]);
				for(j = 0; j < atomicState; j++){
					atomicValue /= 10;
				}
				atomicState++;
			}
		}
		else{
			if(atomicState >= 0){
				err = AddToken('a', atomicValue);
				if(err){
					return err;
				}
				atomicState = -1;
				atomicValue = 0;
			}
		}
		in_named_atomic = in(buffer[i], _named_atomic);
		if(in_named_atomic){
			err = AddToken('a', identifyAtomic(buffer[i]));
			if(err){
				return err;
			}
		}
		
		if(!in_named_atomic && !in_atomic && buffer[i] != '.'){
			err = AddToken(buffer[i], 0);
			if(err){
				return err;
			}
		}
	}
	if(atomicState >= 0){
		err = AddToken('a', atomicValue);
		if(err){
			return err;
		}
	}
	
	for(i = 0; i < _token_index; i++){
		if(in(_tokens[i].t, _open_bracket)){
			open_bracket_count++;
		}
		else if(_tokens[i].t == ')'){
			open_bracket_count--;
		}
	}
	for(i = 0; i < open_bracket_count; i++){
		err = AddToken(')', 0);
		if(err){
			return err;
		}
		open_bracket_count--;
	}
	if(open_bracket_count < 0){
		//????
	}
	
	err = BuildTree(0, _token_index, &root);
	
	*out = 100.5;
	_memoryANS = *out;
	return SUCCESS;
}