#include "calculatorEngine.h"

// forward typedef
typedef struct Token Token;

struct Token{
	unsigned char t;
	double value;
	Token* left;
	Token* right;
};

Token _tokens[BUFFER_SIZE];
unsigned short _token_index = 0;
Token _empty_token;

double _memoryA = 0;
double _memoryB = 0;
double _memoryC = 0;
double _memoryD = 0;
double _memoryANS = 0;

char _valid_chars[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
																'.', '+', '-', '*', '/', '!', '(', ')', '^',
                                's', 'd', 'c', 'v', 't', 'y', 'r', 'l', 'e',
																'E', 'P', 'A', 'B', 'C', 'D', 'a', 0x00};

char _in_atomic[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '.', 0x00};
char _named_atomic[] = {'E', 'P', 'A', 'B', 'C', 'D', 'a', 0x00};
char _open_bracket[] = {'(', 'e', 0x00};
char _operations[] =         {'-', '+', '/', '*', 'n', '!', '^', 'r', 'l', 'y', 'v', 'd', 't', 'c', 's', 0x00};
char _operation_priority[] = { 0,   0,   1,   1,   2,   3,   4,   5,   5,   6,   6,   6,   6,   6,   6};
char _middle_operator[] = {'-', '+', '/', '*', '^', 0x00};
char _left_one_operator[] = {'y', 'v', 'd', 't', 'c', 's', 0x00}; // n is in special case
char _right_one_operator[] = {'!', 0x00};
char _left_two_operator[] = {'r', 'l', 0x00};


double CharToDouble(char c){
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

double IdentifyAtomic(char a){
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

void StoreMemory(char a, double v){
	switch(a){
		case 'A': _memoryA = v;
		case 'B': _memoryB = v;
		case 'C': _memoryC = v;
		case 'D': _memoryD = v;
		case 'a': _memoryANS = v;
	}
}
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
		
	while(In(_tokens[from].t, _open_bracket) && _tokens[to - 1].t == ')'){
		from++;
		to--;
	}
	
	// repace starting - with negative sign
	if(_tokens[from].t == '-'){
		_tokens[from].t = 'n';
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
		else if(In(_tokens[i].t, _open_bracket)){
			bracket_count--;
		}
		
		if(bracket_count == 0){
			if(In(_tokens[i].t, _operations)){
				// check operand validity
				if(( (i <= from) || // check the left hand side
						 (In(_tokens[i].t, _middle_operator)    && (_tokens[i - 1].t == 'a' || _tokens[i - 1].t == ')' || In(_tokens[i - 1].t, _right_one_operator))) ||
						 (In(_tokens[i].t, _left_one_operator)  && (In(_tokens[i - 1].t, _open_bracket) || In(_tokens[i - 1].t, _middle_operator) || _tokens[i - 1].t == 'n')) ||
						 (In(_tokens[i].t, _left_two_operator)  && (In(_tokens[i - 1].t, _open_bracket) || In(_tokens[i - 1].t, _middle_operator) || _tokens[i - 1].t == 'n')) ||
					   (In(_tokens[i].t, _right_one_operator) && (_tokens[i - 1].t == ')' || _tokens[i - 1].t == 'a' || In(_tokens[i - 1].t, _right_one_operator))))
						&&
					 ( (i >= to - 1) ||
				     (In(_tokens[i].t, _middle_operator)    && (_tokens[i + 1].t == 'a' || In(_tokens[i + 1].t, _open_bracket) || In(_tokens[i + 1].t, _left_one_operator) || In(_tokens[i + 1].t, _left_two_operator) || In(_tokens[i + 1].t, _middle_operator))) ||
						 (In(_tokens[i].t, _left_one_operator)  && (In(_tokens[i + 1].t, _open_bracket))) ||
						 (In(_tokens[i].t, _left_two_operator)  && (In(_tokens[i + 1].t, _open_bracket) || In(_tokens[i + 1].t, _middle_operator) || _tokens[i + 1].t == 'a')) ||
					   (In(_tokens[i].t, _right_one_operator) && (_tokens[i + 1].t == ')' || In(_tokens[i + 1].t, _middle_operator) || In(_tokens[i + 1].t, _right_one_operator))) ||
						 (_tokens[i].t == 'n'                   && (_tokens[i + 1].t == 'a' || In(_tokens[i + 1].t, _open_bracket) || In(_tokens[i + 1].t, _left_one_operator) || In(_tokens[i + 1].t, _left_two_operator) || In(_tokens[i + 1].t, _middle_operator))))){
					priority = _operation_priority[IndexOf(_tokens[i].t, _operations)];
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
	if(In((*out)->t, _middle_operator)){
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
	else if(In((*out)->t, _left_one_operator)){
		if(selectedIndex != from){
			return ERR_SYNTAX;
		}
		leftFrom = selectedIndex + 1;
		leftTo = to;

		err = BuildTree(leftFrom, leftTo, &((*out)->left));
		if(err){
			return err;
		}
		(*out)->right = &_empty_token;
	}
	else if(In((*out)->t, _right_one_operator)){
		if(selectedIndex != to - 1){
			return ERR_SYNTAX;
		}
		leftFrom = from;
		leftTo = selectedIndex;
		err = BuildTree(leftFrom, leftTo, &((*out)->left));
		if(err){
			return err;
		}
		(*out)->right = &_empty_token;
	}
	else if(In((*out)->t, _left_two_operator)){
		if(selectedIndex != from){
			return ERR_SYNTAX;
		}
		leftFrom = selectedIndex + 1;
		i = leftFrom;
		while(_tokens[i].t != 'e' && i < to){
			i++;
		}
		if(i >= to){
			return ERR_SYNTAX;
		}
		if(i > leftFrom + 2){
			return ERR_SYNTAX; // more than one token in L2 arg
		}
		
		leftTo = i;
		rightFrom = i;
		rightTo = to;
		
		if(leftFrom < leftTo){		
			err = BuildTree(leftFrom, leftTo, &((*out)->left));
			if(err){
				return err;
			}
		}
		else{
			(*out)->left = &_empty_token;
		}
		err = BuildTree(rightFrom, rightTo, &((*out)->right));
		if(err){
			return err;
		}
	}
	else if((*out)->t == 'n'){
		if(selectedIndex != from){
			return ERR_SYNTAX;
		}
		leftFrom = selectedIndex + 1;
		leftTo = to;

		err = BuildTree(leftFrom, leftTo, &((*out)->left));
		if(err){
			return err;
		}
		(*out)->right = &_empty_token;
	}
	
	return SUCCESS;	
}


char EvalTree(Token* root, double *out){
	double arg1;
	double arg2;
	double val;
	char err;
	
	if(root->t == 'a'){
		*out = root->value;
		return SUCCESS;
	}
	else if(root->t == '_'){
		val = Maths(root->t, 0, 0);
		*out = val;
		return SUCCESS;
	}
	else if(root == 0x0){
		val = Maths('_', 0, 0);
		*out = val;
		return SUCCESS;
	}
	else if(In(root->t, _operations)){
		err = EvalTree(root->left, &arg1);
		if(err){
			return err;
		}
		err = EvalTree(root->right, &arg2);
		if(err){
			return err;
		}
		val = Maths(root->t, arg1, arg2);
		if(IsNand(val)){
			return ERR_MATHS;
		}
		*out = val;
		return SUCCESS;
	}
	else{
		return ERR_SYNTAX;
	}
	
}


char AddToken(unsigned char t, double value){
	_tokens[_token_index].t = t;
	_tokens[_token_index].value = value;
	_token_index++;
	if(_token_index > BUFFER_SIZE){
		return ERR_BUFFER_OVERFLOW;
	}
	return SUCCESS;
}


unsigned char Evaluate(char *buffer, unsigned short index, double *out){
	unsigned short i = 0;
	short atomic_state = -1;
	double atomic_value = 0;
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
		if(!In(buffer[i], _valid_chars)){
			return ERR_INTERNAL;
		}
		
		in_atomic = In(buffer[i], _in_atomic);
		if(buffer[i] == '.'){
			if(atomic_state == 0){
					atomic_state = 1;
			}
			else{
				return ERR_SYNTAX;
			}
		}
		else if(in_atomic){
			if(atomic_state <= 0){
				atomic_value = atomic_value * 10 + CharToDouble(buffer[i]);
				atomic_state = 0;
			}
			else{
				atomic_value += powd(10, -atomic_state) * CharToDouble(buffer[i]);
				atomic_value = roundd(atomic_value, atomic_state);
				atomic_state++;
			}
		}
		else{
			if(atomic_state >= 0){
				err = AddToken('a', atomic_value);
				if(err){
					return err;
				}
				atomic_state = -1;
				atomic_value = 0;
			}
		}
		in_named_atomic = In(buffer[i], _named_atomic);
		if(in_named_atomic){
			err = AddToken('a', IdentifyAtomic(buffer[i]));
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
	if(atomic_state >= 0){
		err = AddToken('a', atomic_value);
		if(err){
			return err;
		}
	}
	
	for(i = 0; i < _token_index; i++){
		if(In(_tokens[i].t, _open_bracket)){
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
	
	if (err == SUCCESS){
		err = EvalTree(root, out);
	}
	if(err){
		return err;
	}
	
	if(absd(*out) > MAX_VALUE){
		return ERR_NUMBER_TOO_LARGE;
	}
	else if(absd(*out) < MIN_VALUE){
		return ERR_NUMBER_TOO_SMALL;
	}
	
	StoreMemory('a', *out);
	return SUCCESS;
}
