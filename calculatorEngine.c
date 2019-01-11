#include "calculatorEngine.h"

// forward typedef for Token struct
typedef struct Token Token;

// definition for the Token struct
struct Token{
	unsigned char t; // type of the Token
	double value;		 // value of the Token
	Token* left;     // Token left and right from this in the tree
	Token* right;
};

Token _tokens[BUFFER_SIZE]; // evaluation buffer
unsigned short _token_index = 0;
Token _empty_token; // placeholder empty token
Token _e_token;			// token used if log has no base
Token _2_token;     // token used if root has no base

double _memoryA = 0;
double _memoryB = 0;
double _memoryC = 0;
double _memoryD = 0;
double _memoryANS = 0;

// these are the only chars that are valid as inputs
char _valid_chars[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
											 '.', '+', '-', '*', '/', '!', '(', ')', '^',
                       's', 'd', 'c', 'v', 't', 'y', 'r', 'l', 'e',
											 'E', 'P', 'A', 'B', 'C', 'D', 'a', 0x00};

char _in_atomic[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '.', 0x00}; // chars that can be inside an atomic number
char _named_atomic[] = {'E', 'P', 'A', 'B', 'C', 'D', 'a', 0x00}; // values that have a number assigned to them
char _open_bracket[] = {'(', 'e', 0x00}; // characters symboling an open bracket
char _operations[] =         {'-', '+', '/', '*', 'n', '!', '^', 'r', 'l', 'y', 'v', 'd', 't', 'c', 's', 0x00}; // all operators
char _operation_priority[] = { 0,   0,   1,   1,   2,   3,   4,   5,   5,   6,   6,   6,   6,   6,   6};        // priority for the operators for order of operations
char _middle_operator[] = {'-', '+', '/', '*', '^', 0x00}; // operators that have their evaluated values on the two side
char _left_one_operator[] = {'y', 'v', 'd', 't', 'c', 's', 0x00}; // operators that have one value to be evaluated to their right
char _right_one_operator[] = {'!', 0x00};                         // operators that have one value to be evaluated to their left
char _left_two_operator[] = {'r', 'l', 0x00};                     // operators that have two values to be evaluated to their right, separated by a special open bracket

void LoadMemory(void){
	_memoryA = 0;
	_memoryB = 0;
	_memoryC = 0;
	_memoryD = 0;
	_memoryANS = 0;
}

double CharToDouble(char c){
	// force char into a number
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
	// Turn a named atomic value into a double
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
		case 'A': _memoryA = v; break;
		case 'B': _memoryB = v; break;
		case 'C': _memoryC = v; break;
		case 'D': _memoryD = v; break;
		case 'a': _memoryANS = v;
	}
}
char BuildTree(unsigned short from, unsigned short to, Token** out){
	// Recursive function to build a tree of Tokens out of a section of the Token buffer
	// section marked with [from, to)
	// out carries the root token
	// return an error code
	int bracket_count = 0;
	int i;
	Token* selectedOperand;
	unsigned short selectedPriority = 15;
	int selectedIndex;
	unsigned short priority;
	unsigned short atomic_count = 0;
	Token* foundAtomic;
	unsigned char leftFrom;
	unsigned char leftTo;
	unsigned char rightFrom;
	unsigned char rightTo;
	char err;
	
	// remove brackets from either side
	while(In(_tokens[from].t, _open_bracket) && _tokens[to - 1].t == ')'){
		from++;
		to--;
	}
	
	// repace starting - with negative sign
	if(_tokens[from].t == '-'){
		_tokens[from].t = 'n';
	}
	
	// if length of the section is 0 or lower
	if(from >= to){
		return ERR_SYNTAX;
	}
	// if only one token and is an atomic return it as the root
	if(to - 1 == from){
		if(_tokens[from].t == 'a'){
			*out = &_tokens[from];
			return SUCCESS;
		}
		else{
			return ERR_SYNTAX;
		}
	}
	
	// go through the tokens in the section
	for(i = to - 1; i >= from;){
		// keep track of the depth of brackets we're in
		if(_tokens[i].t == ')'){
			bracket_count++;
		}
		else if(In(_tokens[i].t, _open_bracket)){
			bracket_count--;
		}
		
		// if not inside a bracket
		if(bracket_count == 0){
			if(In(_tokens[i].t, _operations)){
				// check operand validity
				if(( (i <= from) || // check the left hand side
						 // check middle operators, that it must have an atomic, an close bracket or a right operator to the left of it
						 (In(_tokens[i].t, _middle_operator)    && (_tokens[i - 1].t == 'a' || _tokens[i - 1].t == ')' || In(_tokens[i - 1].t, _right_one_operator))) ||
					   // check left one operator, that it must have an open bracket, middle operator or negate sign to the left of it
						 (In(_tokens[i].t, _left_one_operator)  && (In(_tokens[i - 1].t, _open_bracket) || In(_tokens[i - 1].t, _middle_operator) || _tokens[i - 1].t == 'n')) ||
				     // check left two operator, that it must have an open bracket, middle operator or negate sign to the left of it
						 (In(_tokens[i].t, _left_two_operator)  && (In(_tokens[i - 1].t, _open_bracket) || In(_tokens[i - 1].t, _middle_operator) || _tokens[i - 1].t == 'n')) ||
						 // check right one operator, that it must have a close bracket, atomic right one operator to the left of it
					   (In(_tokens[i].t, _right_one_operator) && (_tokens[i - 1].t == ')' || _tokens[i - 1].t == 'a' || In(_tokens[i - 1].t, _right_one_operator))))
						&&
					 ( (i >= to - 1) || // check the right hand side
						 // check middle operator, that it must have an atomic, open bracket, left one, left two or middle operator to the right of it
				     (In(_tokens[i].t, _middle_operator)    && (_tokens[i + 1].t == 'a' || In(_tokens[i + 1].t, _open_bracket) || In(_tokens[i + 1].t, _left_one_operator) || In(_tokens[i + 1].t, _left_two_operator) || In(_tokens[i + 1].t, _middle_operator))) ||
				     // check left one operator, that it must have an open bracket to the right of it
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
			if((*out)->t == 'r'){
				(*out)->left = &_2_token;
			}
			else if((*out)->t == 'l'){
				(*out)->left = &_e_token;
			}
			else{
				(*out)->left = &_empty_token;
			}
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
	_e_token.t = 'a';
	_e_token.value = E;
	_2_token.t = 'a';
	_2_token.value = 2;
	
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
	else if(absd(*out) > 0 && absd(*out) < MIN_VALUE){
		return ERR_NUMBER_TOO_SMALL;
	}
	
	StoreMemory('a', *out);
	return SUCCESS;
}
