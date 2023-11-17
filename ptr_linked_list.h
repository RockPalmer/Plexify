#ifndef PTR_LINKED_LIST
#define PTR_LINKED_LIST

#include <string>

template <class T> struct ptr_node {
	ptr_node* prev;
	ptr_node* next;
	T* data;

	ptr_node();
	ptr_node(T* element);
	~ptr_node();

	void copy(ptr_node<T>* n);

	std::string to_string();
};

template <class T> class ptr_linked_list {
public:
	int length;
	ptr_node<T>* head;
	ptr_node<T>* tail;

	ptr_linked_list();
	~ptr_linked_list();

	void destruct();
	void destruct_arr();

	std::string to_string();
	std::string print_full();

	void set(int index, T* element);
	void insert(int index, T* element);
	void remove(int index);
	void pop(ptr_node<T>* n);
	void append(T* element);
	void prepend(T* element);
	T* get(int index);

	void copy(ptr_linked_list<T>* l);
};

#endif