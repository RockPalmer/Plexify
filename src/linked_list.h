#ifndef LINKED_LIST
#define LINKED_LIST

#include <string>

template <class T> struct node {
	node* prev;
	node* next;
	T data;

	node();
	node(T element);
	~node();

	std::string to_string();
};

template <class T> class linked_list {
public:
	size_t length;
	node<T>* head;
	node<T>* tail;

	linked_list();
	~linked_list();

	void destruct();
	void destruct_arr();

	std::string to_string();

	void set(size_t index, T element, int type = 0);
	void insert(size_t index, T element);
	void remove(size_t index, int type = 0);
	void pop(node<T>* n);
	node<T>* pop();
	void append(T element);
	void append(node<T>* n);
	void prepend(T element);
	T get(size_t index);
};

#endif
