#include <iostream>
#include <string>

// Length of help_strings[] and valid_cmds[] arrays
#define NUM_CMDS 8

// Length of invalid_filename_chars[] array
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

// Characters that cannot be included in filename that the user provides
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

// Lines that will be displayed when the user issues the "help" command
string help_strings[] = {
	"	generate 	- Generates a truth table CSV file after being provided the filename the input variables and the output variables.",
	"	load 		- Loads a truth table CSV file into memory. This command must be run before performimg 'print', 'sop', or 'pos'.",
	"	print 		- Prints the current truth table loaded into memory.",
	"	sop 		- Gets the 'Sum of Products' boolean equation after being given a variable name and loads this equation into memory.",
	"	pos 		- Gets the 'Product of Sums' boolean equation after being given a variable name and loads this equation into memory.",
	"	quit 		- Ends the program.",
	"	simplify	- Simplifies the current logical expression by applying DeMorgan's laws."
};

// Contains the list of valid commands
string valid_cmds[] = {
	"generate",
	"quit",
	"load",
	"print",
	"sop",
	"pos",
	"help",
	"simplify"
};
const string lineBreak = "-----------------------------------------------------------------------------------------";

// Forward declaration of functions
bool is_whitespace(string str);
bool contains_chars(string str, char chars[], int len);
void doThing();
void explode(string str, string delim, string*& strs, size_t& len, size_t max_size);
void printTableInfo(string filename, string* input_vars, string* output_vars, size_t number_inputs, size_t number_outputs);

int main() {
	doThing();
	cout << "Program terminated" << endl;
}

// This is the main function that the user will interact with
void doThing() {

	// While this is true, the program will continue to ask for user input,
	// Once this is false, the program will terminate
	bool keep_going = true;

	// At each iteration where this is true, it will show the header of the program
	// each time it asks for input that looks like this:
	// 
	// --------------------------------------------------------
	// Current Table:		Table.csv
	// Current Equation:	output = (input1 & input2) | ~input3
	// --------------------------------------------------------
	bool show_header = true;

	// This indicates whether a truth_table is currently loaded into the memory.
	// This is important because, when there is not, certain commands are disabled
	// like "print"
	bool table_loaded = false;

	// Contains the user input
	string line = "";

	// Contains the name of the currently loaded file
	string fname = "";

	// Contains the name of the variable whose equation is currently loaded in memory
	string focus_var = "";

	// Contains the truth_table currently being studied
	truth_table t;

	// Contains the boolean expression that is currently being studied
	log* equation = nullptr;

	// Defines the maximum number of inputs for a truth_table
	size_t g = SIZE_MAX;
	size_t max_inputs = 0;
	while (g) {
		max_inputs++;
		g >>= 1;
	}

	cout << "Welcome! Please enter a command below. If you want to see a list of commands, type 'help' and press ENTER." << endl;
	while (keep_going) {
		if (show_header) {

			// Print the header block
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

		// If the user presses enter, the program should just move to a new line
		if (line.compare("") != 0) {
			show_header = true;

			// This command generates a new CSV truth table file for the user
			// based on parameters they provide
			if (line.compare("generate") == 0) {
				// Stores the current filename that hasn't been stored in <fname> yet
				string filename = "";

				// Store the input and output variables
				size_t number_inputs = 0;
				size_t number_outputs = 0;
				string* input_vars = nullptr;
				string* output_vars = nullptr;

				// Governs what state in the "generate file" process the user is at
				int state = 0;
				while (state >= 0) {

					// Prints out the current information of the file to be generated
					printTableInfo(filename,input_vars,output_vars,number_inputs,number_outputs);
					switch (state) {
						case 0: // Set file name
							cout << "Please enter the filename below that you would like to write the truth table into or press ENTER to go back." << endl;
							cout << ">> ";
							getline(cin,filename);
							filename = trim(filename);
							if (filename.compare("") == 0) {

								// Go back to the main menu
								state = -1;
							} else {
								if (contains_chars(filename,invalid_filename_chars,NUM_INVALID_FNAME_CHARS)) {

									// Prompt the user again if this filename contains invalid characters
									cout << "This filename is invalid." << endl;
									filename = "";
								} else {

									// If this filename is valid, move to the next state
									state = 1;
								}
							}
							break;
						case 1: // Set input variables
							cout << "Please enter the name(s) of the input variables you would like to use for this truth table separated by commas or press ENTER to go back." << endl;
							cout << "For example, if the input variables I wanted to use were called \"apple\", \"pear\", and \"banana\" I would write this:" << endl;
							cout << endl << "\tapple,pear,banana" << endl << endl;
							cout << ">> ";
							getline(cin,line);
							line = trim(line);
							if (line.compare("") == 0) {

								// Go back to the "get file name" step
								filename = "";
								state = 0;
							} else {

								// Store the input variables from <line> to <input_vars> and <number_inputs>
								explode(line,",",input_vars,number_inputs,max_inputs);
								bool is_valid = true;
								if (number_inputs != -3) {
									for (size_t i = 0; i < number_inputs && is_valid; i++) {

										// Throw error if one of the variables is blank
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

									// number_inputs == -3 indicates too many inputs
									cout << "Too many input variables entered. The maximum allowed amount is 64." << endl;
								}
							}
							break;
						case 2: // Set output variables
							cout << "Please enter the name(s) of the output variables you would like to use for this truth table separated by commas or press ENTER to go back." << endl;
							cout << "For example, if the output variables I wanted to use were called \"apple\", \"pear\", and \"banana\" I would write this:" << endl;
							cout << endl << "\tapple,pear,banana" << endl << endl;
							cout << ">> ";
							cout << ">> ";
							getline(cin,line);
							line = trim(line);
							if (line.compare("") == 0) {

								// Go back to the "get input variables" step
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
					ofstream outfile;
					outfile.open(filename);

					// Write the first line of the file. This line defines which columns 
					// are inputs and which columns are outputs
					outfile << string_mapping[INPUT_MAP] << ",";
					for (size_t i = 1; i < number_inputs; i++) {
						outfile << string_mapping[DELIM_MAP];
					}
					outfile << string_mapping[OUTPUT_MAP];
					for (size_t i = 1; i < number_outputs; i++) {
						outfile << string_mapping[DELIM_MAP];
					}
					outfile << endl;

					// Write the second line of the file. This line defines th names
					// of the input and output variables
					for (size_t i = 0; i < number_inputs; i++) {
						if (i != 0) {
							outfile << string_mapping[DELIM_MAP];
						}
						outfile << input_vars[i];
					}
					for (size_t i = 0; i < number_outputs; i++) {
						outfile << string_mapping[DELIM_MAP] << output_vars[i];
					}

					// Write the remaining lines of the file based on how many input
					// variables there are. The total number of data lines written is
					// equal to 1 << number_inputs or 2^(number_inputs)
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

				// Delete the input and output variables arrays as they are now written into the file
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

				// Keeps track of what step in the "load file" process the user is at
				int state = 0;

				// Stores the name of the file the user would like to load
				string filename = "";
				while (state >= 0) {
					switch (state) {
						case 0: // File name prompt
							filename = "";
							cout << "Please enter the name of the file you would like to load or press ENTER to go back." << endl;
							cout << ">> ";
							getline(cin,filename);
							filename = trim(filename);
							if (filename.compare("") == 0) {

								// Return to the main menu
								state = -1;
							} else {
								ifstream infile;
								if (filename.substr(filename.length()-4).compare(".csv") != 0) {
									infile.open(filename + ".csv");
								} else {
									infile.open(filename);
								}
								if (infile) {

									// Error finding this file
									state = 2;
								} else {

									// File found successfully
									state = 1;
								}
								infile.close();
							}
							break;
						case 1: // Invalid file name provided
							cout << "Oops! That file does not exist. Please enter a valid file name or press ENTER to go back." << endl;
							cout << ">> ";
							getline(cin,filename);
							if (filename.compare("") == 0) {

								// Return to main menu
								state = -1;
							} else {
								ifstream infile;
								infile.open(filename);
								if (infile) {

									// File found successfully
									state = 2;
								}
								infile.close();
							}
							break;
						case 2: // Attempt to load the file
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

								// Below is all the errors that could be encountered when reading the truth
								// table file. See truth_table.cpp for more information on the correct
								// format for truth table files
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
							case 0: // Prompt user for variable name
								cout << "Please enter the name of the variable you would like to solve for or press ENTER to go back." << endl;
								cout << ">> ";
								getline(cin,line);
								line = trim(line);
								if (line.compare("") == 0) {
									state = -1;
								} else {
									if (t.indexOfOutput(line) == -1) {
										state = 1;
									} else {
										state = -2;
									}
								}
								break;
							case 1: // Variable name provided was invalid
								cout << "Oops! That variable could not be found in the loaded trth table. Please enter a different variable or press ENTER to go back." << endl;
								cout << ">> ";
								getline(cin,line);
								line = trim(line);
								if (line.compare("") == 0) {
									state = -1;
								} else {
									if (t.indexOfOutput(line) == -1) {
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
			} else if (line.compare("simplify") == 0) {
				if (table_loaded) {
					if (equation == nullptr) {
						cout << "Oops! It looks like you haven't loaded boolean expression yet. Please load an equation that you can simplify." << endl;
					} else {
						equation->demorgify();
						cout << "Your new equation: " << endl;
						cout << focus_var << " = " << equation->to_string() << endl;
					}
				} else {
					cout << "Oops! It looks like you haven't loaded a file yet. Please load a file that you can print." << endl;
				}
			} else {
				cout << "Invalid command. Please try again." << endl;
			}
		}
	}

	// Run garbage collection
	if (equation != nullptr) {
		delete equation;
	}
}

/*
 * Returns true if all the characters in <str> are whitespace characters.
 * Returns false otherwise.
 */
bool is_whitespace(string str) {
	int len = str.length();
	for (int i = 0; i < len; i++) {
		if (str[i] != 32 && str[i] != 9 && str[i] != '\n') {
			return false;
		}
	}
	return true;
}
/*
 * Returns true if <str> contains and character in the <chars>[len] array
 * Returns false otherwise.
 */
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
/*
 * Breaks up <str> using the delimiter <delim> and stores the elements in the
 * array <strs[len]>. If, before it finishes counting the elements, the total
 * number of elements found becomes equal to max_size, it sets len = -3
 */
void explode(string str, string delim, string*& strs, size_t& len, size_t max_size) {
	len = 1;
	size_t start_index = 0;
	bool keep_going = true;

	// Count the total number of elements in the delimited string
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

	// Allocate space for the new string array
	strs = new string[len];

	size_t str_index = 0;
	start_index = 0;
	size_t end_index = 0;
	keep_going = true;

	// Store the elements of the delimited string into <strs>
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

/* 
 * Prints a formatted block that contains the information for a truth table file 
 * that is in the process of being generated
 */
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
	cout << "Input variables  :";
	if (number_inputs > 0) {
		cout << endl;
		buff_len = str_len - input_string.length();
		for (size_t i = 0; i < buff_len; i++) {
			cout << " ";
		}
		cout << input_string;
	}
	cout << endl;
	cout << "Output variables :";
	if (number_outputs > 0) {
		cout << endl;
		buff_len = str_len - output_string.length();
		for (size_t i = 0; i < buff_len; i++) {
			cout << " ";
		}
		cout << output_string;
	}
	cout << endl;
	cout << lineBreak << endl;
}
