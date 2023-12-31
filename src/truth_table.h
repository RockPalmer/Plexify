#ifndef TRUTH_TABLE
#define TRUTH_TABLE

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "truth_values.h"
#include "linked_list.h"
#include "log.h"

class truth_table {
public:
	std::string* inputs;
	std::string* outputs;
	char** inputTable;
	char** outputTable;
	int numInputs;
	int numOutputs;
	int tableHeight;

	truth_table();
	truth_table(std::string* inp, std::string* outp, char** inpTab, char** outpTab, int numInp, int numOutp, int tabHeight);
	truth_table(std::string filename);
	~truth_table();

	linked_list<char*>* get_cases_where(std::string var, char val);
	log* get_sum_of_products(std::string var);
	log* get_product_of_sums(std::string var);
	log* convert_to_log(linked_list<char*>* l, bool is_sop = true);
	void reduce(linked_list<char*>* l);

	int indexOfOutput(std::string var);
};

std::ostream& operator<<(std::ostream& out, truth_table& t);

bool same(char* op1, char* op2, int len);
int find_longest_length_str(std::string* list, int length, int curr_largest);
std::string get_block(std::string str, int width);
std::string get_block(char c, int width);

bool contains(char* container, char* containee, int len);
int containsExcept(char* container, char* containee, int len);
int sameExcept(char* op1, char* op2, int len);
int count_valid(char* vals, int len);

bool find_same_except(linked_list<char*>* l, int num_i);
bool find_contains_except(linked_list<char*>* l, int num_i);

#endif
