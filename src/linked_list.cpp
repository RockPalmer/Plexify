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

/* 
 * Returns a string representaion of the data in the node.
 */
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

	// Delete all nodes in the list
	while (head != nullptr) {
		temp = head;
		head = head->next;
		delete temp;
	}
}

/*
 * This function assumes that the content of the linked
 * list is of pointer type. This function iterates through 
 * the nodes of the linked list and performs the delete
 * operation on the data.
 */
template <class T> void linked_list<T>::destruct() {
	node<T>* n = head;
	for (size_t i = 0; i < length; i++) {
		delete n->data;
		n = n->next;
	}
}
/*
 * This function assumes that the content of the linked
 * list is of dynamic array type. This function iterates through 
 * the nodes of the linked list and performs the delete[]
 * operation on the data.
 */
template <class T> void linked_list<T>::destruct_arr() {
	node<T>* n = head;
	for (size_t i = 0; i < length; i++) {
		delete[] n->data;
		n = n->next;
	}
}

/*
 * Returns a string representation of the linked list
 */
template <class T> std::string linked_list<T>::to_string() {
	std::string result = "[";
	node<T>* temp = head;
	switch (length) {
		case 0: // If the list is empty
			result = result + "EMPTY]";
			break;
		case 1: // If there is only one node in the list
			result = result + head->to_string() + "]";
			break;
		default: // Otherwise
			result = result + head->to_string();
			temp = temp->next;

			// Iterate through the nodes of the list
			for (size_t i = 1; i < length; i++) {
				result = result + "," + temp->to_string();
				temp = temp->next;
			}
			result = result + "]";
			break;
	}
	return result;
}

/*
 * This function sets the value of the <index>th node of the list to
 * <element>. The <type> parameter determines what the function will do
 * with the previous value at that index.
 * 
 * type = 0 -> do nothing
 * type = 1 -> perform delete operation
 * type = 2 -> perform delete[] operation
 */
template <class T> void linked_list<T>::set(size_t index, T element, int type) {
	if (index < length) {
		size_t start = length - index;
		node<T>* temp;
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
		switch (type) {
			case 1:
				delete temp->data;
				break;
			case 2:
				delete[] temp->data;
				break;
		}
		temp->data = element;
	}
}
/*
 * This function inserts a node at index <index> containing <element> as its data.
 */
template <class T> void linked_list<T>::insert(size_t index, T element) {
	if (index < length) {
		node<T>* temp1;
		node<T>* temp2;
		size_t start;
		if (head == nullptr) { // If the list is empty
			head->data = element;
		} else if (index == 0) { // If the list is not empty and the node must be inserted at the beginning of the list
			temp1 = new node<T>(element);
			temp1->next = head;
			head->prev = temp1;
			head = temp1;
		} else if (index == length) { // If the list is not empty and the node must be inserted at the end of the list
			temp1 = new node<T>(element);
			temp1->prev = tail;
			tail->next = temp1;
			tail = temp1;
		} else { // Otherwise
			start = length - index;
			temp2 = new node<T>(element);
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
/*
 * This function inserts the node <m> after the node <n> in the list
 */
template <class T> void linked_list<T>::insertAfter(node<T>* n, node<T>* m) {
	if (n == tail) {
		n->next = m;
		m->next = nullptr;
		m->prev = n;
		tail = m;
	} else {
		m->next = n->next;
		n->next->prev = m;
		m->prev = n;
		n->next = m;
	}
	length++;
}
/*
 * This function removes the node at index <index> from the list and deletes
 * the node. The type parameter determines what to do with the data contained 
 * in the node once its removed.
 * 
 * type = 0 -> do nothing
 * type = 1 -> perform delete operation
 * type = 2 -> perform delete[] operation
 */
template <class T> void linked_list<T>::remove(size_t index, int type) {
	if (index < length) {
		node<T>* temp;
		size_t start;

		if (head == tail && head != nullptr) { // If there is only one node in the list
			switch (type) {
				case 1:
					delete head->data;
					break;
				case 2:
					delete[] head->data;
					break;
			}
			delete head;
			head = nullptr;
			tail = head;
		} else { // If there is more than one element in the list
			if (index == 0) { // If the first node in the list must be removed
				temp = head;
				head = head->next;
				head->prev = nullptr;
			} else if (index == length - 1) { // If the last node in the list must be removed
				temp = tail;
				tail = tail->prev;
				tail->next = nullptr;
			} else { // Otherwise
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
			switch (type) {
				case 1:
					delete temp->data;
					break;
				case 2:
					delete[] temp->data;
					break;
			}
			delete temp;
		}
		length--;
	}
}
/*
 * Removes the node <n> from the list without deleting it.
 */
template <class T> void linked_list<T>::pop(node<T>* n) {
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
/*
 * Removes the first node from the list and returns it
 */
template <class T> node<T>* linked_list<T>::pop() {
	node<T>* n = head;
	if (head != nullptr) {
		head = head->next;
		length--;
	} else {
		tail = head;
	}
	return n;
}
/*
 * Appends a new node to the end of the list with <element> as its data
 */
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
/*
 * Appends the node <n> to the end of the list
 */
template <class T> void linked_list<T>::append(node<T>* n) {
	if (head == nullptr) {
		head = n;
		tail = head;
	} else {
		tail->next = n;
		tail->next->prev = tail;
		tail = tail->next;
	}
	length++;
}
/*
 * Appends a new node to the beginning of the list with <element> as its data
 */
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
/*
 * Appends the node <n> to the beginning of the list
 */
template <class T> void linked_list<T>::prepend(node<T>* n) {
	if (head == nullptr) {
		head = n;
		tail = head;
	} else {
		head->prev = n;
		head->prev->next = head;
		head = head->prev;
	}
	length++;
}
/*
 * Returns the data at the <index>th node of the list
 */
template <class T> T linked_list<T>::get(size_t index) {
	size_t start = length - index;
	node<T>* temp;
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
