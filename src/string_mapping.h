#ifndef STRING_MAP
#define STRING_MAP

#define LO_MAP 0
#define HI_MAP 1
#define DC_MAP 2
#define DELIM_MAP 3
#define INPUT_MAP 4
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
	"0",
	"1",
	"x",
	",",
	"Inputs",
	"Outputs",
	" & ",
	" | ",
	" ^ ",
	"~",
	"?",
	":",
	"(",
	")"
};

#endif