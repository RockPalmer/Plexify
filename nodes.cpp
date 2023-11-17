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

template <class T> ptr_node<T>::~ptr_node() {
	delete data;
}
template <class T> std::string ptr_node<T>::to_string() {
	return std::to_string(*T);
}

template <class T> arr_node<T>::arr_node(T element, int l) {
	
}