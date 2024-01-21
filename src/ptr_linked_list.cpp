template <class T> ptr_node<T>::ptr_node() {
	prev = nullptr;
	next = nullptr;
	data = nullptr;
}
template <class T> ptr_node<T>::ptr_node(T* element, bool copy_vals) {
	prev = nullptr;
	next = nullptr;
	if (copy_vals) {
		data = new T();
		data->copy(element);
	} else {
		data = element;
	}
}
template <class T> ptr_node<T>::~ptr_node() {
	if (data != nullptr) {
		delete data;
	}
}
template <class T> void ptr_node<T>::copy(ptr_node<T>* n) {
	if (data == nullptr) {
		data = new T();
	}
	data->copy(n->data);
}
template <class T> std::string ptr_node<T>::to_string() {
	return data->to_string();
}

template <class T> ptr_linked_list<T>::ptr_linked_list() {
	head = nullptr;
	tail = nullptr;
	length = 0;
}
template <class T> ptr_linked_list<T>::~ptr_linked_list() {
	ptr_node<T>* temp;
	while (head != nullptr) {
		temp = head;
		head = head->next;
		delete temp;
	}
}
template <class T> std::string ptr_linked_list<T>::to_string() {
	std::string result = "[";
	ptr_node<T>* temp = head;
	switch (length) {
		case 0:
			result = result + "EMPTY]";
			break;
		case 1:
			result = result + head->to_string() + "]";
			break;
		default:
			result = result + head->to_string();
			temp = temp->next;
			for (size_t i = 1; i < length; i++) {
				result = result + "," + temp->to_string();
				temp = temp->next;
			}
			result = result + "]";
			break;
	}
	return result;
}
template <class T> std::string ptr_linked_list<T>::print_full() {
	std::string result = "";
	if (length) {
		int max_len = -1;
		std::string temp = "";
		ptr_node<T>* n = head;
		while (n) {
			temp = n->data->to_string();
			if (max_len < (int)(temp.length())) {
				max_len = (int)(temp.length());
			}
			n = n->next;
		}
		n = head;
		int len = -1;
		while (n) {
			result += '\n';
			if (n->prev) {
				temp = n->prev->data->to_string();
			} else {
				temp = "NULL";
			}
			len = temp.length();
			result += '[';
			for (int i = 0; i < max_len - len; i++) {
				result += ' ';
			}
			result = result + temp;
			result = result + "] <- [";
			len = n->data->to_string().length();
			for (int i = 0; i < max_len - len; i++) {
				result += ' ';
			}
			result = result + n->data->to_string() + "] -> [";
			if (n->next) {
				temp = n->next->data->to_string();
			} else {
				temp = "NULL";
			}
			len = temp.length();
			result = result + temp;
			for (int i = 0; i < max_len - len; i++) {
				result += ' ';
			}
			result = result + "]\n";
			n = n->next;
		}
	} else {
		result = "\n[NULL]\n";
	}
	return result;
}
template <class T> void ptr_linked_list<T>::set(size_t index, T* element) {
	if (index < length) {
		size_t start = length - index;
		ptr_node<T>* temp;
		if (index < start) {
			// Start from head and move forward
			temp = head;
			for (size_t i = 0; i < index; i++) {
				temp = temp->next;
			}
		} else {
			// Start from tail and move backward
			temp = tail;
			for (size_t i = length - 1; i > index; i--) {
				temp = temp->prev;
			}
		}
		if (temp->data != nullptr) {
			delete temp->data;
			temp->data = new T();
		}
		//temp->data->copy(element);
		temp->data = element;
	}
}
template <class T> void ptr_linked_list<T>::insert(size_t index, T* element, bool copy_vals) {
	if (index <= length) {
		ptr_node<T>* temp1;
		ptr_node<T>* temp2;
		size_t start;
		if (head == nullptr) {
			head = new ptr_node<T>(element,copy_vals);
			tail = head;
		} else if (index == 0) {
			temp1 = new ptr_node<T>(element,copy_vals);
			temp1->next = head;
			head->prev = temp1;
			head = temp1;
		} else if (index == length) {
			temp1 = new ptr_node<T>(element,copy_vals);
			temp1->prev = tail;
			tail->next = temp1;
			tail = temp1;
		} else {
			start = length - index;
			temp2 = new ptr_node<T>(element,copy_vals);
			if (index < start) {
				// Start from head and work forward
				temp1 = head;
				for (size_t i = 0; i < index; i++) {
					temp1 = temp1->next;
				}
				temp2->next = temp1;
				temp2->prev = temp1->prev;
				temp1->prev = temp2;
				temp2->prev->next = temp2;
			} else {
				// Start from tail and work backward
				temp1 = tail;
				for (size_t i = length - 1; i > index; i--) {
					temp1 = temp1->prev;
				}
				temp2->prev = temp1->prev;
				temp2->next = temp1;
				temp1->prev = temp2;
				temp2->prev->next = temp2;
			}
		}
		length++;
	}
}
template <class T> void ptr_linked_list<T>::remove(size_t index) {
	if (index < length) {
		ptr_node<T>* temp;
		size_t start;
		if (head == tail && head != nullptr) {
			delete head;
			head = nullptr;
			tail = head;
		} else {
			if (index == 0) {
				temp = head;
				head = head->next;
				head->prev = nullptr;
			} else if (index == length - 1) {
				temp = tail;
				tail = tail->prev;
				tail->next = nullptr;
			} else {
				start = length - index;
				if (index < start) {
					// Start at head and work forward
					temp = head;
					for (size_t i = 0; i < index; i++) {
						temp = temp->next;
					}
				} else {
					// Start at tail and work backward
					temp = tail;
					for (size_t i = length - 1; i > index; i--) {
						temp = temp->prev;
					}
				}
				temp->next->prev = temp->prev;
				temp->prev->next = temp->next;
			}
			delete temp;
		}
		length--;
	}
}
template <class T> void ptr_linked_list<T>::pop(ptr_node<T>* n) {
	if (length) {
		if (n->next) {
			n->next->prev = n->prev;
		} else {
			tail = n->prev;
		}
		if (n->prev) {
			n->prev->next = n->next;
		} else {
			head = n->next;	
		}
	} else {
		head = nullptr;
		tail = head;
	}
	length--;
}
template <class T> void ptr_linked_list<T>::append(T* element, bool copy_vals) {
	if (head == nullptr) {
		head = new ptr_node<T>(element,copy_vals);
		tail = head;
	} else {
		tail->next = new ptr_node<T>(element,copy_vals);
		tail->next->prev = tail;
		tail = tail->next;
	}
	length++;
}
template <class T> void ptr_linked_list<T>::append(ptr_node<T>* n) {
	if (head == nullptr) {
		head = n;
		n->next = nullptr;
		n->prev = nullptr;
		tail = head;
	} else {
		tail->next = n;
		n->next = nullptr;
		n->prev = tail;
		tail = tail->next;
	}
	length++;
}
template <class T> void ptr_linked_list<T>::prepend(T* element, bool copy_vals) {
	if (head == nullptr) {
		head = new ptr_node<T>(element,copy_vals);
		tail = head;
	} else {
		head->prev = new ptr_node<T>(element,copy_vals);
		head->prev->next = head;
		head = head->prev;
	}
	length++;
}
template <class T> void ptr_linked_list<T>::prepend(ptr_node<T>* n) {
	if (head == nullptr) {
		head = n;
		n->next = nullptr;
		n->prev = nullptr;
		tail = head;
	} else {
		head->prev = n;
		n->next = head;
		n->prev = nullptr;
		head = head->prev;
	}
	length++;
}
template <class T> T* ptr_linked_list<T>::get(size_t index) {
	size_t start = length - index;
	ptr_node<T>* temp;
	if (index < length) {
		if (index < start) {
			// Start from head and move forward
			temp = head;
			for (size_t i = 0; i < index; i++) {
				temp = temp->next;
			}
		} else {
			// Start from tail and move backward
			temp = tail;
			for (size_t i = length - 1; i > index; i--) {
				temp = temp->prev;
			}
		}
	}
	return temp->data;
}
template <class T> void ptr_linked_list<T>::copy(ptr_linked_list<T>* l) {
	if (length < l->length) {
		if (length == 0) {
			head = new ptr_node<T>(l->head->data);
			tail = head;
			length = 1;
		}
		ptr_node<T>* n = head;
		ptr_node<T>* m = l->head;
		while (length < l->length) {
			if (n->next == nullptr) {
				n->next = new ptr_node<T>(m->next->data);
				n->next->prev = n->next;
				length++;
				tail = n->next;
			} else {
				//n->next->copy(m->next);
				n->next = m->next;
			}

			n = n->next;
			m = m->next;
		}
	} else if (length > l->length) {
		ptr_node<T>* n = nullptr;
		if (l->length == 0) {
			n = head;
			while (head != nullptr) {
				head = head->next;
				delete n;
				n = head;
				length--;
			}
		} else {
			n = tail;
			while (length > l->length) {
				tail = tail->prev;
				tail->next = nullptr;
				if (tail->prev) {
					tail->prev->next = tail;
				}
				delete n;
				n = tail;
				length--;
			}
			n = head;
			ptr_node<T>* m = l->head;
			while (n != nullptr) {
				//n->copy(m);
				n = m;
				n = n->next;
				m = m->next;
			}
		}
	}
}
