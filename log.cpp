log::log() {
	gate = NONE;
	val = DC;
	type = 0;
	var_name = "";
}

log::log(char c, char t) {
	type = t;
	var_name = "";
	switch (t) {
		case 1:
			gate = NONE;
			val = c;
			break;
		case 3:
			gate = c;
			val = DC;
			break;
	}
}

log::log(std::string v) {
	gate = NONE;
	val = DC;
	type = 2;
	var_name = v;
}

log::~log() {}

std::string log::to_string() {
	std::string result = "";
	std::string op_str = "";
	log* temp = nullptr;
	switch (type) {
		case 1:
			if (val == HI) {
				result = "1";
			} else {
				result = "0";
			}
			break;
		case 2:
			result = var_name;
			break;
		case 3:
			if (gate == NOT) {
				temp = args.head->data;
				if (temp->type == 3) {
					result = "~(" + temp->to_string() + ")";
				} else {
					result = "~" + temp->to_string();
				}
			} else {
				switch (gate & ~1) {
					case AND:
						op_str = " & ";
						break;
					case OR:
						op_str = " | ";
						break;
					case XOR:
						op_str = " ^ ";
						break;
				}
				ptr_node<log>* n = args.head;
				for (int i = 0; i < args.length; i++) {
					temp = n->data;
					if (i != 0) {
						result = result + op_str;
					}
					if (temp->type == 3) {
						if (n->data->polarity()) {
							result = result + "(" + temp->to_string() + ")";
						} else {
							result = result + temp->to_string();
						}
					} else {
						result = result + temp->to_string();
					}
					n = n->next;
				}
				if (gate & 1) {
					result = "~(" + result + ")";
				}
			}
			break;
	}
	return result;
}

bool log::equal_to(log* l) {
	if (type == l->type) {
		ptr_node<log>* n;
		bool* temp1;
		int temp2;
		switch (type) {
			case 1:
				return (val == l->val);
				break;
			case 2:
				return (var_name.compare(l->var_name) == 0);
				break;
			case 3:
				if (gate == l->gate) {
					if (args.length == l->args.length) {
						n = args.head;
						while (n != nullptr) {
							if (!l->contains(n->data,temp1,temp2)) {
								delete[] temp1;
								return false;
							}
							n = n->next;
						}
						return true;
					}
					return false;
				}
				return false;
				break;
		}
	}
	return false;
}

log* log::invert() {
	log* r = new log();
	log* s = nullptr;
	std::string temp = "";
	r->copy(this);
	switch (r->type) {
		case 1:
			if (r->val == HI) {
				r->val = LO;
			} else if (r->val == LO) {
				r->val = HI;
			}
			break;
		case 2:
			temp = r->var_name;
			delete r;
			r = new log(NOT,3);
			s = new log(temp);
			r->args.append(s);
			delete s;
			break;
		case 3:
			if (r->gate == NOT) {
				temp = r->args.head->data->var_name;
				delete r;
				r = new log(temp);
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

bool log::polarity() {
	bool result = false;
	switch (type) {
		case 1:
			result = (val == HI);
			break;
		case 2:
			result = true;
			break;
		case 3:
			if (gate == MUX) {
				result = true;
			} else {
				result = !(gate & 1);
			}
			break;
	}
	return result;
}

ptr_node<log>* log::strict_contains(log* l) {
	ptr_node<log>* result = nullptr;

	if (type == 3) {
		ptr_node<log>* n = args.head;
		while (n != nullptr && !result) {
			if (n->data->equal_to(l)) {
				result = n;
			} else {
				n = n->next;
			}
		}
	}
	return result;
}

bool log::contains(log* l, bool*& elements,int& len) {
	len = -1;
	if (type == 3) {
		if (l->type == 3 && l->gate == gate && l->args.length < args.length && gate >= AND && gate <= XNOR) {
			elements = new bool[args.length];
			len = args.length;

			for (int i = 0; i < len; i++) {
				elements[i] = false;
			}

			ptr_node<log>* n = l->args.head;
			for (int i = 0; i < l->args.length; i++) {
				ptr_node<log>* m = args.head;
				for (int j = 0; j < args.length && !elements[i]; j++) {
					if (!elements[i]) {
						if (m->data->equal_to(n->data)) {
							elements[i] = true;
						}
					}
					m = m->next;
				}

				if (!elements[i]) {
					delete[] elements;
					elements = nullptr;
					len = -1;
					break;
				}
				n = n->next;
			}
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

bool log::contains_same_elements(log* l) {
	bool result = false;
	int len = args.length;
	if (len == l->args.length) {
		result = true;
		ptr_node<log>* found_node = nullptr;
		ptr_node<log>* n = args.head;
		ptr_node<log>* m = nullptr;
		log* p = nullptr;

		for (int i = 0; i < len; i++) {
			p = n->data;
			found_node = l->strict_contains(p);
			if (!found_node) {
				p = p->invert();
				found_node = l->strict_contains(p);
				if (!found_node) {
					result = false;
					break;
				}
				delete p;
			}
			n = n->next;
		}

		if (result) {
			n = args.head;
			for (int i = 0; i < len; i++) {
				p = n->data;
				found_node = l->strict_contains(p);
				if (found_node) {
					l->args.pop(found_node);
					l->args.insert(i,found_node->data);
					delete found_node;
				} else {
					p = p->invert();
					found_node = l->strict_contains(p);
					if (found_node) {
						l->args.pop(found_node);
						l->args.insert(i,found_node->data);
						delete found_node;
					} else {
						result = false;
						break;
					}
					delete p;
				}
				n = n->next;
			}
		}
	}
	return result;
}

void log::copy(log* l) {
	gate = l->gate;
	val = l->val;
	type = l->type;
	var_name = l->var_name;
	args.copy(&(l->args));
}

bool log::demorgify() {
	int num_hi = 0;
	int num_lo = 0;
	if (type == 3 && gate != MUX) {
		ptr_node<log>* n = args.head;
		while (n != nullptr) {
			switch (n->data->type) {
				case 1:
					if (n->data->val == HI) {
						num_hi++;
					} else if (n->data->val == LO) {
						num_lo++;
					}
					break;
				case 2:
					num_hi++;
					break;
				case 3:
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

		if (gate == XOR || gate == XNOR) {
			n = args.head;
			std::string name = "";
			while (n != nullptr) {
				switch (n->data->type) {
					case 1:
						if (n->data->val == LO) {
							n->data->val = HI;
						}
						break;
					case 3:
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
		} else if (gate == OR || gate == NOR || gate == AND || gate == NAND) {

			if (num_lo >= num_hi) {
				n = args.head;
				std::string name = "";
				while (n != nullptr) {
					switch (n->data->type) {
						case 1:
							if (n->data->val == HI) {
								n->data->val = LO;
							} else if (n->data->val == LO) {
								n->data->val = HI;
							}
							break;
						case 2:
							name = n->data->var_name;
							delete n->data;
							n->data = new log(NOT,3);
							n->data->args.append(new log(name));
							break;
						case 3:
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
	} else if (type == 2) {
		return true;
	} else if (type == 1) {
		if (val == HI) {
			return true;
		} else if (val == LO) {
			return false;
		}
	}
	return false;
}

bool log::xorify() {
	if (gate == OR || gate == NOR) {
		ptr_node<log>* n = args.head;

		int len = args.length;
		bool ands[len];

		for (int i = 0; i < len; i++) {
			if (n->data->type == 3 && n->data->gate == AND) {
				ands[i] = true;
			} else {
				ands[i] = false;
			}
			n = n->next;
		}

		bool same_elements[2][len];

		n = args.head;
		ptr_node<log>* m;
		for (int i = 0; i < len; i++) {
			if (ands[i]) {
				m = n->next;
				for (int j = i + 1; j < len; j++) {
					if (ands[j]) {

					}
				}
			}
		}
	} else {
		return false;
	}
	return true;
}