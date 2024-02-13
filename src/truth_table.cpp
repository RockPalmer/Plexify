/** Defaut constructor */
truth_table::truth_table() {
	inputs = nullptr;
	outputs = nullptr;
	inputTable = nullptr;
	outputTable = nullptr;
	numInputs = 0;
	numOutputs = 0;
	tableHeight = 0;
}
/** Constructor for passing in values by hand rather than parsing a file
 * If no table height is defined, the tableHeight is set to 2^(number of inputs defined)
 */
truth_table::truth_table(std::string* inp, std::string* outp, char** inpTab, char** outpTab, size_t numInp, size_t numOutp, size_t tabHeight = -1) {
	inputs = inp;
	outputs = outp;
	inputTable = inpTab;
	outputTable = outpTab;
	numInputs = numInp;
	numOutputs = numOutp;
	tableHeight = (tabHeight == -1)?(1 << numInp):(tabHeight);
}
/** Construct for parsing from a file */
truth_table::truth_table(std::string filename) {
	load(filename);
}
truth_table::~truth_table() {
	if (inputs != nullptr) {
		delete[] inputs;
	}
	if (outputs != nullptr) {
		delete[] outputs;
	}
	if (inputTable != nullptr) {
		for (size_t i = 0; i < tableHeight; i++) {
			if (inputTable[i] != nullptr) {
				delete[] inputTable[i];
			}
		}
		delete[] inputTable;
	}
	if (outputTable != nullptr) {
		for (size_t i = 0; i < tableHeight; i++) {
			if (outputTable[i] != nullptr) {
				delete[] outputTable[i];
			}
		}
		delete[] outputTable;
	}
}
/** This function gets all rows of the <inputTable array where the output variable 
 * defined by <var> is at the logic level <val>, loads them into a linked_list<char*> and 
 * returns a pointer to that linked list. 
 * 
 * Therefore, each entry in the linked list will be a char array that will be a copy
 * of one of the rows in <inputTable> */
linked_list<char*>* truth_table::get_cases_where(std::string var, char val) {
	linked_list<char*>* l = new linked_list<char*>();

	// Get the index of the variable <var> in the outputs[] array.
	size_t index = indexOfOutput(var);

	// Iterate through the rows of the truth table
	for (size_t i = 0; i < tableHeight; i++) {
		if (outputTable[i][index] == val) {

			// If the output value of <var> at this row matches the desired logic value,
			// copy that row into a new char array and attach it to the end of the linked list.
			l->append(new char[numInputs]);
			for (size_t j = 0; j < numInputs; j++) {
				l->tail->data[j] = inputTable[i][j];
			}
		}
	}
	return l;
}
/* This function get the minimized Sum of Products expression for the output variable <var> 
 * and returns it in the form of a pointer the the log struct containing the expression
 */
log* truth_table::get_sum_of_products(std::string var) {

	// Grab all rows of the input table where <var> = HI or <var> = DC
	linked_list<char*>* hi_cases = get_cases_where(var,HI);
	linked_list<char*>* dc_cases = get_cases_where(var,DC);

	// If any of the rows of the DC list contain DC bits, expand those rows into the
	// rows that make them up.
	expand(dc_cases);

	// Remove all redundant rows between these 2 lists
	find_redundant(hi_cases,dc_cases);

	// Simplify the list of cases
	reduce(hi_cases,dc_cases);

	// Convert the resulting cases into a log struct
	log* l = convert_to_log(hi_cases);

	// Destory the linked lists
	hi_cases->destruct_arr();
	dc_cases->destruct_arr();
	delete hi_cases;
	delete dc_cases;

	// Return the resulting expression
	return l;
}
/* This function get the minimized Product of Sums expression for the output variable <var> 
 * and returns it in the form of a pointer the the log struct containing the expression
 */
log* truth_table::get_product_of_sums(std::string var) {

	// Grab all rows of the input table where <var> = LO or <var> = DC
	linked_list<char*>* lo_cases = get_cases_where(var,LO);
	linked_list<char*>* dc_cases = get_cases_where(var,DC);

	// If any of the rows of the DC list contain DC bits, expand those rows into the
	// rows that make them up.
	expand(dc_cases);

	// Remove all redundant rows between these 2 lists
	find_redundant(lo_cases,dc_cases);

	// Simplify the list of cases
	reduce(lo_cases,dc_cases);

	// Convert the resulting cases into a log struct
	log* l = convert_to_log(lo_cases,false);

	// Destory the linked lists
	lo_cases->destruct_arr();
	dc_cases->destruct_arr();
	delete lo_cases;
	delete dc_cases;

	// Return the resulting expression
	return l;
}
/* Finds the index of the string <var> in the outputs[] array. If it is not there, return -1 */
size_t truth_table::indexOfOutput(std::string var) {
	for (size_t i = 0; i < numOutputs; i++) {
		if (var.compare(outputs[i]) == 0) {
			return i;
		}
	}
	return -1;
}

/* Operator overload stream insertion (prints out the truth table) */
std::ostream& operator<<(std::ostream& out, truth_table& t) {

	// Get proper formatting width
	size_t len = find_longest_length_str(t.inputs,t.numInputs,0);
	len = find_longest_length_str(t.outputs,t.numOutputs,len) + 1;

	// Print the input variables
	for (size_t i = 0; i < t.numInputs; i++) {
		out << get_block(t.inputs[i],len);
	}
	out << "|";

	// Print the output variables
	for (size_t i = 0; i < t.numOutputs; i++) {
		out << get_block(t.outputs[i],len);
	}
	out << std::endl;

	size_t tab_width = t.numInputs * len;
	for (size_t i = 0; i < tab_width; i++) {
		out << '-';
	}
	tab_width = t.numOutputs * len;
	out << '|';
	for (size_t i = 0; i < tab_width; i++) {
		out << '-';
	}
	out << std::endl;
	
	for (size_t i = 0; i < t.tableHeight; i++) {
		for (size_t j = 0; j < t.numInputs; j++) {
			out << get_block(t.inputTable[i][j],len);
		}
		out << '|';
		for (size_t j = 0; j < t.numOutputs; j++) {
			out << get_block(t.outputTable[i][j],len);
		}
		out << std::endl;
	}

	return out;
}
/* Converts the linked_list <l> to a logical expression. The boolean <is_sop> defines whether 
 * or not the resulting expression is meant to be a Sum of Products or Product of Sums.
 * 		is_sop = true  -> Sum or Products expression
 * 		is_sop = false -> Product of Sums expression
 */
log* truth_table::convert_to_log(linked_list<char*>* l, bool is_sop) {
	log* result;
	log* temp1 = nullptr;
	log* temp2 = nullptr;
	log* temp3 = nullptr;

	switch (l->length) {
		case 0: // list is empty -> result = LO
			result = new log();
			result->val = is_sop?LO:HI;
			result->type = LITERAL;
			break;
		case 1: // list has only one element -> result = HI || result = "var" || result = NOT("var") || result = AND(...)
			switch (count_valid(l->get(0),numInputs)) {
				case 0: // No inputs specifiied -> result = HI
					result = new log();
					result->val = is_sop?HI:LO;
					result->type = LITERAL;
					break;
				case 1: // Only one input is specified -> result = "var" || temp1 = "var", result = NOT(temp1)
					for (size_t i = 0; i < numInputs; i++) {
						if (l->get(0)[i] != DC) { // Input at i is set -> add this input
							result = new log();
							if (is_sop) {
								if (l->get(0)[i] == HI) { // Input at i is HI -> result = "var"
									result->var_name = inputs[i];
									result->type = VARIABLE;
								} else { // input at i is LO -> temp1 = "var", result = NOT(temp1)
									result->gate = NOT;
									temp1 = new log();
									temp1->var_name = inputs[i];
									temp1->type = VARIABLE;
									result->args.append(temp1);
									result->type = EXPRESSION;
									delete temp1;
								}
							} else {
								if (l->get(0)[i] == HI) { // Input at i is HI -> result = "var"
									result->gate = NOT;
									temp1 = new log();
									temp1->var_name = inputs[i];
									temp1->type = VARIABLE;
									result->args.append(temp1);
									result->type = EXPRESSION;
									delete temp1;
								} else { // input at i is LO -> temp1 = "var", result = NOT(temp1)
									result->var_name = inputs[i];
									result->type = VARIABLE;
								}
							}
							break;
						}
					}
					break;
				default: // Many inputs are specified -> result = AND(...)
					result = new log();
					result->type = EXPRESSION;
					result->gate = is_sop?AND:OR;
					for (size_t i = 0; i < numInputs; i++) {
						if (l->get(0)[i] != DC) { // Input i is set -> add this input
							if (is_sop) {
								if (l->get(0)[i] == HI) { // Input i is set HI -> temp1 = "var", result &= temp1
									temp1 = new log();
									temp1->type = VARIABLE;
									temp1->var_name = inputs[i];
									result->args.append(temp1);
									delete temp1;
								} else { // Input is set LO -> temp2 = "var", temp1 = NOT(temp2), result &= temp1
									temp1 = new log();
									temp1->gate = NOT;
									temp1->type = EXPRESSION;
									temp2 = new log();
									temp2->type = VARIABLE;
									temp2->var_name = inputs[i];
									temp1->args.append(temp2);
									result->args.append(temp1);
									delete temp1;
									delete temp2;
								}
							} else {
								if (l->get(0)[i] == HI) { // Input i is set HI -> temp1 = "var", result &= temp1
									temp1 = new log();
									temp1->gate = NOT;
									temp1->type = EXPRESSION;
									temp2 = new log();
									temp2->type = VARIABLE;
									temp2->var_name = inputs[i];
									temp1->args.append(temp2);
									result->args.append(temp1);
									delete temp1;
									delete temp2;
								} else { // Input is set LO -> temp2 = "var", temp1 = NOT(temp2), result &= temp1
									temp1 = new log();
									temp1->type = VARIABLE;
									temp1->var_name = inputs[i];
									result->args.append(temp1);
									delete temp1;
								}
							}
						}
					}
					break;
			}
			break;
		default: // There are many cases when output is on -> result = OR(...)
			result = new log();
			result->gate = is_sop?OR:NAND;
			result->type = EXPRESSION;
			for (size_t i = 0; i < l->length; i++) {
				switch (count_valid(l->get(i),numInputs)) {
					case 0: // current case has no inputs set -> temp1 = HI, result |= temp1
						temp1 = new log();
						temp1->val = is_sop?HI:LO;
						temp1->type = LITERAL;
						result->args.append(temp1);
						delete temp1;
						break;
					case 1: // current case has only one input set -> temp1 = "var", result |= temp1 || temp2 = "var", temp1 = NOT(temp2), result |= temp1
						for (size_t j = 0; j < numInputs; j++) {
							if (l->get(i)[j] != DC) { // Input j is set -> add this input
								if (is_sop) {
									if (l->get(i)[j] == HI) { // Input j is set HI -> temp1 = "var", result &= temp1
										temp1 = new log();
										temp1->gate = NONE;
										temp1->type = VARIABLE;
										temp1->var_name = inputs[j];
										result->args.append(temp1);
										delete temp1;
									} else { // Input j is set LO -> temp2 = "var", temp1 = NOT(temp1), result &= temp1
										temp1 = new log();
										temp1->gate = NOT;
										temp1->type = EXPRESSION;
										temp2 = new log();
										temp2->type = VARIABLE;
										temp2->var_name = inputs[j];
										temp1->args.append(temp2);
										result->args.append(temp1);
										delete temp1;
										delete temp2;
									}
								} else {
									if (l->get(i)[j] == HI) { // Input j is set HI -> temp1 = "var", result &= temp1
										temp1 = new log();
										temp1->gate = NOT;
										temp1->type = EXPRESSION;
										temp2 = new log();
										temp2->type = VARIABLE;
										temp2->var_name = inputs[j];
										temp1->args.append(temp2);
										result->args.append(temp1);
										delete temp1;
										delete temp2;
									} else { // Input j is set LO -> temp2 = "var", temp1 = NOT(temp1), result &= temp1
										temp1 = new log();
										temp1->type = VARIABLE;
										temp1->var_name = inputs[j];
										result->args.append(temp1);
										delete temp1;
									}
								}
								break;
							}
						}
						break;
					default: // current case has many inputs set -> temp1 = AND(...), result |= temp1
						temp1 = new log();
						temp1->type = EXPRESSION;
						temp1->gate = is_sop?AND:OR;
						for (size_t j = 0; j < numInputs; j++) {
							if (l->get(i)[j] != DC) { // Input j is set -> add this input
								if (is_sop) {
									if (l->get(i)[j] == HI) { // Input j is set HI -> temp2 = "var", temp1 &= temp2
										temp2 = new log();
										temp2->type = VARIABLE;
										temp2->var_name = inputs[j];
										temp1->args.append(temp2);
										delete temp2;
									} else { // Input j is set LO -> temp3 = "var", temp2 = NOT(temp3), temp1 &= temp2
										temp2 = new log();
										temp2->gate = NOT;
										temp2->type = EXPRESSION;
										temp3 = new log();
										temp3->type = VARIABLE;
										temp3->var_name = inputs[j];
										temp2->args.append(temp3);
										temp1->args.append(temp2);
										delete temp2;
										delete temp3;
									}
								} else {
									if (l->get(i)[j] == HI) { // Input j is set HI -> temp2 = "var", temp1 &= temp2
										temp2 = new log();
										temp2->gate = NOT;
										temp2->type = EXPRESSION;
										temp3 = new log();
										temp3->type = VARIABLE;
										temp3->var_name = inputs[j];
										temp2->args.append(temp3);
										temp1->args.append(temp2);
										delete temp2;
										delete temp3;
									} else { // Input j is set LO -> temp3 = "var", temp2 = NOT(temp3), temp1 &= temp2
										temp2 = new log();
										temp2->type = VARIABLE;
										temp2->var_name = inputs[j];
										temp1->args.append(temp2);
										delete temp2;
									}
								}
							}
						}
						result->args.append(temp1);
						delete temp1;
						break;
				}
			}
			break;
	}

	return result;
}
/** This function parses the file designated by <filename> into a truth_table object. If this
 * function encounters any errors while parsing the file, it throws a "new pgm_error();" 
 * instance.
 * 
 * As this is a file parser, strict rules must be followed on the formatting of the file the
 * user whishes to load. All the character values that the parser looks for can be found
 * and changed at the programmer's discretion in the "string_mapping.h" file. Beyond this,
 * below are the rest of the formatting requirements that must be met for a successful
 * read of the input file.
 * 
 * This function read the input file as a rows of delimited values where the delimiting
 * string is contained in string_mapping[DELIM_MAP]. The function expects the data
 * to be orgainized into rows delimited by line breaks and columns delimited by the previously
 * mentioned delimition string. So, whenever mention is made of a "column", it can be assumed 
 * that this is referring to the strings contained between the previously mentioned 
 * delimiters
 * 
 * Line 1: 
 * 
 * The first column must contain the value found at string_mapping[INPUT_MAP]. This designates
 * the column index of the first input column of the truth table. However, as the parser expects
 * this as the first value, this will always be 0. All subsequent empty columns indicate the 
 * number of input variables, in addition to this first one, there will be. The maximum number
 * of input variables possible is defined by the compiler as log2(SIZE_MAX). The very next 
 * column after the empty columns must contains the value found at string_mapping[OUTPUT_MAP]. 
 * This designates the column index of the first output column for the truth table which we will
 * call "n".
 * 
 * Line 2:
 * 
 * Each column should contain the name of a variable in the truth table. The values found in
 * columns 0 through (n-1) will be the names of the input variables and the values in all
 * subsequent columns will be the names of the output variables. The maximum number of output 
 * variables possible is defined by the compiler as SIZE_MAX.
 * 
 * All subsequent lines:
 * 
 * Each column should contain a "truth value". The valid values for these columns is true, false,
 * or don't care. The string mappings for these values are found at string_mapping[HI_MAP],
 * string_mapping[LO_MAP], and string_mapping[DC_MAP]. 
 * 
 * Every possible input combination should be accounted for in the table. However, this does not 
 * mean that every input combination needs to be present. For example, if I have 4 inputs and 1 
 * output and 1 row of the truth table is "1,1,1,x,1", this accounts for 2 possible input 
 * combinations: "1,1,1,1,1" and "1,1,1,0,1" so some input combinations can be implicitly 
 * included through th use of don't care bits.
 * 
 * There should also be no contradictory rows. This means that there should be no rows that 
 * contain the same input combinations but have different outputs
 * 
 * If there is an input combination that is not accounted for or there are contradictory rows, 
 * an error will be thrown. */
void truth_table::load(std::string filename) {
	try {
		/* Reset values */
		destruct();

		/* Define maximum allowed inputs */
		size_t g = SIZE_MAX;
		size_t max_inputs = 0;
		while (g) {
			max_inputs++;
			g >>= 1;
		}

		// The input stream from which the file will be read
		std::ifstream file;

		// The variable that will store each line of the input file as it is being parsed
		std::string line;

		/** In this step, we will define the table size parameters. These include
		 * <numInputs>, <numOutputs> and <tableHeight> */
		{
			// Open file
			file.open(filename, std::ios::in);

			size_t xIndex = 0;
			size_t yIndex = 0;
			size_t index1 = 0;
			size_t tempHeight = 0;
			std::string element = "";

			if (file.eof()) {
				// If the file is empty, throw an error
				file.close();
				throw new pgm_error(NOT_ENOUGH_ROWS,0,0);
			} else {
				// Otherwise, grab the first line.
				getline(file,line);
			}

			// Grab the index of the first delimiter
			size_t index2 = line.find(string_mapping[DELIM_MAP]);

			/** In this step, we will find the output column index and count the total
			 * number of inputs to be expected **/
			while (true) {
				if (index2 == std::string::npos) {
					// If no more delimiters are found - i.e. this is the last column

					// Trim the element found at this column
					element = trim(line.substr(index1,line.length() - index1));

					if (element.compare(string_mapping[OUTPUT_MAP]) != 0) {

						// If the element in this column is not the the Output column
						// identifier, throw an error as this indicates that no start
						// to the output columns could be found
						file.close();
						pgm_error* pg = new pgm_error(NOT_FOUND,0,1);
						pg->str_args[0] = string_mapping[OUTPUT_MAP];
						throw pg;
					} else {

						// If the element in this column is the Output column identifier
						// break out of this loop as this indeicates that the first
						// index of the input and output columns have both been found
						break;
					}
				}

				// If another delimiter has been found - i.e. this is not the last column

				// Trim the element found at this column
				element = trim(line.substr(index1,index2 - index1));

				if (index1 != 0) {

					// If this is not the first column

					if (element.compare(string_mapping[OUTPUT_MAP]) == 0) {

						// If the element in this column is the Output column identifier
						// break out of this loop as this indeicates that the first
						// index of the input and output columns have both been found
						break;
					} else if (element.compare("") == 0) {

						// If this column is empty

						if (numInputs == max_inputs) {

							// If the maximum number of input columns has been reached,
							// throw an error
							file.close();
							pgm_error* pg = new pgm_error(TOO_MANY_INPUTS,1,0);
							pg->setSizeArgs(0);
							throw pg;
						} else {

							// If the maximum number of input columns has not been reached,
							// incremend the number of input columns for this truth table
							numInputs++;
						}
					} else {

						// If this column is not empty and does not contain the Output
						// column identifer, throw an error
						file.close();
						pgm_error* pg = new pgm_error(UNRECOGNIZED_CHAR,1,1);
						pg->setSizeArgs(index1);
						pg->str_args[0] = line;
						throw pg;
					}
				} else {

					// If this is the first column

					if (element.compare(string_mapping[INPUT_MAP]) != 0) {

						// If the element in this column is not the Input column
						// identifier, throw an error as this indicates that no start
						// to the input columns could be found
						file.close();
						pgm_error* pg = new pgm_error(NOT_FOUND,0,1);
						pg->str_args[0] = string_mapping[INPUT_MAP];
						throw pg;
					} else {

						// If the element in this column is the Input column identifier,
						// increment the number of input columns for this truth table
						numInputs++;
					}
				}

				// Iterate to the next column
				index1 = index2 + 1;
				index2 = line.find(string_mapping[DELIM_MAP],index1);
			}

			// Designate a maximum height for the data in this truth table
			tempHeight = 1 << numInputs;

			if (file.eof()) {
				// If the file has ended, through an error
				file.close();
				throw new pgm_error(NOT_ENOUGH_ROWS,0,0);
			} else {
				// If the file has not ended, grab the second line
				getline(file,line);
			}

			/** In this step, we will count the total number of outputs to be expected */

			index1 = 0;
			// Grab the index of the first delimiter
			index2 = line.find(string_mapping[DELIM_MAP]);
			xIndex = 0;
			while (true) {
				if (index2 == std::string::npos) {

					// If this is the last column

					if (xIndex < numInputs) {

						// If the current column index is less than the total number of input
						// columns throw an error as this indicates that the file is 
						// missing all the output variables and some of the input variables
						file.close();
						pgm_error* pg = new pgm_error(NOT_ENOUGH_INPUTS,1,0);
						pg->setSizeArgs(1);
						throw pg;
					} else {

						// If the current column index is greater than or equal to the total 
						// number of input columns, then leave the loop as this means all
						// input and output columns have been parsed.
						break;
					}
				}

				// If this is not the last column

				if (xIndex >= numInputs) {

					// If the current column index is greater than the total number of input
					// columns, this means that the current column is an output column

					if (numOutputs == SIZE_MAX) {

						// If the maximum number of output columns has been reached, throw an 
						// error
						file.close();
						pgm_error* pg = new pgm_error(TOO_MANY_OUTPUTS,1,0);
						pg->setSizeArgs(1);
						throw pg;
					} else {

						// If the maximum number of output columns has not been reached, 
						// increment the total number of output columns
						numOutputs++;
					}
				}

				// Iterate to the next column
				xIndex++;
				index1 = index2 + 1;
				index2 = line.find(string_mapping[DELIM_MAP],index1);
			}

			if (numOutputs == SIZE_MAX) {
				// If the maximum number of output columns has been reached, throw an error
				file.close();
				pgm_error* pg = new pgm_error(TOO_MANY_OUTPUTS,1,0);
				pg->setSizeArgs(1);
				throw pg;
			} else {
				// If the maximum number of output columns has not been reached, increment the 
				// total number of output columns
				numOutputs++;
			}

			/** In this step, we will count the total number of subsequent lines there
			 * are which will be the expected table height **/
			while (!(file.eof())) { 
				getline(file,line);
				line = trim(line);
				if (line.compare("") != 0) {

					// If this line is not empty

					if (tableHeight == tempHeight) {
						// If the maxmimum number of table rows has been reached, throw an error
						file.close();
						throw new pgm_error(TOO_MANY_ROWS,0,0);
					} else {
						// If the maxmimum number of table rows has not been reached, increment
						// the total number of table rows for this truth table
						tableHeight++;
					}
				} else {
					break;
				}
			}

			// Close file
			file.close();
		}

		// If there are no data rows, throw an error
		if (tableHeight == 0) {
			throw new pgm_error(NOT_ENOUGH_ROWS,0,0);
		}

		/** In this step, we will define the <inputs> and <outputs> arrays */
		{
			// Reopen file
			file.open(filename, std::ios::in);

			// Skip the first line as they have already been parsed.
			getline(file,line);

			// Grab the next line (i.e. the one that will contain the variable names)
			getline(file,line);

			// Allocate variable arrays
			inputs = new std::string[numInputs];
			outputs = new std::string[numOutputs];

			/** Set the <inputs> and <outputs> arrays */

			size_t xIndex = 0;
			size_t index1 = 0;
			// Grab the index of the first delimiter
			size_t index2 = line.find(string_mapping[DELIM_MAP]);
			std::string element;

			// Iterate until a delimiter cannot be found
			while (index2 != std::string::npos) {
				// Grab the element in this column
				element = trim(line.substr(index1,index2 - index1));
				if (xIndex >= numInputs) {

					// If the current column index is greater than or equal to the total
					// number of inputs, then assume that the value at this column is 
					// an output variable name

					if (xIndex >= numOutputs + numInputs) {

						// If the current column index is greater than or equal to the 
						// total number inputs + the total number of outputs, then
						// throw an error as this means there are too many columns in
						// this row

						file.close();
						pgm_error* pg = new pgm_error(TOO_MANY_INPUTS,1,0);
						pg->setSizeArgs(1);
						throw pg;
					}

					if (element.compare("") == 0) {

						// If this column is empty, throw an error as there should be a variable
						// name in each column of this row

						file.close();
						throw new pgm_error(MISSING_ARG,0,0);
					}

					// Set the name of this output variable to the string value of this column
					outputs[xIndex - numInputs] = element;
				} else {

					// If the current column index is less than the total number of inputs, 
					// then assume that the value at this column is an input variable name

					if (element.compare("") == 0) {

						// If this column is empty, throw an error as there should be a variable
						// name in each column of this row

						file.close();
						throw new pgm_error(MISSING_ARG,0,0);
					}

					inputs[xIndex] = element;
				}

				// Iterate to the next column
				index1 = index2 + 1;
				index2 = line.find(string_mapping[DELIM_MAP],index1);
				xIndex++;
			}

			// Parse the last element of the row
			element = trim(line.substr(index1));
			if (element.compare("") == 0) {
				file.close();
				throw new pgm_error(MISSING_ARG,0,0);
			}
			if (xIndex < numInputs) {
				file.close();
				pgm_error* pg = new pgm_error(NOT_ENOUGH_INPUTS,1,0);
				pg->setSizeArgs(1);
				throw pg;
			}
			if (xIndex != numInputs + numOutputs - 1) {
				file.close();
				pgm_error* pg = new pgm_error(NOT_ENOUGH_OUTPUTS,1,0);
				pg->setSizeArgs(1);
				throw pg;
			}
			outputs[xIndex - numInputs] = element;
		}

		// Allocate <inputTable> and <outputTable> arrays
		inputTable = new char*[tableHeight];
		outputTable = new char*[tableHeight];
		for (size_t i = 0; i < tableHeight; i++) {
			inputTable[i] = new char[numInputs];
			outputTable[i] = new char[numOutputs];
		}

		/** In this step, we will define the data in the <inputTable> and <outputTable> 
		 * arrays */
		{
			size_t index1;
			size_t index2;
			size_t xIndex = 0;
			size_t yIndex = 0;
			size_t linenum = 0;
			std::string element = "";

			// Iterate until the end of the file
			while (!(file.eof())) {

				// Grab line
				getline(file,line);
				line = trim(line);

				xIndex = 0;

				// Skip row if it's empty
				if (line.compare("") != 0) {

					// Parse line
					index1 = 0;
					index2 = line.find(string_mapping[DELIM_MAP]);
					while (index2 != std::string::npos) {

						/* Parse element in row */

						// Throw error if there is too many columns
						if (xIndex >= numOutputs + numInputs) {
							file.close();
							pgm_error* pg = new pgm_error(TOO_MANY_ARGS,1,0);
							pg->setSizeArgs(linenum);
							throw pg;
						}

						// Grab element in row
						element = trim(line.substr(index1,index2 - index1));

						// Assign value to data at (xIndex,yIndex)
						if (element.compare(string_mapping[HI_MAP]) == 0) {
							if (xIndex >= numInputs) {
								outputTable[yIndex][xIndex - numInputs] = HI;
							} else {
								inputTable[yIndex][xIndex] = HI;
							}
						} else if (element.compare(string_mapping[LO_MAP]) == 0) {
							if (xIndex >= numInputs) {
								outputTable[yIndex][xIndex - numInputs] = LO;
							} else {
								inputTable[yIndex][xIndex] = LO;
							}
						} else if (element.compare(string_mapping[DC_MAP]) == 0) {
							if (xIndex >= numInputs) {
								outputTable[yIndex][xIndex - numInputs] = DC;
							} else {
								inputTable[yIndex][xIndex] = DC;
							}
						} else {
							file.close();
							pgm_error* pg = new pgm_error(UNRECOGNIZED_CHAR,1,1);
							pg->setSizeArgs(index1);
							pg->str_args[0] = line;
							throw pg;
						}
						index1 = index2 + 1;
						index2 = line.find(string_mapping[DELIM_MAP],index1);
						xIndex++;
					}
					if (xIndex >= numOutputs + numInputs) {
						file.close();
						pgm_error* pg = new pgm_error(TOO_MANY_ARGS,1,0);
						pg->setSizeArgs(linenum);
						throw pg;
					}
					element = trim(line.substr(index1));
					if (element.compare(string_mapping[HI_MAP]) == 0) {
						if (xIndex >= numInputs) {
							outputTable[yIndex][xIndex - numInputs] = HI;
						} else {
							inputTable[yIndex][xIndex] = HI;
						}
					} else if (element.compare(string_mapping[LO_MAP]) == 0) {
						if (xIndex >= numInputs) {
							outputTable[yIndex][xIndex - numInputs] = LO;
						} else {
							inputTable[yIndex][xIndex] = LO;
						}
					} else if (element.compare(string_mapping[DC_MAP]) == 0) {
						if (xIndex >= numInputs) {
							outputTable[yIndex][xIndex - numInputs] = DC;
						} else {
							inputTable[yIndex][xIndex] = DC;
						}
					} else {
						file.close();
						pgm_error* pg = new pgm_error(UNRECOGNIZED_CHAR,1,1);
						pg->setSizeArgs(index1);
						pg->str_args[0] = line;
						throw pg;
					}
					yIndex++;
				}
				linenum++;
			}
			file.close();
		}

		/** In this step, we will verify that all input combinations are accounted for and
		 * that there are no contradictory rows. We will do this by first 
		 * initializing a size_t array of size 2^numInputs called <cases>. We will then set all 
		 * values in this array to be -1. Each index of this array will correspond to a specific 
		 * row that should be in the truth table. For example, if we have 4 inputs, the data at 
		 * index 3 would correspond to the case where the first 2 inputs are set to low and the 
		 * last 2 inputs are set to high (i.e. "0,0,1,1"). This is because the binary 
		 * representation of 3 in 4 bits is 0011.
		 * 
		 * Then, we will iterate over every row in <inputTable>. If there are no indexes that
		 * contain a don't care bit in the given row, then we will just evaluate that input
		 * combination. If the corresponding entry in the <cases> array is -1, this means
		 * that this combination has not been accounted for yet so we will set the value
		 * at that index of <cases> to be the index of this input combination in <inputTable>		 * 
		 * 
		 * If the value at this index of <cases> is not -1, then this means that this input
		 * combination has already been found at that index stored in <cases>. If this is the 
		 * case, then we check the outputs at that index. If the outputs are the same as the 
		 * outputs at the current index of <inputTable>, then we do nothing, as no logical
		 * contradiction has been made. Those 2 rows are just duplicates. If the outputs are
		 * different, then the function throws an error as that implies that the same input
		 * combination can have 2 different outcomes.
		 * 
		 * If there are some indexes in a given row of <inputTable> that are don't care bits,
		 * then we "expand" that input combination into all the possible input combinations
		 * it contains. 
		 * 
		 * For example: If this truth table has the row "1,1,0,x", then we will check both
		 * "1,1,0,0" and "1,1,0,1". 
		 * 
		 * Once we have iterated over all the rows in <inputTable> this means that there are
		 * no contradictory rows. So, we check that all input combinations are accounted
		 * for by iterating through the <cases> array. If any of the values of <cases> are
		 * -1, then this indicates that this case was not accounted for so the function will
		 * throw an error */
		{

			// Initialize array of all cases
			size_t max_cases = 1 << numInputs;
			size_t cases[max_cases];
			for (size_t i = 0; i < max_cases; i++) {
				cases[i] = -1;
			}

			// Helper variables
			size_t num_dcs;
			size_t num_cases;
			size_t case_template;
			size_t dc_index;
			size_t current_case;

			// Iterate through each case in the table
			for (size_t i = 0; i < tableHeight; i++) {

				// num_dcs = number of don't care bits for this input combination
				num_dcs = numInputs - count_valid(inputTable[i],numInputs);
				case_template = 0;
				dc_index = 0;

				if (num_dcs > 0) {

					// If this input combination contains don't care bits

					// This array will contain the exact indexes of the don't care bits
					// within this input combination
					size_t dc_indexes[num_dcs];

					// We now set <case_template> tp be equal to this input combination
					// but we replace the don't care bits with LO bits
					for (size_t j = 0; j < numInputs; j++) {
						switch (inputTable[i][numInputs - j - 1]) {
							case HI:
								case_template |= (1 << j);
								break;
							case DC:
								dc_indexes[dc_index] = j;
								dc_index++;
								break;
						}
					}

					// <num_cases> = number of input combinations this expands to
					num_cases = 1 << num_dcs;

					// For each input combination in the set of expanded cases
					for (size_t j = 0; j < num_cases; j++) {

						// <current_case> = current case from expanded set of cases
						current_case = case_template;

						// Now, we set the "don't care" bits to the values that the
						// next input combination would have
						for (size_t k = 0; k < num_dcs; k++) {
							current_case |= ((j >> dc_indexes[k]) & 1) << dc_indexes[k];
						}

						if (cases[current_case] != -1) {

							// If this input combination has already been found

							// Check if the outputs at the 2 duplicate input combinations
							// are the same
							bool same_outputs = true;
							for (size_t k = 0; k < numOutputs && same_outputs; k++) {
								if (outputTable[i][k] != outputTable[cases[current_case]][k]) {
									same_outputs = false;
								}
							}

							if (!same_outputs) {
								// If the outputs are not the same, throw an error
								pgm_error* pg = new pgm_error(DUPLICATE_ARG,2,4);
								pg->setSizeArgs(cases[current_case],i);
								pg->str_args[0] = "";
								for (size_t k = 0; k < numInputs; k++) {
									pg->str_args[0] += inputTable[i][k];
								}
								pg->str_args[1] = "";
								for (size_t k = 0; k < numInputs; k++) {
									pg->str_args[1] += inputTable[cases[current_case]][k];
								}
								pg->str_args[2] = "";
								for (size_t k = 0; k < numOutputs; k++) {
									pg->str_args[2] += outputTable[i][k];
								}
								pg->str_args[3] = "";
								for (size_t k = 0; k < numOutputs; k++) {
									pg->str_args[3] += outputTable[cases[current_case]][k];
								}
								throw pg;
							}
						} else {

							// If this input combination has not yet been found, set the value
							// at this index to the current index of <inputTable>

							cases[current_case] = i;
						}
					}
				} else {

					// If this input combination has no don't care bits

					// Copy this input combination into the size_t variable <case_template>
					for (size_t j = 0; j < numInputs; j++) {
						if (inputTable[i][numInputs - j - 1] == HI) {
							case_template |= (1 << j);
						}
					}

					if (cases[case_template] != -1) {

						// If this input combination has already been accounted for

						// Check if the outputs of the duplicate input combinations match
						bool same_outputs = true;
						for (size_t k = 0; k < numOutputs && same_outputs; k++) {
							if (outputTable[i][k] != outputTable[cases[case_template]][k]) {
								same_outputs = false;
							}
						}

						if (!same_outputs) {
							
							// If the outputs do not match, throw and error
							pgm_error* pg = new pgm_error(DUPLICATE_ARG,2,4);
							pg->setSizeArgs(cases[case_template],i);
							pg->str_args[0] = "";
							for (size_t k = 0; k < numInputs; k++) {
								pg->str_args[0] += inputTable[i][k];
							}
							pg->str_args[1] = "";
							for (size_t k = 0; k < numInputs; k++) {
								pg->str_args[1] += inputTable[cases[case_template]][k];
							}
							pg->str_args[2] = "";
							for (size_t k = 0; k < numOutputs; k++) {
								pg->str_args[2] += outputTable[i][k];
							}
							pg->str_args[3] = "";
							for (size_t k = 0; k < numOutputs; k++) {
								pg->str_args[3] += outputTable[cases[case_template]][k];
							}
							throw pg;
						}
					} else {

						// If this input combination has not yet been found, set the value at 
						// this index to the current index of <inputTable>

						cases[case_template] = i;
					}
				}
			}

			// Verfiy that all cases have been accounted for
			for (size_t i = 0; i < max_cases; i++) {
				if (cases[i] == -1) {
					pgm_error* pg = new pgm_error(MISSING_ARG,0,1);
					pg->str_args[0] = toBin(i,numInputs);
					throw pg;
				}
			}
		}
	} catch (pgm_error* p) {

		// If an error was thrown, decontruct the truth_table and throw the error again

		destruct();
		throw p;
	}
}

/** Deconstructs the truth_table */
void truth_table::destruct() {
	if (inputs != nullptr) {
		delete[] inputs;
	}
	if (outputs != nullptr) {
		delete[] outputs;
	}
	if (inputTable != nullptr) {
		for (size_t i = 0; i < tableHeight; i++) {
			delete[] inputTable[i];
		}
		delete[] inputTable;
	}
	if (outputTable != nullptr) {
		for (size_t i = 0; i < tableHeight; i++) {
			delete[] outputTable[i];
		}
		delete[] outputTable;
	}

	inputs = nullptr;
	outputs = nullptr;
	inputTable = nullptr;
	outputTable = nullptr;
	numInputs = 0;
	numOutputs = 0;
	tableHeight = 0;
}

/** This function simplifies the 2 linked lists of cases. It assumes that <l1> contains
 * the cases where the output has the desired logic level and <l2> contains the cases
 * where logic level of the output is a don't care bit.
 * 
 * In other words, if this function was called from get_sum_of_products("d"), <l1> would 
 * contain all the cases where "d" is HI and <l2> would contain all the cases where "d" 
 * is DC. This is because when we are doint a sum of products, we simplify the cases where the 
 * desired variable is true.
 * 
 * Whereas if this function was called from get_product_of_sums("e"), <l1> would contain all the
 * cases where "e" is LO and <l2> would contain all the cases where "e" is DC. This is because
 * when we are doing a product of sums, we are trying to simplify the cases were the desired
 * output is false. */
void truth_table::reduce(linked_list<char*>* l1, linked_list<char*>* l2) {
	// Generate all simplified cases generated from simplifying the 2 lists
	generate_same_except(l1,l2);
	// Remove all redundant cases
	find_redundant(l1,l2);
}

/** This function takes a linked list of cases <l> and, for each case that contains don't care
 * bits, this function removes that case and replaces it with the input combinations that it
 * includes.
 * 
 * For example, if we say <l> is equal to this:
 * 		l = {
 * 			{0,0,1,x},
 * 			{0,1,1,0}
 * 		}
 * then this function will remove the first entry from the list and replace it with {0,0,1,0}
 * and {0,0,1,1} so that this is the resulting list: 
 * 
 * 		l = {
 * 			{0,0,1,0},
 * 			{0,0,1,1},
 * 			{0,1,1,0}
 * 		}
 * */
void truth_table::expand(linked_list<char*>* l) {

	// Initialize helper veriables
	node<char*>* n = l->head;
	linked_list<char*> g;
	size_t num_dcs;

	// Iterate through <l>
	for (size_t i = 0; i < l->length; i++) {

		// num_dc = the number of don't care bits in the current case
		num_dcs = numInputs - count_valid(n->data,numInputs);

		// Put the indexes of all the don't care bits in this case into the <dc_indexes> array
		size_t dc_indexes[num_dcs];
		size_t dc_index = 0;
		for (size_t j = 0; j < numInputs; j++) {
			if (n->data[j] == DC) {
				dc_indexes[dc_index] = j;
				n->data[j] = LO;
				dc_index++;
			}
		}

		// Iterate through all possible input combinations that are represented in this case
		dc_index = 0;
		size_t num_cases = 1 << num_dcs;
		for (size_t j = 1; j < num_cases; j++) {

			// Append a new char array of size <numInputs> to g
			g.append(new node<char*>(nullptr));
			g.tail->data = new char[numInputs];

			// Copy the current input combination into the char array
			for (size_t k = 0; k < numInputs; k++) {
				if (k == numInputs - dc_indexes[dc_index] - 1) {
					if (j & (1 << k)) {
						g.tail->data[numInputs - k - 1] = HI;
					} else {
						g.tail->data[numInputs - k - 1] = LO;
					}
					dc_index++;
				} else {
					g.tail->data[numInputs - k - 1] = n->data[numInputs - k - 1];
				}
			}
		}
		n = n->next;
	}

	// Put all the nodes of <g> into <l>
	while (g.length > 0) {
		l->append(g.pop());
	}
}

/** This function is used for formatting purposed when displaying the truth table.
 * It finds the length of the longest string in <list> */
size_t find_longest_length_str(std::string* list, size_t length, size_t curr_largest) {
	size_t longest = curr_largest;
	for (size_t i = 0; i < length; i++) {
		if (list[i].length() > longest) {
			longest = list[i].length();
		}
	}
	return longest;
}

/** This function is used for formatting purposed when displaying the truth table.
 * It returns <str> but with sufficient padding so that it has a length of <width> */
std::string get_block(std::string str, size_t width) {
	std::string result = "";
	if (width < str.length()) {
		width = str.length();
	}
	for (size_t i = 0; i < width; i++) {
		if (i < str.length()) {
			result += str[i];
		} else {
			result += " ";
		}
	}
	return result;
}
/** This function is used for formatting purposed when displaying the truth table.
 * It returns a string that contains <c> but with sufficient padding so that it has a length 
 * of <width> */
std::string get_block(char c, size_t width) {
	std::string result = "";
	if (width < 1) {
		width = 1;
	}
	result += c;
	for (size_t i = 1; i < width; i++) {
		result += " ";
	}
	return result;
}
/** This function takes 2 characters arrays pointed to by <op1> and <op2>, both of lenght <len>
 * and checks if they are considered "redundant cases".
 * 
 * 2 cases are said to be redundant if
 * 
 * 1) They are the same
 * Ex:
 * 		list = {
 * 			{1,0,0},
 * 			{0,0,1}, <- redundant
 * 			{0,0,1}  <- redundant
 * 		}
 * 2) One of the cases contains the other
 * Ex: The first case contains the third case.
 * 		list = {
 * 			{0,0,x}, <- redundant
 * 			{0,1,0},
 * 			{0,0,1}  <- redundant
 * 		}
 * The integer value it returns determines the relationship between the 2 cases.
 * 		0: The cases are unrelated
 * 		1: The cases are the same
 * 		2: <op1> contains <op2>
 * 		3: <op2> contains <op1>
 *  */
int check_redundant(char* op1, char* op2, size_t len) {
	int result = 1;
	for (size_t i = 0; i < len; i++) {
		if (op1[i] != op2[i]) {
			if (op1[i] == DC) {
				switch (result) {
					case 1:
						result = 2;
						break;
					case 3:
						return 0;
				}
			} else if (op2[i] == DC) {
				switch (result) {
					case 1:
						result = 3;
						break;
					case 2:
						return 0;
				}
			} else {
				return 0;
			}
		}
	}
	return result;
}
/** This function checks if 2 cases are the exact same with the exception of one bit that is 
 * flipped between the 2 cases. If the 2 cases ARE in fact the same with the exception of 1
 * bit, then this function returns the index of the don't care bit.
 * 
 * Example: The second and third cases in the list are the same with the exception of the last
 * bit
 * 		list = {
 * 			{1,1,1},
 * 			{0,0,0}, <-
 * 			{0,0,1}  <-
 * 		}
 *  */
size_t sameExcept(char* op1, char* op2, size_t len) {
	size_t result = -1;
	size_t num_diff = 0;
	bool cont = true;
	for (size_t i = 0; i < len && cont; i++) {
		switch (op1[i]) {
			case HI:
				switch (op2[i]) {
					case LO:
						if (num_diff == 1) {
							cont = false;
							result = -1;
						} else {
							num_diff++;
							result = i;
						}
						break;
					case DC:
						cont = false;
						result = -1;
						break;
				}
				break;
			case LO:
				switch (op2[i]) {
					case HI:
						if (num_diff == 1) {
							cont = false;
							result = -1;
						} else {
							num_diff++;
							result = i;
						}
						break;
					case DC:
						cont = false;
						result = -1;
						break;
				}
				break;
			case DC:
				if (op2[i] != DC) {
					cont = false;
					result = -1;
				}
				break;
		}
	}
	return result;
}
/** This function counts the total number of bits that are not don't cares in the given char
 * array <vals> which is of length <len> */
size_t count_valid(char* vals, size_t len) {
	size_t result = 0;
	for (size_t i = 0; i < len; i++) {
		if (vals[i] != DC) {
			result++;
		}
	}
	return result;
}
/** This function assumes that <l1> contains the cases where the desired output has the desired
 * logic level and that <l2> contains the case swhere the desired output is a don't care. It
 * does the majority of the simplification required to perform get_sum_of_products() and 
 * get_product_of_sums().
 * 
 * It does this by first, creating 2 new lists <temp_list1> and <temp_list2>. It then iterates 
 * through the 2 lists and finding all combinations of 2 cases that are the exact same with the 
 * exception of 1 bit that is flipped. It then takes these 2 cases and genrates a 3rd case that 
 * is the exact same as the previous 2 cases except that the bit that makes the 2 cases differ 
 * is replaced with a don't care bit. Then if both of the cases that were used to make this 
 * new case were from <l1>, it puts this new case in <temp_list1>, otherwise, it puts it into
 * <temp_list2>. If the generated case exists in the corresponding list already, then it 
 * destroys the generated case.
 * 
 * Once it has done this for all pairs for input combinations, it calls the reduce() function
 * on the resulting lists <temp_list1> and <temp_list2> where <temp_list1> is considered the
 * pertinent cases and <temp_list2> is considered the don't care cases. This ensures that
 * the final results in <temp_list1> and <temp_list2> contain the most general cases
 * are implied to be true by <l1> and <l2>. The contents of <temp_list1> and <temp_list2> are
 * then emptied and placed into <l1> and <l2> respectively.
 * */
bool truth_table::generate_same_except(linked_list<char*>* l1, linked_list<char*>* l2) {
	size_t index;
	bool result = false;
	bool m_go_to_next = true;
	bool m_in_l1;
	bool n_in_l1;
	node<char*>* n;
	node<char*>* m;
	node<char*>* p;
	if (l1->head == nullptr) {
		n = l2->head;
		n_in_l1 = false;
		m_in_l1 = false;
	} else {
		n = l1->head;
		n_in_l1 = true;
		m_in_l1 = true;
	}
	linked_list<char*>* temp_list1 = new linked_list<char*>();
	linked_list<char*>* temp_list2 = new linked_list<char*>();
	while (n != nullptr) {

		// Set m to be the next node after n
		if (n_in_l1) { // n is in l1
			if (n->next == nullptr) { // n is at the end of l1
				if (l2 == nullptr) { // there is no l2
					m = nullptr;
					m_in_l1 = true;
				} else { // there is an l2
					m = l2->head;
					m_in_l1 = false;
				}
			} else { // n is not at the end of l1
				m = n->next;
				m_in_l1 = true;
			}
		} else {
			m = n->next;
			m_in_l1 = false;
		}
		while (m != nullptr) {
			index = sameExcept(n->data,m->data,numInputs);
			if (index != -1) {

				// If the 2 cases are the same with the exception of 1 bit

				m_go_to_next = true;

				// Copy the data from node n into a new node
				node<char*>* p = new node<char*>();
				p->data = new char[numInputs];
				for (size_t i = 0; i < numInputs; i++) {
					p->data[i] = n->data[i];
				}
				p->data[index] = DC;

				// Place the new copied node into the correct list
				if (n_in_l1 && m_in_l1) {
					if (can_insert(p,temp_list1,numInputs)) {
						temp_list1->append(p);
					} else {
						delete[] p->data;
						delete p;
					}
				} else {
					if (can_insert(p,temp_list2,numInputs)) {
						temp_list2->append(p);
					} else {
						delete[] p->data;
						delete p;
					}
				}
				result = true;
			} else {

				// If the 2 cases are not the same with the exception of 1 bit

				m_go_to_next = true;
			}

			// Iterate m to the next node
			if (m_go_to_next) {
				if (m_in_l1) {
					if (m->next == nullptr) {
						if (l2 == nullptr) {
							m = nullptr;
						} else {
							m = l2->head;
							m_in_l1 = false;
						}
					} else {
						m = m->next;
					}
				} else {
					m = m->next;
				}
			}
		}

		// Iterate n to the next node
		if (n_in_l1) {
			if (n->next == nullptr) {
				if (l2 == nullptr) {
					n = nullptr;
				} else {
					n = l2->head;
					n_in_l1 = false;
				}
			} else {
				n = n->next;
			}
		} else {
			n = n->next;
		}
	}

	// If any simplification occured, reduce <temp_list1> and <temp_list2> and append their
	// contents to <l1> and <l2> respectively
	if (result) {
		reduce(temp_list1,temp_list2);
		node<char*>* q;
		while (temp_list1->head != nullptr) {
			q = temp_list1->head;
			temp_list1->pop(q);
			l1->prepend(q);
		}
		while (temp_list2->head != nullptr) {
			q = temp_list2->head;
			temp_list2->pop(q);
			l2->prepend(q);
		}
	}

	// Delete <temp_list1> and <temp_list2>
	delete temp_list1;
	delete temp_list2;
	return result;
}
/** This function determines if <n> can be inserted into <list> by verifying that it is not
 * a redundant case for this list. In other words, if it is the same as another input
 * combination in <list> or if  one of the input combinations in the list contains the case
 * <n> and, thereby, accounts for the case <n>.
 * 
 * If, by chance, the case <n> contains one of the cases in <list> then that case is removed
 * from <list> and the function returns true. */
bool can_insert(node<char*>* n, linked_list<char*>* list, size_t len) {
	if (list == nullptr) {
		return true;
	}
	node<char*>* m = list->head;
	int are_redundant;
	while (m != nullptr) {
		are_redundant = check_redundant(n->data,m->data,len);
		switch (are_redundant) {
			case 1:
			case 3:
				return false;
			case 2:
				list->pop(m);
				delete[] m->data;
				delete m;
				return true;
		}
		m = m->next;
	}
	return true;
}
/** This function iterates through every pair of cases in <l1> and <l2> and, if 2 redundant 
 * cases are found, it removes one of them. The result returned from check_redundant() 
 * determines which node to remove if a node needs to be removed.
 * 
 * For nodes <n> and <m>, if the result of check_redundant(n,m) is...
 * 0: The 2 cases are not redundant so neither is removed.
 * 1: The 2 cases are the same so <m> is removed.
 * 2: The case at <n> contains the case at <m> so <m> is removed.
 * 3: The case at <m> contains the case at <n> so <n> is removed.
 * 
 * This function returns true if any simplification occured.
 * */
bool truth_table::find_redundant(linked_list<char*>* l1, linked_list<char*>* l2) {
	bool result = true;
	bool m_in_l1;	// Tells whether <n> is currently in <l1> or not
	bool n_in_l1;	// Tells whether <m> is currently in <l1> or not

	// Tells whether, for the given iteration, <m> should be iterated to the next node.
	bool m_go_to_next = true;

	// Helper nodes
	node<char*>* n;
	node<char*>* m;
	node<char*>* p;

	// Stores the result of check_redundant(<n>,<m>)
	int are_redundant = 0;
	if (l1->head == nullptr) {
		n = l2->head;
		n_in_l1 = false;
		m_in_l1 = false;
	} else {
		n = l1->head;
		n_in_l1 = true;
		m_in_l1 = true;
	}
	while (n != nullptr) {

		// Set <m> as the next node after <n>
		if (n_in_l1) { // n is in l1
			if (n->next == nullptr) { // n = l1->tail
				if (l2 == nullptr) { // no l2
					m = nullptr;
					m_in_l1 = true;
				} else { // there is an l2
					m = l2->head;
					m_in_l1 = false;
				}
			} else { // n is in l1 but not at the end
				m = n->next;
				m_in_l1 = true;
			}
		} else { // n is not in l1
			m = n->next;
			m_in_l1 = false;
		}
		while (m != nullptr) {

			// Check if either node needs to be removed
			are_redundant = check_redundant(n->data,m->data,numInputs);
			switch (are_redundant) {
				case 0:
					m_go_to_next = true;
					break;
				case 1:
				case 2:

					/* Remove <m> */

					m_go_to_next = false;
					p = m;
					if (m->next == nullptr) {
						if (m == l1->tail) {
							if (l2 == nullptr) {
								m = nullptr;
								m_in_l1 = true;
							} else {
								m = l2->head;
								m_in_l1 = false;
							}
							l1->pop(p);
						} else {
							m = nullptr;
							m_in_l1 = false;
							l2->pop(p);
						}
					} else {
						m = m->next;
						if (m_in_l1) {
							l1->pop(p);
						} else {
							l2->pop(p);
						}
					}
					delete[] p->data;
					delete p;
					result = true;
					break;
				case 3:

					/* Remove <n> */

					m_go_to_next = false;
					if (m->next == nullptr) {
						if (m == l1->tail) {
							if (l2 == nullptr) {
								p = nullptr;
								m_in_l1 = true;
							} else {
								p = l2->head;
								m_in_l1 = false;
							}
						} else {
							p = nullptr;
							m_in_l1 = false;
						}
					} else {
						p = m->next;
					}
					if (m_in_l1) {
						l1->pop(m);
					} else {
						l2->pop(m);
					}
					if (n_in_l1) {
						l1->insertAfter(n,m);
						m = p;
						p = n;
						n = n->next;
						l1->pop(p);
						delete[] p->data;
						delete p;
					} else {
						l2->insertAfter(n,m);
						m = p;
						p = n;
						n = n->next;
						l2->pop(p);
						delete[] p->data;
						delete p;
					}
					result = true;
					break;
			}

			// Iterate <m> to next node
			if (m_go_to_next) {
				if (m_in_l1) { // m is in l1
					if (m->next == nullptr) { // m is at the end of l1
						if (l2 == nullptr) { // there is not l2
							m = nullptr;
							m_in_l1 = true;
						} else { // there is an l2
							m = l2->head;
							m_in_l1 = false;
						}
					} else { // m is in l1 but not at the end
						m = m->next;
						m_in_l1 = true;
					}
				} else {
					m = m->next;
					m_in_l1 = false;
				}
			} else {
				m_go_to_next = true;
			}
		}

		// Iterate <n> to next node
		if (n->next == nullptr) {
			if (n == l1->tail) {
				if (l2 == nullptr) {
					n = nullptr;
					n_in_l1 = true;
				} else {
					n = l2->head;
					n_in_l1 = false;
				}
			} else {
				n = nullptr;
				n_in_l1 = false;
			}
		} else {
			n = n->next;
		}
	}
	return result;
}
