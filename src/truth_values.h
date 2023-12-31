#ifndef TRUTH_VALUES
#define TRUTH_VALUES

#define NONE 0
#define NOT 1
#define AND 2
#define NAND 3
#define OR 4
#define NOR 5
#define XOR 6
#define XNOR 7
#define MUX 8

#define HI '1'
#define LO '0'
#define DC 'x'

enum log_type {
	LITERAL = 1,
	VARIABLE = 2,
	EXPRESSION = 3
};

#endif
