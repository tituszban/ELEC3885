#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define pi 3.14159265358979323846f
#define e  2.71828182845904523536f

float consts[] = {pi,e};

////        arg1   arg2
float  sind(float,float); // 0 sin(arg1)
float asind(float,float); // 1 asin(arg1)
float  fact(float,float); // 2 arg1!
float  cosd(float,float); // 3 cos(arg1)
float acosd(float,float); // 4 acos(arg1)
float  tand(float,float); // 5 tan(arg1)
float atand(float,float); // 6 atan(arg1)
float  root(float,float); // 7 arg1 ^ ( 1 / arg2)
float sqrtd(float,float); // 8 arg1 ^ (1 / 2)
float  logd(float,float); // 9 log arg1 (arg2)
float  divd(float,float); //10 arg1 / arg2
float  mult(float,float); //11 arg1 * arg2
float  plus(float,float); //12 arg1 + arg2
float minus(float,float); //13 arg1 - arg2

float (*ops[])(float arg1, float arg2) = {sind,asind,fact,cosd,acosd,tand,atand,root,sqrtd,logd,divd,mult,plus,minus};

float maths(int op,float arg1, float arg2) {

	static float ans;
	
	ans = ops[op](arg1,arg2);
	
	if (isinf(ans)){ // If the answer is infinite set it to 'not a number'
		ans = NAN;
	}
	
	return ans;
	
}
float constants(int element){
	return consts[element];
}

float sind(float arg1, float arg2){
	return sinf(arg1);
}

float asind(float arg1, float arg2){
	return asinf(arg1);
}

float fact(float arg1, float arg2){
	if (arg1 == 2){
		return 2;
	} else {
		return arg1*fact(arg1-1,0);
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