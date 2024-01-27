truth_table::truth_table() {
	inputs = nullptr;
	outputs = nullptr;
	inputTable = nullptr;
	outputTable = nullptr;
	numInputs = 0;
	numOutputs = 0;
	tableHeight = 0;
}
truth_table::truth_table(std::string* inp, std::string* outp, char** inpTab, char** outpTab, size_t numInp, size_t numOutp, size_t tabHeight = -1) {
	inputs = inp;
	outputs = outp;
	inputTable = inpTab;
	outputTable = outpTab;
	numInputs = numInp;
	numOutputs = numOutp;
	tableHeight = (tabHeight == -1)?(1 << numInp):(tabHeight);
}
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

linked_list<char*>* truth_table::get_cases_where(std::string var, char val) {
	linked_list<char*>* l = new linked_list<char*>();
	size_t index = indexOfOutput(var);
	for (size_t i = 0; i < tableHeight; i++) {
		if (outputTable[i][index] == val) {
			l->append(new char[numInputs]);
			for (size_t j = 0; j < numInputs; j++) {
				l->tail->data[j] = inputTable[i][j];
			}
		}
	}
	return l;
}
log* truth_table::get_sum_of_products(std::string var) {
	linked_list<char*>* hi_cases = get_cases_where(var,HI);
	linked_list<char*>* dc_cases = get_cases_where(var,DC);
	expand(dc_cases);
	reduce(hi_cases,dc_cases);
	log* l = convert_to_log(hi_cases);
	hi_cases->destruct_arr();
	dc_cases->destruct_arr();
	delete hi_cases;
	delete dc_cases;
	return l;
}
log* truth_table::get_product_of_sums(std::string var) {
	linked_list<char*>* lo_cases = get_cases_where(var,LO);
	linked_list<char*>* dc_cases = get_cases_where(var,DC);
	expand(dc_cases);
	reduce(lo_cases,dc_cases);
	log* l = convert_to_log(lo_cases,false);
	lo_cases->destruct_arr();
	dc_cases->destruct_arr();
	delete lo_cases;
	delete dc_cases;
	return l;
}
size_t truth_table::indexOfOutput(std::string var) {
	for (size_t i = 0; i < numOutputs; i++) {
		if (var.compare(outputs[i]) == 0) {
			return i;
		}
	}
	return -1;
}
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

		std::ifstream file;
		std::string line;

		/* Define table size */
		{
			// Open file
			file.open(filename, std::ios::in);

			size_t xIndex = 0;
			size_t yIndex = 0;
			size_t index1 = 0;
			size_t tempHeight = 0;
			std::string element = "";

			if (file.eof()) {
				file.close();
				throw new pgm_error(NOT_ENOUGH_ROWS,0,0);
			} else {
				getline(file,line);
			}
			size_t index2 = line.find(string_mapping[DELIM_MAP]);

			while (true) {
				if (index2 == std::string::npos) {
					element = trim(line.substr(index1,line.length() - index1));
					if (element.compare(string_mapping[OUTPUT_MAP]) != 0) {
						file.close();
						pgm_error* pg = new pgm_error(NOT_FOUND,0,1);
						pg->str_args[0] = string_mapping[OUTPUT_MAP];
						throw pg;
					} else {
						break;
					}
				}
				element = trim(line.substr(index1,index2 - index1));
				if (index1 != 0) {
					if (element.compare(string_mapping[OUTPUT_MAP]) == 0) {
						break;
					} else if (element.compare("") == 0) {
						if (numInputs == max_inputs) {
							file.close();
							pgm_error* pg = new pgm_error(TOO_MANY_INPUTS,1,0);
							pg->setSizeArgs(0);
							throw pg;
						} else {
							numInputs++;
						}
					} else {
						file.close();
						pgm_error* pg = new pgm_error(UNRECOGNIZED_CHAR,1,1);
						pg->setSizeArgs(index1);
						pg->str_args[0] = line;
						throw pg;
					}
				} else {
					if (element.compare(string_mapping[INPUT_MAP]) != 0) {
						file.close();
						pgm_error* pg = new pgm_error(NOT_FOUND,0,1);
						pg->str_args[0] = string_mapping[INPUT_MAP];
						throw pg;
					} else {
						numInputs++;
					}
				}
				index1 = index2 + 1;
				index2 = line.find(string_mapping[DELIM_MAP],index1);
			}
			tempHeight = 1 << numInputs;

			if (file.eof()) {
				throw new pgm_error(NOT_ENOUGH_ROWS,0,0);
			} else {
				getline(file,line);
			}

			index1 = 0;
			index2 = line.find(string_mapping[DELIM_MAP]);
			xIndex = 0;
			while (true) {
				if (index2 == std::string::npos) {
					if (xIndex < numInputs) {
						file.close();
						pgm_error* pg = new pgm_error(NOT_ENOUGH_INPUTS,1,0);
						pg->setSizeArgs(1);
						throw pg;
					} else {
						break;
					}
				}
				if (xIndex >= numInputs) {
					if (numOutputs == SIZE_MAX) {
						file.close();
						pgm_error* pg = new pgm_error(TOO_MANY_OUTPUTS,1,0);
						pg->setSizeArgs(1);
						throw pg;
					} else {
						numOutputs++;
					}
				}
				xIndex++;
				index1 = index2 + 1;
				index2 = line.find(string_mapping[DELIM_MAP],index1);
			}
			if (numOutputs == SIZE_MAX) {
				file.close();
				pgm_error* pg = new pgm_error(TOO_MANY_OUTPUTS,1,0);
				pg->setSizeArgs(1);
				throw pg;
			} else {
				numOutputs++;
			}

			while (!(file.eof())) { 
				getline(file,line);
				line = trim(line);
				if (line.compare("") != 0) {
					if (tableHeight == tempHeight) {
						file.close();
						throw new pgm_error(TOO_MANY_ROWS,0,0);
					} else {
						tableHeight++;
					}
				}
			}

			// Close file
			file.close();
		}

		if (tableHeight == 0) {
			throw new pgm_error(NOT_ENOUGH_ROWS,0,0);
		}

		/* Set up input and output variables */
		{
			// Reopen file
			file.open(filename, std::ios::in);
			getline(file,line);
			getline(file,line);

			// Allocate variable arrays
			inputs = new std::string[numInputs];
			outputs = new std::string[numOutputs];

			size_t xIndex = 0;
			size_t index1 = 0;
			size_t index2 = line.find(string_mapping[DELIM_MAP]);
			std::string element;
			while (index2 != std::string::npos) {
				element = trim(line.substr(index1,index2 - index1));
				if (xIndex >= numInputs) {
					if (xIndex >= numOutputs + numInputs) {
						file.close();
						pgm_error* pg = new pgm_error(TOO_MANY_INPUTS,1,0);
						pg->setSizeArgs(1);
						throw pg;
					} else {
						if (element.compare("") != 0) {
							outputs[xIndex - numInputs] = element;
						} else {
							file.close();
							throw new pgm_error(MISSING_ARG,0,0);
						}
					}
				} else {
					if (element.compare("") != 0) {
						inputs[xIndex] = element;
					} else {
						file.close();
						throw new pgm_error(MISSING_ARG,0,0);
					}
				}
				index1 = index2 + 1;
				index2 = line.find(string_mapping[DELIM_MAP],index1);
				xIndex++;
			}
			element = trim(line.substr(index1));
			if (element.compare("") != 0) {
				if (xIndex >= numInputs) {
					if (xIndex == numInputs + numOutputs - 1) {
						outputs[xIndex - numInputs] = element;
					} else {
						file.close();
						pgm_error* pg = new pgm_error(NOT_ENOUGH_OUTPUTS,1,0);
						pg->setSizeArgs(1);
						throw pg;
					}
				} else {
					file.close();
					pgm_error* pg = new pgm_error(NOT_ENOUGH_INPUTS,1,0);
					pg->setSizeArgs(1);
					throw pg;
				}
			} else {
				file.close();
				throw new pgm_error(MISSING_ARG,0,0);
			}
		}

		// Allocate data matrices
		inputTable = new char*[tableHeight];
		outputTable = new char*[tableHeight];
		for (size_t i = 0; i < tableHeight; i++) {
			inputTable[i] = new char[numInputs];
			outputTable[i] = new char[numOutputs];
		}

		/* Set up table rows */
		{
			size_t index1;
			size_t index2;
			size_t xIndex = 0;
			size_t yIndex = 0;
			size_t linenum = 0;
			std::string element = "";
			while (!(file.eof())) {

				// Grab row
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

		/* Verify that all cases are accounted for and there are no contradictions */
		{


			// Initialize array of all cases
			size_t max_cases = 1 << numInputs;
			char cases[max_cases];
			for (size_t i = 0; i < max_cases; i++) {
				cases[i] = -1;
			}

			// Verify
			size_t num_dcs;
			size_t num_cases;
			size_t case_template;
			size_t dc_index;
			size_t current_case;

			// Iterate through each case in the table
			for (size_t i = 0; i < tableHeight; i++) {

				/* Set up initial case with dcs */

				// num_dcs = number of don't care bits in this case
				num_dcs = numInputs - count_valid(inputTable[i],numInputs);
				case_template = 0;
				dc_index = 0;

				if (num_dcs > 0) {
					// dc_indexes = array of indexes of don't care bits
					size_t dc_indexes[num_dcs];

					// Create case template with dc bit = lo bit
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

					// num_cases = number of cases this case expands to
					num_cases = 1 << num_dcs;

					// For each case in the set of expanded cases
					for (size_t j = 0; j < num_cases; j++) {

						// current_case = current case from expanded set of cases
						current_case = case_template;

						// Create current case
						for (size_t k = 0; k < num_dcs; k++) {
							current_case |= ((j >> dc_indexes[k]) & 1) << dc_indexes[k];
						}

						if (cases[current_case] != -1) {
							bool same_outputs = true;
							for (size_t k = 0; k < numOutputs && same_outputs; k++) {
								if (outputTable[i][k] != outputTable[cases[current_case]][k]) {
									same_outputs = false;
								}
							}

							if (!same_outputs) {
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
							cases[current_case] = i;
						}
					}
				} else {
					for (size_t j = 0; j < numInputs; j++) {
						if (inputTable[i][numInputs - j - 1] == HI) {
							case_template |= (1 << j);
						}
					}

					if (cases[case_template] != -1) {
						bool same_outputs = true;
						for (size_t k = 0; k < numOutputs && same_outputs; k++) {
							if (outputTable[i][k] != outputTable[cases[case_template]][k]) {
								same_outputs = false;
							}
						}

						if (!same_outputs) {
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
		destruct();
		throw p;
	}
}
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
void truth_table::reduce(linked_list<char*>* l1, linked_list<char*>* l2) {
	find_same(l1,l2);
	find_contains(l1,l2);
	find_same_except(l1,l2);
	find_same(l1,l2);
	find_contains(l1,l2);
	find_same_except(l1,l2,true);
	find_same(l1,l2);
	find_contains(l1,l2);
}
void truth_table::expand(linked_list<char*>* l) {
	node<char*>* n = l->head;
	linked_list<char*> g;
	size_t num_dcs;
	for (size_t i = 0; i < l->length; i++) {
		num_dcs = numInputs - count_valid(n->data,numInputs);
		size_t dc_indexes[num_dcs];
		size_t dc_index = 0;
		for (size_t j = 0; j < numInputs; j++) {
			if (n->data[j] == DC) {
				dc_indexes[dc_index] = j;
				n->data[j] = LO;
				dc_index++;
			}
		}
		dc_index = 0;
		size_t num_cases = 1 << num_dcs;
		for (size_t j = 1; j < num_cases; j++) {
			g.append(new node<char*>(nullptr));
			g.tail->data = new char[numInputs];
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

	while (g.length > 0) {
		l->append(g.pop());
	}
}

size_t find_longest_length_str(std::string* list, size_t length, size_t curr_largest) {
	size_t longest = curr_largest;
	for (size_t i = 0; i < length; i++) {
		if (list[i].length() > longest) {
			longest = list[i].length();
		}
	}
	return longest;
}

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

bool same(char* op1, char* op2, size_t len) {
	bool result = true;
	for (size_t i = 0; i < len; i++) {
		if (op1[i] != op2[i]) {
			result = false;
			break;
		}
	}
	return result;
}
bool contains(char* container, char* containee, size_t len) {
	bool result = true;
	bool contained_found = false;
	for (size_t i = 0; i < len; i++) {
		if (container[i] != containee[i]) {
			if (container[i] != DC) {
				return false;
			}
		}
	}
	return true;
}
size_t containsExcept(char* container, char* containee, size_t len) {
	char* temp = new char[len];
	size_t index;
	for (size_t i = 0; i < len; i++) {
		if (container[i] == DC) {
			temp[i] = containee[i];
		} else {
			temp[i] = container[i];
		}
	}
	index = sameExcept(temp,containee,len);
	delete[] temp;
	return index;
}
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
size_t count_valid(char* vals, size_t len) {
	size_t result = 0;
	for (size_t i = 0; i < len; i++) {
		if (vals[i] != DC) {
			result++;
		}
	}
	return result;
}
bool truth_table::find_same(linked_list<char*>* l1, linked_list<char*>* l2) {
	bool result = true;
	bool m_in_l1;
	bool m_go_to_next = true;
	node<char*>* n;
	node<char*>* m;
	node<char*>* p;
	if (l1->head == nullptr) {
		n = l2->head;
		m_in_l1 = false;
	} else {
		n = l1->head;
		m_in_l1 = true;
	}
	while (n != nullptr) {
		if (n->next == nullptr) {
			if (n == l1->tail) {
				if (l2 == nullptr) {
					m = nullptr;
					m_in_l1 = true;
				} else {
					m = l2->head;
					m_in_l1 = false;
				}
			} else {
				m = nullptr;
				m_in_l1 = false;
			}
		} else {
			m = n->next;
		}
		while (m != nullptr) {
			if (same(n->data,m->data,numInputs)) {
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
			} else {
				m_go_to_next = true;
			}
			if (m_go_to_next) {
				if (m->next == nullptr) {
					if (m == l1->tail) {
						if (l2 == nullptr) {
							m = nullptr;
						} else {
							m = l2->head;
						}
					} else {
						m = nullptr;
					}
				} else {
					m = m->next;
				}
			}
		}
		if (n->next == nullptr) {
			if (n == l1->tail) {
				if (l2 == nullptr) {
					n = nullptr;
				} else {
					n = l2->head;
				}
			} else {
				n = nullptr;
			}
		} else {
			n = n->next;
		}
	}
	return result;
}
bool truth_table::find_same_except(linked_list<char*>* l1, linked_list<char*>* l2, bool simplify) {
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
				if (simplify) {
					m_go_to_next = false;
					p = m;
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
						l1->pop(p);
					} else {
						m = m->next;
						l2->pop(p);
					}
					delete[] p->data;
					delete p;
					result = true;
				} else {
					m_go_to_next = true;
					node<char*>* p = new node<char*>();
					p->data = new char[numInputs];
					for (size_t i = 0; i < numInputs; i++) {
						p->data[i] = n->data[i];
					}
					p->data[index] = DC;
					if (n_in_l1 && m_in_l1) {
						temp_list1->append(p);
					} else {
						temp_list2->append(p);
					}
				}
				result = true;
			} else {
				m_go_to_next = true;
			}
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
	if (result && !simplify) {
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
	delete temp_list1;
	delete temp_list2;
	return result;
}
bool truth_table::find_contains_except(linked_list<char*>* l1, linked_list<char*>* l2) {
	if (l1->head == nullptr) {
		return false;
	}
	size_t index;
	bool n_in_l1 = true;
	bool m_in_l1 = true;
	bool result = false;
	node<char*>* n = l1->head;
	node<char*>* m;
	while (n != nullptr) {
		if (n_in_l1) {
			if (n->next == nullptr) {
				m = l2->head;
				m_in_l1 = false;
			} else {
				m = n->next;
				m_in_l1 = true;
			}
		} else {
			m = n->next;
			m_in_l1 = false;
		}
		while (m != nullptr) {
			index = containsExcept(n->data,m->data,numInputs);
			if (index == -1) {
				index = containsExcept(m->data,n->data,numInputs);
				if (index != -1) {
					n->data[index] = DC;
					result = true;
				}
			} else {
				m->data[index] = DC;
				result = true;
			}
			if (m_in_l1) {
				if (m->next == nullptr) {
					m = l2->head;
					m_in_l1 = false;
				} else {
					m = m->next;
					m_in_l1 = true;
				}
			} else {
				m = m->next;
				m_in_l1 = false;
			}
		}
		if (n_in_l1) {
			if (n->next == nullptr) {
				n = l2->head;
				n_in_l1 = false;
			} else {
				n = n->next;
				n_in_l1 - true;
			}
		} else {
			n = n->next;
			n_in_l1 = false;
		}
	}
	return result;
}
bool truth_table::find_contains(linked_list<char*>* l1, linked_list<char*>* l2) {
	bool result = true;
	bool m_in_l1;
	bool n_in_l1;
	bool m_go_to_next = true;
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
	while (n != nullptr) {
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
			if (contains(n->data,m->data,numInputs)) {
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
			} else {
				if (contains(m->data,n->data,numInputs)) {
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
				}
			}
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
void print(linked_list<char*>* l, size_t num_i) {
	node<char*>* n = l->head;
	printSpecial("------------------");
	std::string line;
	while (n != nullptr) {
		line = "";
		for (size_t j = 0; j < num_i; j++) {
			line += n->data[j];
		}
		printSpecial(line);
		n = n->next;
	}
	printSpecial("------------------");
}
void print(node<char*>* n, size_t num_i) {
	printSpecial("------------------");
	std::string fn = "";
	for (size_t i = 0; i < num_i; i++) {
		fn += n->data[i];
	}
	printSpecial(fn);
	printSpecial("------------------");
}
void printSpecial(std::string str) {
	for (int i = 0; i < iteration; i++) {
		std::cout << "   ";
	}
	std::cout << str << std::endl;
}
