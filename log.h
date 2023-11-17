#ifndef _LOG_
#define _LOG_

#include "ptr_linked_list.h"
#include "truth_values.h"
#include <string>

struct log {
	char gate;
	char val;
	char type;
	std::string var_name;
	ptr_linked_list<log> args;

	log();
	log(char c, char t);
	log(std::string v);
	~log();

	std::string to_string();
	bool equal_to(log* l);
	ptr_node<log>* strict_contains(log* l);
	bool contains(log* l, bool*& elements, int& len);
	bool contains_same_elements(log* l);
	void copy(log* l);
	bool demorgify();
	bool xorify();
	log* invert();
	bool polarity();
};

#endif