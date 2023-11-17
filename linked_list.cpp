template <class T> node<T>::node() {
	prev = nullptr;
	next = nullptr;
	data = nullptr;
}
template <class T> node<T>::node(T element) {
	prev = nullptr;
	next = nullptr;
	data = element;
}
template <class T> node<T>::~node() {}
template <class T> std::string node<T>::to_string() {
	return std::to_string(data);
}
template <class T> linked_list<T>::linked_list() {
	head = nullptr;
	tail = nullptr;
	length = 0;
}
template <class T> linked_list<T>::~linked_list() {
	node<T>* temp;
	while (head != nullptr) {
		temp = head;
		head = head->next;
		delete temp;
	}
}
template <class T> void linked_list<T>::destruct() {
	for (int i = 0; i < length; i++) {
		delete get(i);
	}
}
template <class T> void linked_list<T>::destruct_arr() {
	for (int i = 0; i < length; i++) {
		delete[] get(i);
	}
}
template <class T> std::string linked_list<T>::to_string() {
	std::string result = "[";
	node<T>* temp = head;
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
			for (int i = 1; i < length; i++) {
				result = result + "," + temp->to_string();
				temp = temp->next;
			}
			result = result + "]";
			break;
	}
	return result;
}
template <class T> void linked_list<T>::set(int index, T element) {
	if (index < length) {
		int start = length - index;
		node<T>* temp;
		if (index < start) {
			// Start from head and move forward
			temp = head;
			for (int i = 0; i < index; i++) {
				temp = temp->next;
			}
		} else {
			// Start from tail and move backward
			temp = tail;
			for (int i = length - 1; i > index; i--) {
				temp = temp->prev;
			}
		}
		temp->data = element;
	}
}
template <class T> void linked_list<T>::insert(int index, T element) {
	if (index < length) {
		node<T>* temp1;
		node<T>* temp2;
		int start;
		if (head == nullptr) {
			head->data = element;
		} else if (index == 0) {
			temp1 = new node<T>(element);
			temp1->next = head;
			head->prev = temp1;
			head = temp1;
		} else if (index == length) {
			temp1 = new node<T>(element);
			temp1->prev = tail;
			tail->next = temp1;
			tail = temp1;
		} else {
			start = length - index;
			temp2 = new node<T>(element);
			if (index < start) {
				// Start from head and work forward
				temp1 = head;
				for (int i = 0; i < index; i++) {
					temp1 = temp1->next;
				}
				temp2->next = temp1;
				temp2->prev = temp1->prev;
				temp1->prev = temp2;
				temp2->prev->next = temp2;
			} else {
				// Start from tail and work backward
				temp1 = tail;
				for (int i = length - 1; i > index; i--) {
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
template <class T> void linked_list<T>::remove(int index) {
	if (index < length) {
		node<T>* temp;
		int start;
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
					for (int i = 0; i < index; i++) {
						temp = temp->next;
					}
				} else {
					// Start at tail and work backward
					temp = tail;
					for (int i = length - 1; i > index; i--) {
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
template <class T> void linked_list<T>::append(T element) {
	if (head == nullptr) {
		head = new node<T>(element);
		tail = head;
	} else {
		tail->next = new node<T>(element);
		tail->next->prev = tail;
		tail = tail->next;
	}
	length++;
}
template <class T> void linked_list<T>::prepend(T element) {
	if (head == nullptr) {
		head = new node<T>(element);
		tail = head;
	} else {
		head->prev = new node<T>(element);
		head->prev->next = head;
		head = head->prev;
	}
	length++;
}
template <class T> T linked_list<T>::get(int index) {
	int start = length - index;
	node<T>* temp;
	if (index < length) {
		if (index < start) {
			// Start from head and move forward
			temp = head;
			for (int i = 0; i < index; i++) {
				temp = temp->next;
			}
		} else {
			// Start from tail and move backward
			temp = tail;
			for (int i = length - 1; i > index; i--) {
				temp = temp->prev;
			}
		}
	}
	return temp->data;
}