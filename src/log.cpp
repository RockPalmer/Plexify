/*
 * Default constructor
 */
log::log() {
	gate = NONE;
	val = DC;
	type = LITERAL;
	var_name = "";
}
/*
 * This constructor is used to construct a log of type EXPRESSION, where the logical
 * expression consists of arguments and an operand (gate) or of type LITERAL, where 
 * the logical expression consists of 1 boolean literal.
 * 
 * In the case of type EXPRESSION, <c> should be replaced with the desired gate 
 * {NOT,OR,NOR,AND,NAND,XOR,XNOR} and <t> should be replaced with EXPRESSION.
 *
 * In the case of type LITERAL, <c> chould be replaced with the desired boolean value
 * {HI,LO} and <t> should be replaced with LITERAL.
 */
log::log(char c, log_type t) {
	type = t;
	var_name = "";
	switch (t) {
		case LITERAL:
			gate = NONE;
			val = c;
			break;
		case EXPRESSION:
			gate = c;
			val = DC;
			break;
	}
}
/*
 * This constructore is used to construct a log of type VARIABLE, where the logical
 * expression consists of 1 boolean variable.
 * 
 * In this case, <v> should be replaced with the name of the variable as a string.
 */
log::log(std::string v) {
	gate = NONE;
	val = DC;
	type = VARIABLE;
	var_name = v;
}
log::~log() {}
/*
 * Returns a new log instance <l> such that <l> = ~(<this>)
 */
log* log::invert() {
	log* r = new log();
	log* s = nullptr;
	std::string temp = "";
	r->copy(this);
	switch (r->type) {

		// If it is of type LITERAL, invert the boolean value
		case LITERAL:
			if (r->val == HI) {
				r->val = LO;
			} else if (r->val == LO) {
				r->val = HI;
			}
			break;

		// If it is of type VARIABLE with variable name "a", return ~a
		case VARIABLE:
			temp = r->var_name;
			delete r;
			r = new log(NOT,EXPRESSION);
			s = new log(temp);
			r->args.append(s);
			delete s;
			break;
		case EXPRESSION:

			// If the gate is a NOT, remove the gate
			if (r->gate == NOT) {
				temp = r->args.head->data->var_name;
				delete r;
				r = new log(temp);

			// If th gate is not a NOT and is not a MUX, invert the gate and simplify the expression
			} else {
				if (r->gate != MUX) {
					r->gate ^= 1;
					r->demorgify();
				}
			}
			break;
	}
	return r;
}
/*
 * Returns a string representation of the logical expression
 */
std::string log::to_string() {
	std::string result = "";
	std::string op_str = "";
	log* temp = nullptr;
	switch (type) {
		// If it is of type LITERAL, return the string_mapping of HI or LO
		case LITERAL:
			if (val == HI) {
				result = string_mapping[HI_MAP];
			} else {
				result = string_mapping[LO_MAP];
			}
			break;
		// If it is of type VARIABLE, return the name of the variable as a string 
		case VARIABLE:
			result = var_name;
			break;
		// If it is of type EXPRESSION, either return the string_mapping of NOT
		// followed by the string representation of the argument if the gate is NOT
		// or, return a the string representations of each argument delimited
		// by the string_mapping of the designated gate.
		case EXPRESSION:
			if (gate == NOT) {
				temp = args.head->data;
				if (temp->type == 3) {
					result = string_mapping[NOT_MAP] + string_mapping[GROUP_OPEN_MAP] + temp->to_string() + string_mapping[GROUP_CLOSE_MAP];
				} else {
					result = string_mapping[NOT_MAP] + temp->to_string();
				}
			} else {

				// Grab the string mapping of the base gate
				switch (gate & ~1) {

					// AND or NAND
					case AND:
						op_str = string_mapping[AND_MAP];
						break;

					// OR or NOR
					case OR:
						op_str = string_mapping[OR_MAP];
						break;

					// XOR or XNOR
					case XOR:
						op_str = string_mapping[XOR_MAP];
						break;
				}
				ptr_node<log>* n = args.head;
				for (size_t i = 0; i < args.length; i++) {
					temp = n->data;
					if (i != 0) {
						result = result + op_str;
					}
					if (temp->type == EXPRESSION) {
						if (n->data->polarity()) {
							result = result + string_mapping[GROUP_OPEN_MAP] + temp->to_string() + string_mapping[GROUP_CLOSE_MAP];
						} else {
							result = result + temp->to_string();
						}
					} else {
						result = result + temp->to_string();
					}
					n = n->next;
				}
				// If the gate is inverted from its base gate (i.e. NAND,NOR,XNOR) add a NOT at the beginning
				// of the string
				if (gate & 1) {
					result = string_mapping[NOT_MAP] + string_mapping[GROUP_OPEN_MAP] + result + string_mapping[GROUP_CLOSE_MAP];
				}
			}
			break;
	}
	return result;
}
/*
 * Checks if this log is equal to <l>
 */
bool log::equal_to(log* l) {
	if (type != l->type) {
		return false;
	}

	ptr_node<log>* n;
	bool* temp1;
	size_t temp2;
	switch (type) {

		// If they are both of type LITERAL, compare their boolean values
		case LITERAL:
			return (val == l->val);
			break;

		// If they are both of type VARIABLE, compare their names
		case VARIABLE:
			return (var_name.compare(l->var_name) == 0);
			break;

		// If they are both of type EXPRESSION, compare their gates and arguments
		case EXPRESSION:
			if (gate != l->gate) {
				return false;
			}

			// If they don't have the same number of arguments, then we don't need
			// to compare the arguments
			if (args.length != l->args.length) {
				return false;
			}

			// Compare their arguments
			n = args.head;
			while (n != nullptr) {
				if (!l->contains(n->data,temp1,temp2)) {
					delete[] temp1;
					return false;
				}
				n = n->next;
			}
			return true;
			break;
	}
	return false;
}
/*
 * This function checks if <l> is contained in <this>. The parameters <elements> and <len>
 * are included for the case in which, <l> is not 1 singular argument of <this> but rather,
 * involves several members of <this>.
 *
 * For example: <this> = (a & b & c & d), <l> = (c & d)
 * 
 * In this example, we know that <this> contains <l> because it contains c & d. However,
 * if we were to just iterate through each member of <this> we would get the following behavior:
 * 
 * a != (c & d)
 * b != (c & d)
 * c != (c & d)
 * d != (c & d)
 * return false;
 * 
 * Because of this, in this scenario, <len> = <this>.args.length and <element> = new bool[len].
 * Then, each index of <elements> corresponds to a node in <this>.args. if <elements>[i] is true
 * for some int i, then the corresponding node in the arguments if <this> is equal to one of the
 * arguments of <l>
 * 
 * However, in the case of <l> existing as one of the members of <this>, <elements> = new bool[1]
 * where <elements>[0] = true and <len> = 1
 */
bool log::contains(log* l, bool*& elements,size_t& len) {
	len = -1;
	if (type == EXPRESSION) {
		// Check if <l> exists as many members of <this>
		if (l->type == EXPRESSION && l->gate == gate && l->args.length < args.length && gate >= AND && gate <= XNOR) {
			elements = new bool[args.length];
			len = args.length;

			for (size_t i = 0; i < len; i++) {
				elements[i] = false;
			}

			ptr_node<log>* n = l->args.head;
			for (size_t i = 0; i < l->args.length; i++) {
				ptr_node<log>* m = args.head;
				for (size_t j = 0; j < args.length && !elements[i]; j++) {
					if (!elements[i]) {
						if (m->data->equal_to(n->data)) {
							elements[i] = true;
						}
					}
					m = m->next;
				}

				// If 1 argument of <l> is not found, then <this> does not contain <l>
				if (!elements[i]) {
					delete[] elements;
					elements = nullptr;
					len = -1;
					break;
				}
				n = n->next;
			}

		// Check if <l> exists as just 1 member of <this>
		} else {
			ptr_node<log>* n = args.head;
			elements = new bool[1];
			elements[0] = false;
			len = 1;
			while (n != nullptr) {
				if (n->data->equal_to(l)) {
					elements[0] = true;
					break;
				}
				n = n->next;
			}

			if (!elements[0]) {
				delete[] elements;
				elements = nullptr;
				len = -1;
			}
		}
	}
	return elements != nullptr;
}
/*
 * This function simplifies the logical expression by reducing the total number of inverted
 * gates in the expression. It returns the polarity() of the resulting logical expression.
 */
bool log::demorgify() {
	size_t num_hi = 0;
	size_t num_lo = 0;
	if (type == EXPRESSION && gate != MUX) {

		// Count the total number of args that are "HI" and "LO"
		ptr_node<log>* n = args.head;
		for (size_t i = 0; i < args.length; i++) {
			switch (n->data->type) {
				case LITERAL:
					if (n->data->val == HI) {
						num_hi++;
					} else if (n->data->val == LO) {
						num_lo++;
					}
					break;
				case VARIABLE:
					num_hi++;
					break;
				case EXPRESSION:
					if (n->data->gate == NOT) {
						num_lo++;
					} else {
						if (n->data->demorgify()) {
							num_hi++;
						} else {
							num_lo++;
						}
					}
					break;
			}
			n = n->next;
		}
		if (gate & 1) {
			num_lo++;	
		} else {
			num_hi++;
		}

		// For expressions where the base gate is XOR (i.e. XOR and XNOR) we follow
		// the following principle that we know about XOR gates:
		//
		// If the total number of inverted gates among the arguments plus the parent gate
		// itself is odd, this can be rewritten to an equivalent statement where none of 
		// the arguments have inverted gates but the parent gate is inverted.
		//
		// Ex: (a ^ b ^ ~c) <=>
		//     (a ^ ~b ^ c) <=>
		//     (~a ^ b ^ c) <=>
		//     (~a ^ ~b ^ ~c) <=> 
		//     ~(a ^ ~b ^ ~c) <=>
		//     ~(~a ^ b ^ ~c) <=>
		//     ~(~a ^ ~b ^ c) <=> ~(a ^ b ^ c)
		//
		// If the total number of inverted gates among the arguments plus the parent gate
		// itself is even, this can be rewritten to an equivalent statement where none of 
		// the arguments have inverted gates and the parent gate is not inverted.
		//
		// Ex: (a ^ ~b ^ ~c) <=>
		//     (~a ^ b ^ ~c) <=>
		//     (~a ^ ~b ^ c) <=>
		//     ~(a ^ b ^ ~c) <=> 
		//     ~(a ^ ~b ^ c) <=>
		//     ~(~a ^ b ^ c) <=>
		//     ~(~a ^ ~b ^ ~c) <=> (a ^ b ^ c)
		//
		// If any of the arguments are inverted, then we use the simplification rules above
		if (gate == XOR || gate == XNOR) {
			n = args.head;
			std::string name = "";
			while (n != nullptr) {
				switch (n->data->type) {
					case LITERAL:
						if (n->data->val == LO) {
							n->data->val = HI;
						}
						break;
					case EXPRESSION:
						if (n->data->gate == NOT) {
							name = n->data->args.head->data->var_name;
							delete n->data;
							n->data = new log(name);
						} else {
							if (n->data->gate & 1) {
								n->data->gate ^= 1;
							}
						}
						break;
				}
				n = n->next;
			}

			if (num_lo & 1) {
				gate = XNOR;
				return false;
			} else {
				gate = XOR;
				return true;
			}

		// For expressions where the base gate is OR or AND, we use DeMorgan's rules
		// to simplify the expression only if the resulting number of inverted gates after
		// the simplification will be less than or equal to the original number of inverted
		// gates
		} else if (gate == OR || gate == NOR || gate == AND || gate == NAND) {
			if (num_lo >= num_hi) {
				n = args.head;
				std::string name = "";
				for (size_t i = 0; i < args.length; i++) {
					switch (n->data->type) {
						case LITERAL:
							if (n->data->val == HI) {
								n->data->val = LO;
							} else if (n->data->val == LO) {
								n->data->val = HI;
							}
							break;
						case VARIABLE:
							name = n->data->var_name;
							delete n->data;
							n->data = new log(NOT,EXPRESSION);
							n->data->args.append(new log(name),false);
							break;
						case EXPRESSION:
							if (n->data->gate == NOT) {
								name = n->data->args.head->data->var_name;
								delete n->data;
								n->data = new log(name);
							} else {
								if (n->data->gate & 1) {
									n->data->gate ^= 1;
								}
							}
							break;
					}
					n = n->next;
				}

				switch (gate) {
					case OR:
						gate = NAND;
						break;
					case NOR:
						gate = AND;
						break;
					case AND:
						gate = NOR;
						break;
					case NAND:
						gate = OR;
						break;
				}
			}

			if (gate & 1) {
				return false;
			} else {
				return true;
			}
		}

	// If this log is not of type EXPRESSION, do nothing
	} else if (type == VARIABLE) {
		return true;
	} else if (type == LITERAL) {
		if (val == HI) {
			return true;
		} else if (val == LO) {
			return false;
		}
	}
	return false;
}
/*
 * Returns the "inverted" status of the expression.
 * This means that if it is of type EXPRESSION, return false if the gate is an "inverting" gate
 * (i.e. NOT,NAND,NOR,XNOR) and return true if it is not an inverting gate.
 * If it is of type VARIABLE, return true, as the variable is not inverted.
 * If it is of type LITERAL, return the boolean value of the literal.
 */
bool log::polarity() {
	bool result = false;
	switch (type) {
		case LITERAL:
			result = (val == HI);
			break;
		case VARIABLE:
			result = true;
			break;
		case EXPRESSION:
			if (gate == MUX) {
				result = true;
			} else {
				result = !(gate & 1);
			}
			break;
	}
	return result;
}
/*
 * Performs the same operation as invert() but does not return a new log. It instead
 * applies the operation to the object itself. (i.e. <this> = ~(<this>) )
 */
void log::invert_this() {
	log* s = nullptr;
	std::string temp = "";
	switch (type) {
		case LITERAL:
			if (val == HI) {
				val = LO;
			} else if (val == LO) {
				val = HI;
			}
			break;
		case VARIABLE:
			args.append(new log(var_name),false);
			type = EXPRESSION;
			gate = NOT;
			var_name = "";
			break;
		case EXPRESSION:
			if (gate == NOT) {
				gate = args.head->data->gate;
				val = args.head->data->val;
				type = args.head->data->type;
				var_name = args.head->data->var_name;
				if (args.head->data->args.length > 0) {
					ptr_node<log>* n = args.head->data->args.head;
					for (size_t i = 0; i < args.head->data->args.length; i++) {
						args.head->data->args.pop(n);
						args.append(n);
					}
					args.remove(0);
					demorgify();
				}
			} else {
				if (gate != MUX) {
					gate ^= 1;
					demorgify();
				}
			}
			break;
	}
}
/*
 * Copies the data from <l> to <this>
 */
void log::copy(log* l) {
	gate = l->gate;
	val = l->val;
	type = l->type;
	var_name = l->var_name;
	args.copy(&(l->args));
}
