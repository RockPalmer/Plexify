instruction::instruction() {
	is_choice = false;
	output = "";
	choice[0] = nullptr;
	choice[1] = nullptr;
}

instruction::~instruction() {
	if (is_choice) {
		if (choice[0] != nullptr) {
			delete choice[0];
		}
		if (choice[1] != nullptr) {
			delete choice[1];
		}
	}
}

truth_table::truth_table() {
	inputs = nullptr;
	outputs = nullptr;
	inputTable = nullptr;
	outputTable = nullptr;
	numInputs = 0;
	numOutputs = 0;
	tableHeight = 0;
}
truth_table::truth_table(string* inp, string* outp, char** inpTab, char** outpTab, int numInp, int numOutp, int tabHeight = -1) {
	inputs = inp;
	outputs = outp;
	inputTable = inpTab;
	outputTable = outpTab;
	numInputs = numInp;
	numOutputs = numOutp;
	tableHeight = (tabHeight == -1)?(1 << numInp):(tabHeight);
}
truth_table::truth_table(string filename) {
	ifstream file;
	string line;
	int yIndex = 0;
	int xIndex = 0;
	int outIndex = 0;
	int max = 0;
	bool inpu = true;
	bool vals = false;
	string var = "";

	numInputs = 0;
	numOutputs = 0;
	tableHeight = 0;

	file.open(filename, ios::in);

	while (!file.eof()) { 
		getline(file,line);
		if (yIndex == 0) {
			outIndex = line.find("Outputs");
			for (int i = 0; i < line.length(); i++) {
				if (line.at(i) == ',') {
					if (i < outIndex) {
						numInputs++;
					} else {
						numOutputs++;
					}
				}
			}
			numOutputs++;
		} else if (yIndex > 2) {
			tableHeight++;
		}
		yIndex++;
	}

	file.close();

	inputs = new string[numInputs];
	outputs = new string[numOutputs];
	inputTable = new char*[tableHeight];
	outputTable = new char*[tableHeight];

	for (int i = 0; i < tableHeight; i++) {
		inputTable[i] = new char[numInputs];
		outputTable[i] = new char[numOutputs];
	}

	file.open(filename, ios::in);

	yIndex = 0;
	while (!file.eof()) {
		xIndex = 0;
		inpu = true;
		getline(file,line);
		if (yIndex == 0 && !vals) {
			for (int i = 0; i < line.length(); i++) {
				if (line.at(i) == ',') {
					xIndex++;
				} else {
					if (i == outIndex) {
						outIndex = xIndex;
					}
				}
			}
		} else if (yIndex == 1 && !vals) {
			for (int i = 0; i < line.length(); i++) {
				if (line.at(i) == ',' || i == line.length() - 1) {
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
					var += line.at(i);
				}
				if (i == line.length() - 1) {
					outputs[numOutputs - 1] += line.at(i);
				}
			}
		} else {
			if (!vals) {
				vals = true;
				yIndex = 0;
			}
			for (int i = 0; i < line.length(); i++) {
				if (line.at(i) != ',') {
					if (xIndex < outIndex && inpu) {
						inputTable[yIndex][xIndex] = line.at(i);
					} else {
						if (inpu) {
							inpu = false;
							xIndex = 0;
						}
						outputTable[yIndex][xIndex] = line.at(i);
					}
					xIndex++;
				}
			}
		}
		yIndex++;
	}
}
truth_table::~truth_table() {
	delete[] inputs;
	delete[] outputs;
	int max = (numInputs >= numOutputs)?(numInputs):(numOutputs);
	for (int i = 0; i < max; i++) {
		if (i < numInputs) {
			delete[] inputTable[i];
		}
		if (i < numOutputs) {
			delete[] outputTable[i];
		}
	}
	delete[] inputTable;
	delete[] outputTable;
}

linked_list<char*>* truth_table::get_cases_where(string var, char val) {
	linked_list<char*>* l = new linked_list<char*>();
	int index = indexOfOutput(var);
	for (int i = 0; i < tableHeight; i++) {
		if (outputTable[i][index] == val) {
			l->append(inputTable[i]);
		}
	}
	return l;
}

int truth_table::indexOfOutput(string var) {
	for (int i = 0; i < numOutputs; i++) {
		if (var.compare(outputs[i]) == 0) {
			return i;
		}
	}
	return -1;
}

string truth_table::to_string() {
	string result = "";
	char* blob;
	int tab_width;

	int len = find_longest_length_str(inputs,numInputs,0);
	len = find_longest_length_str(outputs,numOutputs,len) + 1;

	for (int i = 0; i < numInputs; i++) {
		result += get_block(inputs[i],len);
	}
	result += "|";
	for (int i = 0; i < numOutputs; i++) {
		result += get_block(outputs[i],len);
	}
	result += "\n";

	tab_width = numInputs * len;
	for (int i = 0; i < tab_width; i++) {
		result += '-';
	}
	tab_width = numOutputs * len;
	result += '|';
	for (int i = 0; i < tab_width; i++) {
		result += '-';
	}
	result += "\n";
	
	for (int i = 0; i < tableHeight; i++) {
		for (int j = 0; j < numInputs; j++) {
			result += get_block(inputTable[i][j],len);
		}
		result += '|';
		for (int j = 0; j < numOutputs; j++) {
			result += get_block(outputTable[i][j],len);
		}
		result += "\n";
	}

	return result;
}

int find_longest_length_str(string* list, int length, int curr_largest) {
	int longest = curr_largest;
	for (int i = 0; i < length; i++) {
		if (list[i].length() > longest) {
			longest = list[i].length();
		}
	}
	return longest;
}

string get_block(string str, int width) {
	string result = "";
	if (width < str.length()) {
		width = str.length();
	}
	for (int i = 0; i < width; i++) {
		if (i < str.length()) {
			result += str.at(i);
		} else {
			result += " ";
		}
	}
	return result;
}
string get_block(char c, int width) {
	string result = "";
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
log* convert_to_log(linked_list<char*>* l, truth_table& t) {
	log* result;
	log* temp1 = nullptr;
	log* temp2 = nullptr;
	log* temp3 = nullptr;

	switch (l->length) {
		case 0: // list is empty -> result = LO
			result = new log();
			result->val = LO;
			result->type = 1;
			break;
		case 1: // list has only one element -> result = HI || result = "var" || result = NOT("var") || result = AND(...)
			switch (count_valid(l->get(0),t.numInputs)) {
				case 0: // No inputs specifiied -> result = HI
					result = new log();
					result->val = HI;
					result->type = 1;
					break;
				case 1: // Only one input is specified -> result = "var" || temp1 = "var", result = NOT(temp1)
					for (int i = 0; i < t.numInputs; i++) {
						if (l->get(0)[i] != DC) { // Input at i is set -> add this input
							result = new log();
							if (l->get(0)[i] == HI) { // Input at i is HI -> result = "var"
								result->var_name = t.inputs[i];
								result->type = 2;
							} else { // input at i is LO -> temp1 = "var", result = NOT(temp1)
								result->gate = NOT;
								temp1 = new log();
								temp1->var_name = t.inputs[i];
								temp1->type = 2;
								result->args.append(temp1);
								result->type = 3;
								delete temp1;
							}
							break;
						}
					}
					break;
				default: // Many inputs are specified -> result = AND(...)
					result = new log();
					result->type = 3;
					result->gate = AND;
					for (int i = 0; i < t.numInputs; i++) {
						if (l->get(0)[i] != DC) { // Input i is set -> add this input
							if (l->get(0)[i] == HI) { // Input i is set HI -> temp1 = "var", result &= temp1
								temp1 = new log();
								temp1->type = 2;
								temp1->var_name = t.inputs[i];
								result->args.append(temp1);
								delete temp1;
							} else { // Input is set LO -> temp2 = "var", temp1 = NOT(temp2), result &= temp1
								temp1 = new log();
								temp1->gate = NOT;
								temp1->type = 3;
								temp2 = new log();
								temp2->type = 2;
								temp2->var_name = t.inputs[i];
								temp1->args.append(temp2);
								result->args.append(temp1);
								delete temp1;
								delete temp2;
							}
						}
					}
					break;
			}
			break;
		default: // There are many cases when output is on -> result = OR(...)
			result = new log();
			result->gate = OR;
			result->type = 3;
			for (int i = 0; i < l->length; i++) {
				switch (count_valid(l->get(i),t.numInputs)) {
					case 0: // current case has no inputs set -> temp1 = HI, result |= temp1
						temp1 = new log();
						temp1->val = HI;
						temp1->type = 1;
						result->args.append(temp1);
						delete temp1;
						break;
					case 1: // current case has only one input set -> temp1 = "var", result |= temp1 || temp2 = "var", temp1 = NOT(temp2), result |= temp1
						for (int j = 0; j < t.numInputs; j++) {
							if (l->get(i)[j] != DC) { // Input j is set -> add this input
								if (l->get(i)[j] == HI) { // Input j is set HI -> temp1 = "var", result &= temp1
									temp1 = new log();
									temp1->type = 2;
									temp1->var_name = t.inputs[j];
									result->args.append(temp1);
									delete temp1;
								} else { // Input j is set LO -> temp2 = "var", temp1 = NOT(temp1), result &= temp1
									temp1 = new log();
									temp1->gate = NOT;
									temp1->type = 3;
									temp2 = new log();
									temp2->type = 2;
									temp2->var_name = t.inputs[j];
									temp1->args.append(temp2);
									result->args.append(temp1);
									delete temp1;
									delete temp2;
								}
								break;
							}
						}
						break;
					default: // current case has many inputs set -> temp1 = AND(...), result |= temp1
						temp1 = new log();
						temp1->type = 3;
						temp1->gate = AND;
						for (int j = 0; j < t.numInputs; j++) {
							if (l->get(i)[j] != DC) { // Input j is set -> add this input
								if (l->get(i)[j] == HI) { // Input j is set HI -> temp2 = "var", temp1 &= temp2
									temp2 = new log();
									temp2->type = 2;
									temp2->var_name = t.inputs[j];
									temp1->args.append(temp2);
									delete temp2;
								} else { // Input j is set LO -> temp3 = "var", temp2 = NOT(temp3), temp1 &= temp2
									temp2 = new log();
									temp2->gate = NOT;
									temp2->type = 3;
									temp3 = new log();
									temp3->type = 2;
									temp3->var_name = t.inputs[j];
									temp2->args.append(temp3);
									temp1->args.append(temp2);
									delete temp2;
									delete temp3;
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

bool find_same_except(linked_list<char*>* l, int num_i) {
	int index;
	for (int i = 0; i < l->length; i++) {
		for (int j = i + 1; j < l->length; j++) {
			index = sameExcept(l->get(i),l->get(j),num_i);
			if (index != -1) {
				l->get(i)[index] = DC;
				l->remove(j);
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

void reduce(linked_list<char*>* l,int len) {
	while (find_same_except(l,len) || find_contains_except(l,len));
}
