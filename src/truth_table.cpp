truth_table::truth_table() {
	inputs = nullptr;
	outputs = nullptr;
	inputTable = nullptr;
	outputTable = nullptr;
	numInputs = 0;
	numOutputs = 0;
	tableHeight = 0;
}
truth_table::truth_table(std::string* inp, std::string* outp, char** inpTab, char** outpTab, int numInp, int numOutp, int tabHeight = -1) {
	inputs = inp;
	outputs = outp;
	inputTable = inpTab;
	outputTable = outpTab;
	numInputs = numInp;
	numOutputs = numOutp;
	tableHeight = (tabHeight == -1)?(1 << numInp):(tabHeight);
}
truth_table::truth_table(std::string filename) {
	std::ifstream file;
	std::string line;
	int yIndex = 0;
	int xIndex = 0;
	int outIndex = 0;
	int max = 0;
	bool inpu = true;
	bool vals = false;
	std::string var = "";

	numInputs = 0;
	numOutputs = 0;
	tableHeight = 0;

	file.open(filename, std::ios::in);

	while (!file.eof()) { 
		getline(file,line);
		if (yIndex == 0) {
			outIndex = line.find("Outputs");
			for (int i = 0; i < line.length(); i++) {
				if (line[i] == ',') {
					if (i < outIndex) {
						numInputs++;
					} else {
						numOutputs++;
					}
				}
			}
			numOutputs++;
		} else if (yIndex >= 2) {
			tableHeight++;
		}
		yIndex++;
	}
	file.close();

	inputs = new std::string[numInputs];
	outputs = new std::string[numOutputs];
	inputTable = new char*[tableHeight];
	outputTable = new char*[tableHeight];

	for (int i = 0; i < tableHeight; i++) {
		inputTable[i] = new char[numInputs];
		outputTable[i] = new char[numOutputs];
	}

	file.open(filename, std::ios::in);
	yIndex = 0;
	while (!file.eof()) {
		xIndex = 0;
		inpu = true;
		getline(file,line);
		if (yIndex == 0 && !vals) {
			for (int i = 0; i < line.length(); i++) {
				if (line[i] == ',') {
					xIndex++;
				} else {
					if (i == outIndex) {
						outIndex = xIndex;
					}
				}
			}
		} else if (yIndex == 1 && !vals) {
			for (int i = 0; i < line.length(); i++) {
				if (line[i] == ',' || i == line.length() - 1) {
					if (xIndex < outIndex && inpu) {
						inputs[xIndex] = var;
					} else {
						if (inpu) {
							inpu = false;
							xIndex = 0;
						}
						outputs[xIndex] = var;
					}
					var = "";
					xIndex++;
				} else {
					var += line[i];
				}
				if (i == line.length() - 1) {
					outputs[numOutputs - 1] += line[i];
				}
			}
		} else {
			if (!vals) {
				vals = true;
				yIndex = 0;
			}
			for (int i = 0; i < line.length(); i++) {
				if (line[i] != ',') {
					if (xIndex < outIndex && inpu) {
						inputTable[yIndex][xIndex] = line[i];
					} else {
						if (inpu) {
							inpu = false;
							xIndex = 0;
						}
						outputTable[yIndex][xIndex] = line[i];
					}
					xIndex++;
				}
			}
		}
		yIndex++;
	}
}
truth_table::~truth_table() {
	if (inputs != nullptr) {
		delete[] inputs;
	}
	if (outputs != nullptr) {
		delete[] outputs;
	}
	if (inputTable != nullptr) {
		for (int i = 0; i < tableHeight; i++) {
			if (inputTable[i] != nullptr) {
				delete[] inputTable[i];
			}
		}
		delete[] inputTable;
	}
	if (outputTable != nullptr) {
		for (int i = 0; i < tableHeight; i++) {
			if (outputTable[i] != nullptr) {
				delete[] outputTable[i];
			}
		}
		delete[] outputTable;
	}
}

linked_list<char*>* truth_table::get_cases_where(std::string var, char val) {
	linked_list<char*>* l = new linked_list<char*>();
	int index = indexOfOutput(var);
	for (int i = 0; i < tableHeight; i++) {
		if (outputTable[i][index] == val) {
			l->append(new char[numInputs]);
			for (int j = 0; j < numInputs; j++) {
				l->tail->data[j] = inputTable[i][j];
			}
		}
	}
	return l;
}
log* truth_table::get_sum_of_products(std::string var) {
	linked_list<char*>* cases = get_cases_where(var,HI);
	reduce(cases);
	log* l = convert_to_log(cases);
	cases->destruct_arr();
	delete cases;
	return l;
}
log* truth_table::get_product_of_sums(std::string var) {
	linked_list<char*>* cases = get_cases_where(var,LO);
	reduce(cases);
	log* l = convert_to_log(cases,false);
	cases->destruct_arr();
	delete cases;
	return l;
}
int truth_table::indexOfOutput(std::string var) {
	for (int i = 0; i < numOutputs; i++) {
		if (var.compare(outputs[i]) == 0) {
			return i;
		}
	}
	return -1;
}
std::ostream& operator<<(std::ostream& out, truth_table& t) {

	// Get proper formatting width
	int len = find_longest_length_str(t.inputs,t.numInputs,0);
	len = find_longest_length_str(t.outputs,t.numOutputs,len) + 1;

	// Print the input variables
	for (int i = 0; i < t.numInputs; i++) {
		out << get_block(t.inputs[i],len);
	}
	out << "|";

	// Print the output variables
	for (int i = 0; i < t.numOutputs; i++) {
		out << get_block(t.outputs[i],len);
	}
	out << std::endl;

	int tab_width = t.numInputs * len;
	for (int i = 0; i < tab_width; i++) {
		out << '-';
	}
	tab_width = t.numOutputs * len;
	out << '|';
	for (int i = 0; i < tab_width; i++) {
		out << '-';
	}
	out << std::endl;
	
	for (int i = 0; i < t.tableHeight; i++) {
		for (int j = 0; j < t.numInputs; j++) {
			out << get_block(t.inputTable[i][j],len);
		}
		out << '|';
		for (int j = 0; j < t.numOutputs; j++) {
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
					for (int i = 0; i < numInputs; i++) {
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
					for (int i = 0; i < numInputs; i++) {
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
			for (int i = 0; i < l->length; i++) {
				switch (count_valid(l->get(i),numInputs)) {
					case 0: // current case has no inputs set -> temp1 = HI, result |= temp1
						temp1 = new log();
						temp1->val = is_sop?HI:LO;
						temp1->type = LITERAL;
						result->args.append(temp1);
						delete temp1;
						break;
					case 1: // current case has only one input set -> temp1 = "var", result |= temp1 || temp2 = "var", temp1 = NOT(temp2), result |= temp1
						for (int j = 0; j < numInputs; j++) {
							if (l->get(i)[j] != DC) { // Input j is set -> add this input
								if (is_sop) {
									if (l->get(i)[j] == HI) { // Input j is set HI -> temp1 = "var", result &= temp1
										temp1 = new log();
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
						for (int j = 0; j < numInputs; j++) {
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
void truth_table::reduce(linked_list<char*>* l) {
	while (find_same_except(l,numInputs) || find_contains_except(l,numInputs));
}

int find_longest_length_str(std::string* list, int length, int curr_largest) {
	int longest = curr_largest;
	for (int i = 0; i < length; i++) {
		if (list[i].length() > longest) {
			longest = list[i].length();
		}
	}
	return longest;
}

std::string get_block(std::string str, int width) {
	std::string result = "";
	if (width < str.length()) {
		width = str.length();
	}
	for (int i = 0; i < width; i++) {
		if (i < str.length()) {
			result += str[i];
		} else {
			result += " ";
		}
	}
	return result;
}
std::string get_block(char c, int width) {
	std::string result = "";
	if (width < 1) {
		width = 1;
	}
	result += c;
	for (int i = 1; i < width; i++) {
		result += " ";
	}
	return result;
}

bool same(char* op1, char* op2, int len) {
	bool result = true;
	for (int i = 0; i < len; i++) {
		if (op1[i] != op2[i]) {
			result = false;
			break;
		}
	}
	return result;
}
bool contains(char* container, char* containee, int len) {
	bool result = true;
	bool contained_found = false;
	for (int i = 0; i < len; i++) {
		if (container[i] != containee[i]) {
			if (container[i] == DC) {
				contained_found = true;
			} else {
				if (containee[i] == DC) {
					result = false;
					break;
				}
			}
		}
	}
	return result && contained_found;
}
int containsExcept(char* container, char* containee, int len) {
	char* temp = new char[len];
	int index;
	for (int i = 0; i < len; i++) {
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
int sameExcept(char* op1, char* op2, int len) {
	int result = -1;
	int num_diff = 0;
	bool cont = true;
	for (int i = 0; i < len && cont; i++) {
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
int count_valid(char* vals, int len) {
	int result = 0;
	for (int i = 0; i < len; i++) {
		if (vals[i] != DC) {
			result++;
		}
	}
	return result;
}
bool find_same_except(linked_list<char*>* l, int num_i) {
	int index;
	for (int i = 0; i < l->length; i++) {
		for (int j = i + 1; j < l->length; j++) {
			index = sameExcept(l->get(i),l->get(j),num_i);
			if (index != -1) {
				l->get(i)[index] = DC;
				l->remove(j,2);
				return true;
			}
		}
	}
	return false;
}
bool find_contains_except(linked_list<char*>* l, int num_i) {
	int index;
	for (int i = 0; i < l->length; i++) {
		for (int j = 0; j < l->length; j++) {
			if (i != j) {
				index = containsExcept(l->get(i),l->get(j),num_i);
				if (index != -1) {
					l->get(j)[index] = DC;
					return true;
				}
			}
		}
	}
	return false;
}
