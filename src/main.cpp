#include <iostream>
#include <string>

#define NUM_CMDS 7
#define NUM_INVALID_FNAME_CHARS 9

int iteration = 0;

#include "string_mapping.h"
#include "truth_table.h"
#include "pgm_error.cpp"
#include "truth_table.cpp"
#include "linked_list.cpp"
#include "ptr_linked_list.h"
#include "ptr_linked_list.cpp"
#include "log.cpp"

using namespace std;

char invalid_filename_chars[] = {
	'<',
	'>',
	':',
	'"',
	'/',
	'\\',
	'|',
	'?',
	'*'
};

string help_strings[] = {
	"	generate 	- Generates a truth table CSV file after being provided the filename the input variables and the output variables.",
	"	load 		- Loads a truth table CSV file into memory. This command must be run before performimg 'print', 'sop', or 'pos'.",
	"	print 		- Prints the current truth table loaded into memory.",
	"	sop 		- Gets the 'Sum of Products' boolean equation after being given a variable name and loads this equation into memory.",
	"	pos 		- Gets the 'Product of Sums' boolean equation after being given a variable name and loads this equation into memory.",
	"	quit 		- Ends the program."
};
string valid_cmds[] = {
	"generate",
	"quit",
	"load",
	"print",
	"sop",
	"pos",
	"help"
};
const string lineBreak = "-----------------------------------------------------------------------------------------";
bool equal(string str1, string str2);
bool is_numeric(string str);
bool is_whitespace(string str);
bool contains_chars(string str, char chars[], int len);
int map(string str, string strs[], int len);
void doThing();
void explode(string str, string delim, string*& strs, size_t& len, size_t max_size);
void printTableInfo(string filename, string* input_vars, string* output_vars, size_t number_inputs, size_t number_outputs);
void parseExp(string line, size_t*& terms, size_t& len, size_t max_size);

int main() {
	doThing();
	cout << "Program terminated" << endl;
}

void doThing() {

	bool keep_going = true;
	bool show_header = true;
	bool table_loaded = false;
	string line = "";
	string fname = "";
	string focus_var = "";
	truth_table t;
	log* equation = nullptr;

	cout << "Welcome! Please enter a command below. If you want to see a list of commands, type 'help' and press ENTER." << endl;
	while (keep_going) {
		if (show_header) {
			cout << lineBreak << endl;
			cout << "Current table    : ";
			if (table_loaded) {
				cout << fname << endl;
			} else {
				cout << "Nothing loaded" << endl;
			}
			cout << "Current equation : ";
			if (equation == nullptr) {
				cout << "Nothing loaded" << endl;
			} else {
				cout << focus_var << " = " << equation->to_string() << endl;
			}
			cout << lineBreak << endl;
			show_header = false;
		}
		cout << ">> ";
		getline(cin,line);

		line = trim(line);

		if (line.compare("") != 0) {
			show_header = true;

			if (line.compare("generate") == 0) { // Generate new file
				string filename = "";
				size_t number_inputs = 0;
				size_t number_outputs = 0;
				string* input_vars = nullptr;
				string* output_vars = nullptr;
				int state = 0;
				while (state >= 0) {
					printTableInfo(filename,input_vars,output_vars,number_inputs,number_outputs);
					switch (state) {
						case 0: // Set file name
							cout << "Please enter the filename below that you would like to write the truth table into or press ENTER to go back." << endl;
							cout << ">> ";
							getline(cin,filename);
							filename = trim(filename);
							if (filename.compare("") == 0) {
								state = -1;
							} else {
								if (contains_chars(filename,invalid_filename_chars,NUM_INVALID_FNAME_CHARS)) {
									cout << "This filename is invalid." << endl;
									filename = "";
								} else {
									state = 1;
								}
							}
							break;
						case 1: // Set input variables
							cout << "Please enter the name(s) of the input variables you would like to use for this truth table separated by commas or press ENTER to go back." << endl;
							cout << ">> ";
							getline(cin,line);
							line = trim(line);
							if (line.compare("") == 0) {
								filename = "";
								state = 0;
							} else {
								explode(line,",",input_vars,number_inputs,64);
								bool is_valid = true;
								if (number_inputs != -3) {
									for (size_t i = 0; i < number_inputs && is_valid; i++) {
										if (is_whitespace(input_vars[i])) {
											delete[] input_vars;
											input_vars = nullptr;
											number_inputs = 0;
											is_valid = false;
											cout << "No variable entered in slot #" << i << "." << endl;
										} else {
											input_vars[i] = trim(input_vars[i]);
										}
									}
									if (is_valid) {
										state = 2;
									}
								} else {
									cout << "Too many input variables entered. The maximum allowed amount is 64." << endl;
								}
							}
							break;
						case 2: // Set output variables
							cout << "Please enter the name(s) of the output variables you would like to use for this truth table separated by commas or press ENTER to go back." << endl;
							cout << ">> ";
							getline(cin,line);
							line = trim(line);
							if (line.compare("") == 0) {
								delete[] output_vars;
								output_vars = nullptr;
								number_outputs = 0;
								state = 1;
							} else {
								explode(line,",",output_vars,number_outputs,SIZE_MAX);
								if (number_outputs != -3) {
									bool is_valid = true;
									for (size_t i = 0; i < number_outputs && is_valid; i++) {
										if (is_whitespace(output_vars[i])) {
											delete[] output_vars;
											output_vars = nullptr;
											number_outputs = 0;
											is_valid = false;
											cout << "No variable entered in slot #" << i << "." << endl;
										} else {
											output_vars[i] = trim(output_vars[i]);
										}
									}
									if (is_valid) {
										state = -2;
									}
								} else {
									cout << "Too many output variables entered. The maximum allowed amount is " << SIZE_MAX << "." << endl;
								}
							}
							break;
					}
				}
				if (state == -2) {
					if (filename.substr(filename.length()-4).compare(".csv") != 0) {
						filename = filename + ".csv";
					}
					ofstream outfile;
					outfile.open(filename);
					outfile << string_mapping[INPUT_MAP] << ",";
					for (size_t i = 1; i < number_inputs; i++) {
						outfile << string_mapping[DELIM_MAP];
					}
					outfile << string_mapping[OUTPUT_MAP];
					for (size_t i = 1; i < number_outputs; i++) {
						outfile << string_mapping[DELIM_MAP];
					}
					outfile << endl;

					for (size_t i = 0; i < number_inputs; i++) {
						if (i != 0) {
							outfile << string_mapping[DELIM_MAP];
						}
						outfile << input_vars[i];
					}
					for (size_t i = 0; i < number_outputs; i++) {
						outfile << string_mapping[DELIM_MAP] << output_vars[i];
					}

					long long finished_num = (1 << number_inputs);
					long long current_num = 0;
					while (current_num != finished_num) {
						outfile << endl;
						for (size_t i = 0; i < number_inputs; i++) {
							if (i != 0) {
								outfile << string_mapping[DELIM_MAP];
							}
							if (current_num & (1 << (number_inputs - i - 1))) {
								outfile << string_mapping[HI_MAP];
							} else {
								outfile << string_mapping[LO_MAP];
							}
						}
						for (size_t i = 0; i < number_outputs; i++) {
							outfile << string_mapping[DELIM_MAP] << string_mapping[LO_MAP];
						}
						current_num++;
					}

					outfile.close();
					state = -1;

					cout << "File generated successfully!" << endl;
					printTableInfo(filename,input_vars,output_vars,number_inputs,number_outputs);
				}
				if (input_vars != nullptr) {
					delete[] input_vars;
					input_vars = nullptr;
				}
				if (output_vars != nullptr) {
					delete[] output_vars;
					output_vars = nullptr;
				}
				number_inputs = 0;
				number_outputs = 0;
			} else if (line.compare("quit") == 0) { // Quit
				keep_going = false;
			} else if (line.compare("load") == 0) { // Load file
				int state = 0;
				string filename = "";
				while (state >= 0) {
					switch (state) {
						case 0:
							filename = "";
							cout << "Please enter the name of the file you would like to load or press ENTER to go back." << endl;
							cout << ">> ";
							getline(cin,filename);
							filename = trim(filename);
							if (filename.compare("") == 0) {
								state = -1;
							} else {
								ifstream infile;
								if (filename.substr(filename.length()-4).compare(".csv") != 0) {
									infile.open(filename + ".csv");
								} else {
									infile.open(filename);
								}
								if (infile) {
									state = 2;
								} else {
									state = 1;
								}
								infile.close();
							}
							break;
						case 1:
							cout << "Oops! That file does not exist. Please enter a valid file name or press ENTER to go back." << endl;
							cout << ">> ";
							getline(cin,filename);
							if (filename.compare("") == 0) {
								state = -1;
							} else {
								ifstream infile;
								infile.open(filename);
								if (infile) {
									state = 2;
								}
								infile.close();
							}
							break;
						case 2:
							try {
								t.load(filename);
								cout << "File successfully loaded!" << endl;
								if (equation != nullptr) {
									delete equation;
									equation = nullptr;
									focus_var = "";
								}
								fname = filename;
								state = -1;
								table_loaded = true;
							} catch (pgm_error* pg) {
								cout << "Error found loading this truth table." << endl;
								switch (pg->type) {
									case NOT_FOUND:
										cout << pg->str_args[0] << " column not found." << endl;
										break;
									case TOO_MANY_INPUTS:
										if (pg->size_args[0]) {
											cout << "Too many inputs given in second row for this truth table." << endl;
										} else {
											cout << "Too many inputs given in first row for this truth table." << endl;
										}
										break;
									case TOO_MANY_OUTPUTS:
										if (pg->size_args[0]) {
											cout << "Too many outputs given in second row for this truth table." << endl;
										} else {
											cout << "Too many outputs given in first row for this truth table." << endl;
										}
										break;
									case NOT_ENOUGH_OUTPUTS:
										if (pg->size_args[0]) {
											cout << "Not enough outputs given in second row for this truth table." << endl;
										} else {
											cout << "Not enough outputs given in first row for this truth table." << endl;
										}
										break;
									case NOT_ENOUGH_INPUTS:
										if (pg->size_args[0]) {
											cout << "Not enough inputs given in second row for this truth table." << endl;
										} else {
											cout << "Not enough inputs given in first row for this truth table." << endl;
										}
										break;
									case TOO_MANY_ROWS:
										cout << "Too many data rows defined in this truth table." << endl;
										break;
									case NOT_ENOUGH_ROWS:
										cout << "Not enough data rows defined in this truth table." << endl;
										break;
									case MISSING_ARG:
										if (pg->num_str_args) {
											cout << "Not all cases are accounted for by this truth table." << endl;
											cout << "Missing case: " << pg->str_args[0] << endl;
										} else {
											cout << "The number of arguments defined by the first and second row do not match." << endl;
										}
										break;
									case TOO_MANY_ARGS:
										cout << "Too many columns defined in line #" << pg->size_args[0] << "." << endl;
										break;
									case UNRECOGNIZED_CHAR:
										cout << "Unrecognized value found here:" << endl;
										cout << pg->str_args[0] << endl;
										for (size_t i = 0; i < pg->size_args[0]; i++) {
											cout << " ";
										}
										cout << "^" << endl;
										cout << "Valid values include '" << string_mapping[HI_MAP] << "', '" << string_mapping[LO_MAP] << "', '" << string_mapping[DC_MAP] << "'" << endl;
										break;
									case DUPLICATE_ARG:
										cout << "Input row #" << pg->size_args[0] << " and input row #" << pg->size_args[1] << " are the same but their outputs are different." << endl;
										cout << "Inputs:" << endl;
										cout << "\t" << pg->str_args[0] << endl;
										cout << "\t" << pg->str_args[1] << endl;
										cout << "Outputs:" << endl;
										cout << "\t" << pg->str_args[2] << endl;
										cout << "\t" << pg->str_args[3] << endl;
										break;
								}
								delete pg;
								state = 0;
							}
							break;
					}
				}
			} else if (line.compare("print") == 0) { // Print truth table
				if (table_loaded) {
					cout << t << endl;
				} else {
					cout << "Oops! It looks like you haven't loaded a file yet. Please load a file that you can print." << endl;
				}
			} else if (line.compare("sop") == 0 || line.compare("pos") == 0) { // Get Sum of Products or Product of Sums for variable
				bool get_sop = (line.compare("sop") == 0);
				if (table_loaded) {
					int state = 0;
					while (state >= 0) {
						switch (state) {
							case 0:
								cout << "Please enter the name of the variable you would like to solve for or press ENTER to go back." << endl;
								cout << ">> ";
								getline(cin,line);
								line = trim(line);
								if (line.compare("") == 0) {
									state = -1;
								} else {
									if (map(line,t.outputs,t.numOutputs) == -1) {
										state = 1;
									} else {
										state = -2;
									}
								}
								break;
							case 1:
								cout << "Oops! That variable could not be found in the loaded trth table. Please enter a different variable or press ENTER to go back." << endl;
								cout << ">> ";
								getline(cin,line);
								line = trim(line);
								if (line.compare("") == 0) {
									state = -1;
								} else {
									if (map(line,t.outputs,t.numOutputs) != -1) {
										state = -2;
									}
								}
								break;
						}
					}
					if (state == -2) {
						if (equation != nullptr) {
							delete equation;
						}
						focus_var = line;
						if (get_sop) {
							equation = t.get_sum_of_products(line);
						} else {
							equation = t.get_product_of_sums(line);
						}
						cout << "Here is the equation for the variable \"" << line << "\"." << endl;
						cout << line << " = " << equation->to_string() << endl;
					}
				} else {
					cout << "Oops! It looks like you haven't loaded a file yet. Please load a file first." << endl;
				}
			} else if (line.compare("help") == 0) { // Display list of commands
				cout << "Here is a list of commands:" << endl << endl;
				for (int i = 0; i < NUM_CMDS - 1; i++) {
					cout << help_strings[i] << endl;
				}
			} else {
				cout << "Invalid command. Please try again." << endl;
			}
		}
	}
	if (equation != nullptr) {
		delete equation;
	}
}
bool equal(string str1, string str2) {
	bool result = false;
	int len1 = str1.length();
	int len2 = str2.length();
	if (len1 != len2) {
		result = false;
	} else {
		result = true;
		for (int i = 0; i < len1 && result; i++) {
			if (str1[i] >= 'a' && str1[i] <= 'z') {
				result = (str1[i] == str2[i] || str1[i] == (str2[i] + 32));
			} else if (str1[i] >= 'A' && str1[i] <= 'Z') {
				result = (str1[i] == str2[i] || str1[i] == (str2[i] - 32));
			} else {
				result = str1[i] == str2[i];
			}
		}
	}
	return result;
}
bool is_numeric(string str) {
	int len = str.length();
	switch (len) {
		case 0:
			return false;
			break;
		case 1:
			return (str[0] <= '9' && str[0] >= '0');
			break;
		default:
			if (str[0] == '0') {
				return false;
			} else {
				int start = (str[0] == '-')?1:0;
				for (int i = start; i < len; i++) {
					if (str[i] > '9' || str[i] < '0') {
						return false;
					}
				}
			}
			break;
	}
	return true;
}
bool is_whitespace(string str) {
	int len = str.length();
	for (int i = 0; i < len; i++) {
		if (str[i] != 32 && str[i] != 9 && str[i] != '\n') {
			return false;
		}
	}
	return true;
}
bool contains_chars(string str, char chars[], int len) {
	int str_len = str.length();
	for (int i = 0; i < str_len; i++) {
		for (int j = 0; j < len; j++) {
			if (str[i] == chars[j]) {
				return true;
			}
		}
	}
	return false;
}
int map(string str, string strs[], int len) {
	int num_valid = len;
	int result = -1;
	int str_len = str.length();
	int next[len];
	int prev[len];
	int start = 0;

	for (int i = 0; i < len; i++) {
		prev[i] = i-1;
		next[i] = i+1;
	}
	next[len - 1] = 0;
	prev[0] = len - 1;

	int index = start;
	while (num_valid) {
		if (strs[index].length() != str_len) {
			next[prev[index]] = next[index];
			prev[next[index]] = prev[index];
			num_valid--;
			if (index == start) {
				if (next[index] == start) {
					num_valid = 0;
				} else {
					start = next[index];
					index = next[index];
				}
			} else {
				index = next[index];
				if (index == start) {
					break;
				}
			}
		} else {
			index = next[index];
			if (index == start) {
				break;
			}
		}
	}

	if (num_valid) {
		bool right_val = true;
		for (int i = 0; i < str_len; i++) {
			index = start;
			while (num_valid) {
				if (str[i] != strs[index][i]) {
					next[prev[index]] = next[index];
					prev[next[index]] = prev[index];
					num_valid--;
					if (index == start) {
						if (next[index] == start) {
							num_valid = 0;
						} else {
							start = next[index];
							index = next[index];
						}
					} else {
						index = next[index];
						if (index == start) {
							break;
						}
					}
				} else {
					index = next[index];
					if (index == start) {
						break;
					}
				}
			}
		}
	}

	if (num_valid) {
		result = start;
	}

	return result;
}
void explode(string str, string delim, string*& strs, size_t& len, size_t max_size) {
	len = 1;
	size_t start_index = 0;
	bool keep_going = true;
	while (keep_going) {
		start_index = str.find(delim,start_index);
		if (start_index == string::npos) {
			keep_going = false;
		} else {
			start_index++;
			if (start_index > str.length()) {
				keep_going = false;
			} else {
				if (len == max_size) {
					strs = nullptr;
					len = -3;
					return;
				} else {
					len++;	
				}
			}
		}
	}

	strs = new string[len];

	size_t str_index = 0;
	start_index = 0;
	size_t end_index = 0;
	keep_going = true;
	while (keep_going) {
		start_index = end_index;
		end_index = str.find(delim,start_index);
		if (end_index == string::npos) {
			strs[str_index] = str.substr(start_index);
			str_index++;
			keep_going = false;
		} else {
			end_index++;
			strs[str_index] = str.substr(start_index,(end_index - start_index - 1));
			str_index++;
			if (end_index > str.length()) {
				keep_going = false;
			}
		}
	}
}
void printTableInfo(string filename, string* input_vars, string* output_vars, size_t number_inputs, size_t number_outputs) {
	size_t str_len = filename.length();
	string input_string = "";
	string output_string = "";
	if (number_inputs != -1) {
		for (size_t i = 0; i < number_inputs; i++) {
			if (i != 0) {
				input_string += ',';
			}
			input_string = input_string + input_vars[i];
		}
		size_t g = input_string.length();
		if (str_len < g) {
			str_len = g;
		}
	}
	if (number_outputs != -1) {
		for (size_t i = 0; i < number_outputs; i++) {
			if (i != 0) {
				output_string += ',';
			}
			output_string = output_string + output_vars[i];
		}
		size_t g = output_string.length();
		if (str_len < g) {
			str_len = g;
		}
	}
	size_t buff_len = 0;

	cout << lineBreak << endl;
	cout << "Filename         :";
	if (filename.compare("") != 0) {
		buff_len = str_len - filename.length();
		for (size_t i = 0; i < buff_len; i++) {
			cout << " ";
		}
		cout << filename;
	}
	cout << endl;
	cout << "Input variables  :" << endl;
	if (number_inputs > 0) {
		buff_len = str_len - input_string.length();
		for (size_t i = 0; i < buff_len; i++) {
			cout << " ";
		}
		cout << input_string;
	}
	cout << endl;
	cout << "Output variables :" << endl;
	if (number_outputs > 0) {
		buff_len = str_len - output_string.length();
		for (size_t i = 0; i < buff_len; i++) {
			cout << " ";
		}
		cout << output_string;
	}
	cout << endl;
	cout << lineBreak << endl;
}
