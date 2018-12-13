#ifndef CALCULATOR_H_
#define CALCULATOR_H_


#include "calculatorEngine.h"
#include "util.h"
#include "LCD.h"
#include "PLL.h"
#include "keypad.h"
#include "Maths.h"

#define BUFFER_SIZE 256 // Up to 65,535

#define MIN_VALUE 1E-13
#define MAX_VALUE 1E13

#define SUCCESS 0x00
#define ERR_INTERNAL 0x01
#define ERR_BUFFER_OVERFLOW 0x02
#define ERR_SYNTAX 0x03
#define ERR_MATHS 0x04

#define ERR_NUMBER_TOO_LARGE 0x05
#define ERR_NUMBER_TOO_SMALL 0x06

#define MODE_BIT 0x01
#define EQ_BIT 0x02

#define BACK_HOLD_THRESHOLD 40

void Update(void);
unsigned char CountPressed(unsigned short p);
unsigned char InputToNum(unsigned short input, unsigned char mode, unsigned char eq);

#endif
