/** This class is meant to represent any accurate truth table that the user would like to generate.
 * 
 * This class has 7 data members <inputs>, <outputs>, <inputTable>, <outputTable>, <numInputs>,
 * <numOutputs>, and <tableHeight>
 * 
 * <inputs> and <outputs> are string arrays of size <numInputs> and <numOutputs> respectively. 
 * They store the names of all the input and output variables for the truth table respectively.
 *
 * <inputTable> and <outputTable> are 2D char arrays of size <numInputs> by <tableHeight> and
 * <numOutputs> by <tableHeight> respectively. They store all the rows of input and output values 
 * for the truth table respectively. The value at each index of this 2D array is either HI, LO, 
 * or DC which are macros defined in the "truth_values.h" file.
 *
 * The total amount of input and output variables that can be loaded is compiler-dependent.
 * The maximum number of outputs that can be loaded is set as <SIZE_MAX> and the total number
 * of inputs that can be loaded is set as floor(log2(SIZE_MAX)).
 * 
 * This class is meant to be used to parse CSV files into actual truth tables and run analysis on
 * them. For more information on how the file reading process occurs and formatting requirements
 * for successful file parsing, please see the description for the function
 * "void truth_table::load(std::string filename)" in "truth_table.cpp" */

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

	bool generate_same_except(linked_list<char*>* l1, linked_list<char*>* l2);
	bool find_redundant(linked_list<char*>* l1, linked_list<char*>* l2);

	size_t indexOfOutput(std::string var);
};

std::ostream& operator<<(std::ostream& out, truth_table& t);

size_t find_longest_length_str(std::string* list, size_t length, size_t curr_largest);
std::string get_block(std::string str, size_t width);
std::string get_block(char c, size_t width);

bool can_insert(node<char*>* n, linked_list<char*>* l1, size_t len);
int check_redundant(char* op1, char* op2, size_t len);
size_t sameExcept(char* op1, char* op2, size_t len);
size_t count_valid(char* vals, size_t len);

#endif
