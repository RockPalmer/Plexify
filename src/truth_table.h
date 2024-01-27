#ifndef TRUTH_TABLE
#define TRUTH_TABLE

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "helper_functions.h"
#include "string_mapping.h"
#include "pgm_error.h"
#include "truth_values.h"
#include "linked_list.h"
#include "log.h"

class truth_table {
public:
	std::string* inputs;
	std::string* outputs;
	char** inputTable;
	char** outputTable;
	size_t numInputs;
	size_t numOutputs;
	size_t tableHeight;

	truth_table();
	truth_table(std::string* inp, std::string* outp, char** inpTab, char** outpTab, size_t numInp, size_t numOutp, size_t tabHeight);
	truth_table(std::string filename);
	~truth_table();

	linked_list<char*>* get_cases_where(std::string var, char val);
	log* get_sum_of_products(std::string var);
	log* get_product_of_sums(std::string var);
	log* convert_to_log(linked_list<char*>* l, bool is_sop = true);
	void load(std::string filename);
	void destruct();
	void reduce(linked_list<char*>* l1, linked_list<char*>* l2);
	void expand(linked_list<char*>* l);

	bool find_same(linked_list<char*>* l1, linked_list<char*>* l2);
	bool find_same_except(linked_list<char*>* l1, linked_list<char*>* l2, bool simplify = false);
	bool find_contains_except(linked_list<char*>* l1, linked_list<char*>* l2);
	bool find_contains(linked_list<char*>* l1, linked_list<char*>* l2);

	size_t indexOfOutput(std::string var);
};

std::ostream& operator<<(std::ostream& out, truth_table& t);

bool same(char* op1, char* op2, size_t len);
size_t find_longest_length_str(std::string* list, size_t length, size_t curr_largest);
std::string get_block(std::string str, size_t width);
std::string get_block(char c, size_t width);

bool contains(char* container, char* containee, size_t len);
size_t containsExcept(char* container, char* containee, size_t len);
size_t sameExcept(char* op1, char* op2, size_t len);
size_t count_valid(char* vals, size_t len);

void print(linked_list<char*>* l, size_t num_i);
void print(node<char*>* n, size_t num_i);
void printSpecial(std::string str);

#endif
