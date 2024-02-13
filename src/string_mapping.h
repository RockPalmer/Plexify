#ifndef STRING_MAP
#define STRING_MAP

#define LO_MAP 0
#define HI_MAP 1
#define DC_MAP 2
#define DELIM_MAP 3
#define INPUT_MAP 4
/*
 * This file defines what strings the program will use to write and read
 * truth table files as well as displaying boolean expressions. This has
 * been put in this central location rather than hard coded so that the
 * values can be easily edited should the need arise
 */

#define OUTPUT_MAP 5
#define AND_MAP 6
#define OR_MAP 7
#define XOR_MAP 8
#define NOT_MAP 9
#define MUX_1_MAP 10
#define MUX_2_MAP 11
#define GROUP_OPEN_MAP 12
#define GROUP_CLOSE_MAP 13

std::string string_mapping[] = {
	"0",		// LOW/false bit
	"1",		// HIGH/true bit
	"x",		// don't care bit
	",",		// truth table file delimiter
	"Inputs",	// Start of input columns label
	"Outputs",	// Start of output columns label
	" & ",		// AND operator
	" | ",		// OR operator
	" ^ ",		// XOR operator
	"~",		// NOT operator
	"?",		// MUX operator 1
	":",		// MUX operator 2
	"(",		// Open grouping symbol
	")"			// Close grouping symbol
};

#endif
