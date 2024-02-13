#ifndef PGM_ERROR
#define PGM_ERROR

#include <cstdarg>
#include <string>

enum error_type {
	NONE_ERROR,
	TOO_MANY_INPUTS,
	TOO_MANY_OUTPUTS,
	NOT_ENOUGH_INPUTS,
	NOT_ENOUGH_OUTPUTS,
	NOT_ENOUGH_ROWS,
	MISSING_ARG,
	TOO_MANY_ARGS,
	TOO_MANY_ROWS,
	UNRECOGNIZED_CHAR,
	NOT_FOUND,
	DUPLICATE_ARG
};

struct pgm_error {
	error_type type;
	int num_size_args; // Defines the size of size_args[]
	int num_str_args; // Defines the size of str_args[]
	size_t* size_args;
	std::string* str_args;

	pgm_error();
	pgm_error(error_type t, int num_int, int num_str);
	~pgm_error();

	void setSizeArgs(size_t arg,...);
	void setStrArgs(std::string args[]);
};

#endif
