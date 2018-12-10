#include "keypad.h"
#include "calculator.h"
#include "PLL.h"
#include "LCD.h"
#include "EEPROM.h"
#include "Login.h"

/* STYLE GUIDE
Functions:
	Use CamelCase for the names of functions, with the first letter capitalised.
	Exceptions:
		init functions: CamelCase for the name, followed by _Init
		maths functions: follow the std c naming with all lower case, followed by a d, always with two arguments. eg.: logd(double a, double b)
Variable types:
	Use the smallest type that will still do the job.
	Use unsigned values except in for loops
	Instead of boolean values use unsigned char with 0x00 as False and 0xFF as True (so !True = False)
	(unsigned) char:  8 bits
	(unsigned) short: 16 bits
	(unsigned) int: 	32 bits
	float:						32 bits
	double:						64 bits
Global private variables:
	Each variable must only be used in a single c file. In the case of cross file use of a value, use a globally declared function to manipulate it.
	Variables declared on the file level must use _snake_case with a starting _ underscore.
Local variables:
	Variables must be declared at the start of the function, using camel_case
Constants:
	Constants should be created in .h files using the #define directive and named in ALL_CAPS_CAMEL_CASE
.h files:
	Keep .h files clear and only have functions that are used by other files (public).
	Treat every other function as private/internal functions
*/

int main(void){
	Keypad_Init();
	PLL_Init();
	Lcd_Init();
	EEPROM_Init();
	
	Login();
	
	while(1){
		Update();
	}
}
