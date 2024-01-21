#ifndef _LOG_
#define _LOG_

#include "ptr_linked_list.h"
#include "truth_values.h"
#include <string>

struct log {
	char gate;
	char val;
	log_type type;
	std::string var_name;
	ptr_linked_list<log> args;

	log();
	log(char c, log_type t);
	log(std::string v);
	~log();

	std::string to_string();
	ptr_node<log>* strict_contains(log* l);
	log* invert();
	bool equal_to(log* l);
	bool contains(log* l, bool*& elements, size_t& len);
	bool contains_same_elements(log* l);
	bool demorgify();
	bool xorify();
	bool polarity();
	bool can_xor(log* l);
	void invert_this();
	void copy(log* l);
	//void xor_with(ptr_node<log>* column1, ptr_node<log>* column2, ptr_node<log>* row);
};

#endif
