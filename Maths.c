#include "Maths.h"

////        arg1   arg2
double  sind(double,double); // sin(arg1)
double asind(double,double); // asin(arg1)
double  fact(double,double); // arg1!
double  cosd(double,double); // cos(arg1)
double acosd(double,double); // acos(arg1)
double  tand(double,double); // tan(arg1)
double atand(double,double); // atan(arg1)
double  root(double,double); // arg1 ^ ( 1 / arg2)
double sqrtd(double,double); // arg1 ^ (1 / 2)
double  logd(double,double); // log arg1 (arg2)
double  powd(double,double); // arg1 ^ arg2
double   neg(double,double); // -arg1
double  divd(double,double); // arg1 / arg2
double  mult(double,double); // arg1 * arg2
double  plus(double,double); // arg1 + arg2
double minus(double,double); // arg1 - arg2
double noop(double,double);  // NAN

typedef double (*ops)(double arg1, double arg2);

ops FunctionLookup(char op){
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

double Maths(char op,double arg1, double arg2) {

	static double ans;
	
	ans = FunctionLookup(op)(arg1,arg2);
	
	if (isinf(ans)){ // If the answer is infinite set it to 'not a number'
		ans = NAN;
	}
	
	return ans;
	
}

double absf(double v){
	if(v < 0){
		return -v;
	}
	return v;
}

char IsInt(double n, int* out){
	double tolerance = 1e-7;
	if(absf(n - roundf(n)) < tolerance){
			*out = (int)roundf(n);
			return 0xFF;
	}
	return 0x00;
}

char IsNand(double v){
	return isnan(v);
}

double noop(double arg1, double arg2){
	return NAN;
}

double sind(double arg1, double arg2){
	return sinf(arg1);
}

double asind(double arg1, double arg2){
	return asinf(arg1);
}

double fact(double arg1, double arg2){
	int n = 0;
	if(!IsInt(arg1, &n) || arg1 < 1){
		return NAN;
	}
	if (n == 1 || n == 0){
		return 1;
	} else {
		return n*fact(n-1,0);
	}
}

double cosd(double arg1, double arg2){
	return cosf(arg1);
}

double acosd(double arg1, double arg2){
	return acosf(arg1);
}

double tand(double arg1, double arg2){
	return tanf(arg1);
}

double atand(double arg1, double arg2){
	return atanf(arg1);
}

double root(double arg1, double arg2){
	return powf(arg2,1/arg1);
}

double sqrtd(double arg1, double arg2){
	return sqrtf(arg1);
}

double logd(double arg1, double arg2){
	return (logf(arg2)/logf(arg1));
}

double  powd(double arg1, double arg2){
	return (powf(arg1,arg2));
}

double   neg(double arg1, double arg2){
	return (-arg1);
}

double divd(double arg1,double arg2){
	return (arg1 / arg2);
}

double  mult(double arg1,double arg2){
	return (arg1 * arg2);
}

double  plus(double arg1,double arg2){
	return (arg1 + arg2);
}

double minus(double arg1,double arg2){
	return (arg1 - arg2);
}

double absd(double arg1){
	return absf(arg1);
}

double roundd(double arg1, int decimal){
	double d = powd(10, decimal);
	double id = powd(10, -decimal);
	arg1 = round(arg1 * d) * id;
	return arg1;
}
