/** This file defines global variables that are used throughout the program */

#ifndef TRUTH_VALUES
#define TRUTH_VALUES

// All available logic gates
#define NONE 0
#define NOT 1
#define AND 2
#define NAND 3
#define OR 4
#define NOR 5
#define XOR 6
#define XNOR 7
#define MUX 8

// All possible "truth values" for a given bit
#define HI '1'
#define LO '0'
#define DC 'x'

// All possible types of logical expressions
enum log_type {
	LITERAL = 1,
	VARIABLE = 2,
	EXPRESSION = 3
};

#endif
