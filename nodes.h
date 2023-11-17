#ifndef NODE
#define NODE

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

template <class T> struct ptr_node : public node<T> {
	~ptr_node();
	std::string to_string();
};

template <class T> struct arr_node : public node<T> {
	int size;

	arr_node(T element, int l);
	~arr_node();
	std::string to_string();
};

#endif