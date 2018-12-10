#include "util.h"


unsigned char Normalise(unsigned char v){
	if(v){
		return 0x1;
	}
	else{
		return 0x0;
	}
}

unsigned short NormaliseShort(unsigned short v){
	if(v){
		return 0x1;
	}
	else{
		return 0x0;
	}
}

char In(char c, char* arr){
	while(*arr){
		if(c == *arr){
			return 0xFF;
		}
		arr++;
	}
	return 0x0;	
}

unsigned short IndexOf(char c, char* arr){
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
