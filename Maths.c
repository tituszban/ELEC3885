#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define pi 3.14159265358979323846f
#define e  2.71828182845904523536f

float consts[] = {pi,e};

////        arg1   arg2
float  sind(float,float); // sin(arg1)
float asind(float,float); // asin(arg1)
float  fact(float,float); // arg1!
float  cosd(float,float); // cos(arg1)
float acosd(float,float); // acos(arg1)
float  tand(float,float); // tan(arg1)
float atand(float,float); // atan(arg1)
float  root(float,float); // arg1 ^ ( 1 / arg2)
float sqrtd(float,float); // arg1 ^ (1 / 2)
float  logd(float,float); // log arg1 (arg2)
float  powd(float,float); // arg1 ^ arg2
float   neg(float,float); // -arg1
float  divd(float,float); // arg1 / arg2
float  mult(float,float); // arg1 * arg2
float  plus(float,float); // arg1 + arg2
float minus(float,float); // arg1 - arg2
float noop(float,float);  // NAN

typedef float (*ops)(float arg1, float arg2);

ops functionLookup(char op){
	switch(op){
			case 's': return sind;
			case 'd': return asind;
			case '!': return fact;
			case 'c': return cosd;
			case 'v': return acosd;
			case 't': return tand; 
			case 'y': return atand;
			case 'r': return root;
			case 'l': return logd;
			case '/': return divd;
			case '*': return mult;
			case '+': return plus;
			case '-': return minus;
		  case '^': return powd;
		  case 'n': return neg;
		  case '_': return noop;
			default: return noop;
		}
}

float maths(char op,float arg1, float arg2) {

	static float ans;
	
	ans = functionLookup(op)(arg1,arg2);
	
	if (isinf(ans)){ // If the answer is infinite set it to 'not a number'
		ans = NAN;
	}
	
	return ans;
	
}

float absf(float v){
	if(v < 0){
		return -v;
	}
	return v;
}

char isInt(float n, int* out){
	float tolerance = 1e-7;
	if(absf(n - roundf(n)) < tolerance){
			*out = (int)roundf(n);
			return 0xFF;
	}
	return 0x00;
}

float constants(int element){
	return consts[element];
}

float noop(float arg1, float arg2){
	return NAN;
}

float sind(float arg1, float arg2){
	return sinf(arg1);
}

float asind(float arg1, float arg2){
	return asinf(arg1);
}

float fact(float arg1, float arg2){
	int n = 0;
	if(!isInt(arg1, &n) || arg1 < 1){
		return NAN;
	}
	if (n == 1 || n == 0){
		return 1;
	} else {
		return n*fact(n-1,0);
	}
}

float cosd(float arg1, float arg2){
	return cosf(arg1);
}

float acosd(float arg1, float arg2){
	return acosf(arg1);
}

float tand(float arg1, float arg2){
	return tanf(arg1);
}

float atand(float arg1, float arg2){
	return atanf(arg1);
}

float root(float arg1, float arg2){
	return powf(arg2,1/arg1);
}

float sqrtd(float arg1, float arg2){
	return sqrtf(arg1);
}

float logd(float arg1, float arg2){
	return (logf(arg2)/logf(arg1));
}

float  powd(float arg1, float arg2){
	return (powf(arg1,arg2));
}

float   neg(float arg1, float arg2){
	return (-arg1);
}

float divd(float arg1,float arg2){
	return (arg1 / arg2);
}

float  mult(float arg1,float arg2){
	return (arg1 * arg2);
}

float  plus(float arg1,float arg2){
	return (arg1 + arg2);
}

float minus(float arg1,float arg2){
	return (arg1 - arg2);
}