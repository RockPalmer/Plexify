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
	int length;
	node<T>* head;
	node<T>* tail;

	linked_list();
	~linked_list();

	void destruct();
	void destruct_arr();

	std::string to_string();

	void set(int index, T element);
	void insert(int index, T element);
	void remove(int index);
	void append(T element);
	void prepend(T element);
	T get(int index);
};

#endif