#ifndef TRUTH_TABLE
#define TRUTH_TABLE

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "truth_values.h"
#include "linked_list.h"
#include "log.h"

using namespace std;

struct instruction {
	string output;
	bool is_choice;
	instruction* choice[2];

	instruction();

	~instruction();
};

class truth_table {
public:
	string* inputs;
	string* outputs;
	char** inputTable;
	char** outputTable;
	int numInputs;
	int numOutputs;
	int tableHeight;

	truth_table();
	truth_table(string* inp, string* outp, char** inpTab, char** outpTab, int numInp, int numOutp, int tabHeight);
	truth_table(string filename);
	~truth_table();

	linked_list<char*>* get_cases_where(string var, char val);

	int indexOfOutput(string var);
	string to_string();
};


bool same(char* op1, char* op2, int len);
int find_longest_length_str(string* list, int length, int curr_largest);
string get_block(string str, int width);
string get_block(char c, int width);

bool contains(char* container, char* containee, int len);
int containsExcept(char* container, char* containee, int len);
int sameExcept(char* op1, char* op2, int len);
int count_valid(char* vals, int len);
log* convert_to_log(linked_list<char*>* l, truth_table& t);

bool find_same_except(linked_list<char*>* l, int num_i);
bool find_contains_except(linked_list<char*>* l, int num_i);
void reduce(linked_list<char*>* l, int len);

#endif