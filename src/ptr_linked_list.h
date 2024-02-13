/** This class acts the same as the linked_list class with the exception that all data members 
 * are pointers to other objects. The class assumed this and, when the destructor is called,
 * The data at each pointer is also deallocated */

#ifndef PTR_LINKED_LIST
#define PTR_LINKED_LIST

#include <string>

template <class T> struct ptr_node {
	ptr_node* prev;
	ptr_node* next;
	T* data;

	ptr_node();
	ptr_node(T* element, bool copy_vals = true);
	~ptr_node();

	void copy(ptr_node<T>* n);

	std::string to_string();
};

template <class T> class ptr_linked_list {
public:
	size_t length;
	ptr_node<T>* head;
	ptr_node<T>* tail;

	ptr_linked_list();
	~ptr_linked_list();

	void destruct();
	void destruct_arr();

	std::string to_string();
	std::string print_full();

	void set(size_t index, T* element);
	void insert(size_t index, T* element, bool copy_vals = true);
	void remove(size_t index);
	void pop(ptr_node<T>* n);
	void append(T* element, bool copy_vals = true);
	void append(ptr_node<T>* n);
	void prepend(T* element, bool copy_vals = true);
	void prepend(ptr_node<T>* n);
	T* get(size_t index);

	void copy(ptr_linked_list<T>* l);
};

#endif
