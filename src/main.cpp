#include <iostream>
#include <string>

#define NUM_CMDS 7
#define NUM_INVALID_FNAME_CHARS 9

int iteration = 0;

#include "truth_table.h"
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
string trim(string str);
void doThing();
void explode(string str, string delim, string*& strs, int& len);

int main() {
	doThing();
	cout << "FINISHED" << endl;
}

void doThing() {

	bool keep_going = true;
	bool show_header = true;
	string line = "";
	string filename = "";
	string fname = "";
	string str1 = "";
	string str2 = "";
	string focus_var = "";
	string* input_vars = nullptr;
	string* output_vars = nullptr;
	truth_table* t = nullptr;
	log* equation = nullptr;
	int number_inputs = -1;
	int number_outputs = -1;
	int index = -1;
	int state = -1;
	int str_len = -1;

	cout << "Welcome! Please enter a command below. If you want to see a list of commands, type 'help' and press ENTER." << endl;
	while (keep_going) {
		if (show_header) {
			cout << lineBreak << endl;
			cout << "Current table    : ";
			if (t == nullptr) {
				cout << "Nothing loaded" << endl;
			} else {
				cout << fname << endl;
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
			index = map(line,valid_cmds,NUM_CMDS);

			switch (index) {
				case 0: 	// Generate new file
					state = 0;
					filename = "";
					while (state >= 0) {
						switch (state) {
							case 0:
								cout << lineBreak << endl;
								cout << "Filename        :" << endl;
								cout << "Input variables :   Output variables :" << endl;
								cout << lineBreak << endl;
								filename = "";
								cout << "Please enter the name of the new file you would like to generate or press ENTER to go back." << endl;
								cout << ">> ";
								getline(cin,filename);
								filename = trim(filename);
								if (filename.compare("") == 0) {
									state = -1;
								} else {
									if (contains_chars(filename,invalid_filename_chars,NUM_INVALID_FNAME_CHARS)) {
										state = 1;
									} else {
										state = 2;
									}
								}
								break;
							case 1:
								cout << lineBreak << endl;
								cout << "Filename        :" << endl;
								cout << "Input variables :   Output variables :" << endl;
								cout << lineBreak << endl;
								filename = "";
								cout << "The file name entered contains invalid characters. Please enter another file name or press ENTER to go back." << endl;
								cout << ">> ";
								getline(cin,filename);
								filename = trim(filename);
								if (filename.compare("") == 0) {
									state = -1;
								} else {
									if (!contains_chars(filename,invalid_filename_chars,NUM_INVALID_FNAME_CHARS)) {
										state = 2;
									}
								}
								break;
							case 2:
								if (filename.substr(filename.length()-4).compare(".csv") != 0) {
									str1 = filename + ".csv";
								}
								str_len = str1.length();
								cout << lineBreak << endl;
								cout << "Filename        : " << str1 << endl;
								cout << "Input variables : ";
								for (int i = 0; i < str_len; i++) {
									cout << " ";
								}
								cout << " Output variables:" << endl;
								cout << lineBreak << endl;
								if (input_vars != nullptr) {
									delete[] input_vars;
									input_vars = nullptr;
								}
								number_inputs = -1;
								cout << "Please enter the name of the input variables you would like to use for this truth table separated by commas or press ENTER to go back." << endl;
								cout << ">> ";
								getline(cin,line);
								line = trim(line);
								if (line.compare("") == 0) {
									state = 0;
								} else {
									explode(line,",",input_vars,number_inputs);
									for (int i = 0; i < number_inputs && state == 2; i++) {
										if (is_whitespace(input_vars[i])) {
											state = 3;
											index = i;
										} else {
											input_vars[i] = trim(input_vars[i]);
										}
									}
									state = 4;
								}
								break;
							case 3:
								if (filename.substr(filename.length()-4).compare(".csv") != 0) {
									str1 = filename + ".csv";
								}
								str_len = str1.length();
								cout << lineBreak << endl;
								cout << "Filename        : " << str1 << endl;
								cout << "Input variables : ";
								for (int i = 0; i < str_len; i++) {
									cout << " ";
								}
								cout << " Output variables:" << endl;
								cout << lineBreak << endl;
								if (input_vars != nullptr) {
									delete[] input_vars;
									input_vars = nullptr;
								}
								number_inputs = -1;
								cout << "No variable entered in slot #" << index << ". Please try again or press ENTER to go back." << endl;
								cout << ">> ";
								getline(cin,line);
								line = trim(line);
								if (line.compare("") == 0) {
									state = 0;
								} else {
									explode(line,",",input_vars,number_inputs);
									bool is_valid = true;
									for (int i = 0; i < number_inputs && is_valid; i++) {
										if (is_whitespace(input_vars[i])) {
											is_valid = false;
											index = i;
										} else {
											input_vars[i] = trim(input_vars[i]);
										}
									}
									if (is_valid) {
										state = 4;
									}
								}
								break;
							case 4:
								if (filename.substr(filename.length()-4).compare(".csv") != 0) {
									str1 = filename + ".csv";
								}
								str_len = str1.length();
								str2 = "";
								for (int i = 0; i < number_inputs; i++) {
									if (i != 0) {
										str2 += ',';
									}
									str2 = str2 + input_vars[i];
								}
								if (str_len < str2.length()) {
									str_len = str2.length();
								}
								cout << lineBreak << endl;
								cout << "Filename        : " << str1;
								for (int i = str1.length(); i < str_len; i++) {
									cout << " ";
								}
								cout << endl;
								cout << "Input variables : " << str2;
								for (int i = str2.length(); i < str_len; i++) {
									cout << " ";
								}
								cout << " Output variables:" << endl;
								cout << lineBreak << endl;
								if (output_vars != nullptr) {
									delete[] output_vars;
									output_vars = nullptr;
								}
								number_outputs = -1;
								cout << "Please enter the name of the output variables you would like to use for this truth table separated by commas or press ENTER to go back." << endl;
								cout << ">> ";
								getline(cin,line);
								line = trim(line);
								if (line.compare("") == 0) {
									state = 2;
								} else {
									explode(line,",",output_vars,number_outputs);
									for (int i = 0; i < number_outputs && state == 2; i++) {
										if (is_whitespace(output_vars[i])) {
											state = 4;
											index = i;
										} else {
											output_vars[i] = trim(output_vars[i]);
										}
									}
									state = -2;
								}
								break;
							case 5:
								if (filename.substr(filename.length()-4).compare(".csv") != 0) {
									str1 = filename + ".csv";
								}
								str_len = str1.length();
								str2 = "";
								for (int i = 0; i < number_inputs; i++) {
									if (i != 0) {
										str2 += ',';
									}
									str2 = str2 + input_vars[i];
								}
								if (str_len < str2.length()) {
									str_len = str2.length();
								}
								cout << lineBreak << endl;
								cout << "Filename        : " << str1;
								for (int i = str1.length(); i < str_len; i++) {
									cout << " ";
								}
								cout << endl;
								cout << "Input variables : " << str2;
								for (int i = str2.length(); i < str_len; i++) {
									cout << " ";
								}
								cout << " Output variables:" << endl;
								cout << lineBreak << endl;
								if (output_vars != nullptr) {
									delete[] output_vars;
									output_vars = nullptr;
								}
								number_outputs = -1;
								cout << "No variable entered in slot #" << index << ". Please try again or press ENTER to go back." << endl;
								cout << ">> ";
								getline(cin,line);
								line = trim(line);
								if (line.compare("") == 0) {
									state = 2;
								} else {
									explode(line,",",output_vars,number_outputs);
									bool is_valid = true;
									for (int i = 0; i < number_outputs && is_valid; i++) {
										if (is_whitespace(output_vars[i])) {
											is_valid = false;
											index = i;
										} else {
											output_vars[i] = trim(output_vars[i]);
										}
									}
									if (is_valid) {
										state = -2;
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
						outfile << "Inputs,";
						for (int i = 1; i < number_inputs; i++) {
							outfile << ",";
						}
						outfile << "Outputs";
						for (int i = 1; i < number_outputs; i++) {
							outfile << ",";
						}
						outfile << endl;

						for (int i = 0; i < number_inputs; i++) {
							if (i != 0) {
								outfile << ",";
							}
							outfile << input_vars[i];
						}
						for (int i = 0; i < number_outputs; i++) {
							outfile << "," << output_vars[i];
						}

						long long finished_num = (1 << number_inputs);
						long long current_num = 0;
						while (current_num != finished_num) {
							outfile << endl;
							for (int i = 0; i < number_inputs; i++) {
								if (i != 0) {
									outfile << ",";
								}
								if (current_num & (1 << (number_inputs - i - 1))) {
									outfile << HI;
								} else {
									outfile << LO;
								}
							}
							for (int i = 0; i < number_outputs; i++) {
								outfile << "," << LO;
							}
							current_num++;
						}

						outfile.close();
						state = -1;

						str_len = filename.length();
						str1 = "";
						for (int i = 0; i < number_inputs; i++) {
							if (i != 0) {
								str1 += ',';
							}
							str1 = str1 + input_vars[i];
						}
						str2 = "";
						for (int i = 0; i < number_outputs; i++) {
							if (i != 0) {
								str2 += ',';
							}
							str2 = str2 + output_vars[i];
						}
						if (str_len < str1.length()) {
							str_len = str1.length();
						}
						if (str_len < str2.length()) {
							str_len = str2.length();
						}
						cout << lineBreak << endl;
						cout << "File generated successfully!" << endl;
						cout << "Filename        : " << filename;
						for (int i = filename.length(); i < str_len; i++) {
							cout << " ";
						}
						cout << endl;
						cout << "Input variables : " << str1;
						for (int i = str1.length(); i < str_len; i++) {
							cout << " ";
						}
						cout << " Output variables: " << str2;
						for (int i = str2.length(); i < str_len; i++) {
							cout << " ";
						}
						cout << endl;
						cout << lineBreak << endl;
					}
					if (input_vars != nullptr) {
						delete[] input_vars;
						input_vars = nullptr;
					}
					if (output_vars != nullptr) {
						delete[] output_vars;
						output_vars = nullptr;
					}
					number_inputs = -1;
					number_outputs = -1;
					break;
				case 1: 	// Quit
					keep_going = false;
					break;
				case 2: 	// Load file
					state = 0;
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
										state = -2;
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
									if (filename.substr(filename.length()-4).compare(".csv") != 0) {
										infile.open(filename + ".csv");
									} else {
										infile.open(filename);
									}
									if (infile) {
										state = -2;
									}
									infile.close();
								}
								break;
						}
					}
					if (state == -2) {
						if (t != nullptr) {
							delete t;
						}
						if (filename.substr(filename.length()-4).compare(".csv") != 0) {
							filename = filename + ".csv";
						}
						t = new truth_table(filename);
						cout << "File successfully loaded!" << endl;
						if (equation != nullptr) {
							delete equation;
							equation = nullptr;
							focus_var = "";
						}
						fname = filename;
					}
					break;
				case 3: 	// Print truth table
					if (t == nullptr) {
						cout << "Oops! It looks like you haven't loaded a file yet. Please load a file that you can print." << endl;
					} else {
						cout << (*t) << endl;
					}
					break;
				case 4: 	// Get Sum of Products for variable
					if (t == nullptr) {
						cout << "Oops! It looks like you haven't loaded a file yet. Please load a file first." << endl;
					} else {
						state = 0;
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
										if (map(line,t->outputs,t->numOutputs) == -1) {
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
										if (map(line,t->outputs,t->numOutputs) != -1) {
											state = -2;
										}
									}
							}
						}
						if (state == -2) {
							if (equation != nullptr) {
								delete equation;
							}
							focus_var = line;
							equation = t->get_sum_of_products(line);
							cout << "Here is the equation for the variable \"" << line << "\"." << endl;
							cout << line << " = " << equation->to_string() << endl;
						}
					}
					break;
				case 5: 	// Get Product of Sums for variable
					if (t == nullptr) {
						cout << "Oops! It looks like you haven't loaded a file yet. Please load a file first." << endl;
					} else {
						state = 0;
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
										if (map(line,t->outputs,t->numOutputs) == -1) {
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
										if (map(line,t->outputs,t->numOutputs) != -1) {
											state = -2;
										}
									}
							}
						}
						if (state == -2) {
							if (equation != nullptr) {
								delete equation;
							}
							focus_var = line;
							equation = t->get_product_of_sums(line);
							cout << "Here is the equation for the variable \"" << line << "\"." << endl;
							cout << line << " = " << equation->to_string() << endl;
						}
					}
					break;
				case 6:		// Display list of commands
					cout << "Here is a list of commands:" << endl << endl;
					for (int i = 0; i < NUM_CMDS - 1; i++) {
						cout << help_strings[i] << endl;
					}
					break;
				default: 	// Invalid command
					cout << "Invalid command. Please try again." << endl;
					break;
			}
		}
	}

	if (t != nullptr) {
		delete t;
	}
	if (equation != nullptr) {
		delete equation;
	}
	if (input_vars != nullptr) {
		delete[] input_vars;
	}
	if (output_vars != nullptr) {
		delete[] output_vars;
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
				right_val = true;
				if (str[i] >= 'a' && str[i] <= 'z') {
					if (str[i] != strs[index][i] && str[i] != (strs[index][i] + 32)) {
						right_val = false;
					}
				} else if (str[i] >= 'A' && str[i] <= 'Z') {
					if (str[i] != strs[index][i] && str[i] != (strs[index][i] - 32)) {
						right_val = false;
					}
				} else if (str[i] != strs[index][i]) {
					right_val = false;
				}
				if (!right_val) {
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
string trim(string str) {
	string result = str;
	if (str.compare("") != 0) {
		for (int i = 0; i < result.length(); i++) {
			if (result[i] != 32 && result[i] != 9) {
				result = result.substr(i);
				break;
			}
		}

		for (int i = result.length(); i > 0; i--) {
			if (result[i-1] != 32 && result[i-1] != 9) {
				result = result.substr(0,i);
				break;
			}
		}
	}
	return result;
}
void explode(string str, string delim, string*& strs, int& len) {
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
				len++;
			}
		}
	}

	strs = new string[len];

	int str_index = 0;
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