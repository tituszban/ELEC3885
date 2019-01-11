#include "util.h"


unsigned char Normalise(unsigned char v){ // normalise any char into a 0x00 or 0x01
	if(v){
		return 0x1;
	}
	else{
		return 0x0;
	}
}

unsigned short NormaliseShort(unsigned short v){ // normalise any short into an 0x00 or 0x01
	if(v){
		return 0x1;
	}
	else{
		return 0x0;
	}
}

char In(char c, char* arr){ // check if char is in null ended char array
	while(*arr){
		if(c == *arr){
			return 0xFF;
		}
		arr++;
	}
	return 0x0;	
}

unsigned short IndexOf(char c, char* arr){ // get index of char in null ended char array
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
