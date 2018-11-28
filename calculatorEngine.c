#define SUCCESS 0x00
#define ERR_INTERNAL 0x01
#define ERR_BUFFER_OVERFLOW 0x02
#define ERR_SYNTAX 0x03
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
                                's', 'd', 'c', 'v', 't', 'y', 'e', 'r', 'l',
																'E', 'P', 'A', 'B', 'C', 'D', 'a'};

char _in_atomic[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
char _named_atomic[] = {'E', 'P', 'A', 'B', 'C', 'D', 'a'};

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
	return 0x0;/*
	for(i = 0; i < sizeof(arr) / sizeof(char); i++){
		if(c == arr[i]){
			
		}
	}*/
	
}
																
typedef struct{
	unsigned char type;
	float value;
	// attached function
	// left leaf
	// right leaf
} Token;

unsigned char Evaluate(char *buffer, unsigned short index, float *out){
	unsigned short i = 0;
	unsigned char j = 0;
	short atomicState = -1;
	float atomicValue = 0;
	
	Token tokens[BUFFER_SIZE];
	unsigned short token_index = 0;
	
	*out = 0;
	if(index <= 0){
		_memoryANS = *out;
		return SUCCESS;
	}
	
	// validate chars
	for(i = 0; i < index; ){
		if(!in(buffer[i], _valid_chars)){
			return ERR_INTERNAL;
		}
		
		if(in(buffer[i], _in_atomic)){
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
		else if(buffer[i] == '.'){
			if(atomicState == 0){
					atomicState = 1;
			}
			else{
				return ERR_SYNTAX;
			}
		}
		else{
			if(atomicState >= 0){
				tokens[token_index] = (Token){'a', atomicValue};
				token_index++;
				atomicState = -1;
				atomicValue = 0;
			}
		}
		
		if(in(buffer[i], _named_atomic)){
			tokens[token_index] = (Token){'a', identifyAtomic(buffer[i])};
			token_index++;
		}
		i++;
	}
	if(atomicState >= 0){
		tokens[token_index] = (Token){.type= 'a', .value= atomicValue};
		token_index++;
	}
	
	
	
	
	*out = 100.5;
	_memoryANS = *out;
	return SUCCESS;
}