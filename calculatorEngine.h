#ifndef CALCULATORENGINE_H_
#define CALCULATORENGINE_H_

#include "calculator.h"
#include "util.h"

#define E  2.7182818284590452353602874
#define PI 3.1415926535897932384626433

unsigned char Evaluate(char *buffer, unsigned short index, double *out);
void StoreMemory(char a, double v);

#endif
