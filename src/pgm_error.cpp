pgm_error::pgm_error() {
	type = NONE_ERROR;
	num_size_args = 0;
	num_str_args = 0;
	size_args = nullptr;
	str_args = nullptr;
}
pgm_error::pgm_error(error_type t, int num_int, int num_str) {
	type = t;
	num_size_args = num_int;
	num_str_args = num_str;
	if (num_int > 0) {
		size_args = new size_t[num_int];
	} else {
		size_args = nullptr;
	}
	if (num_str > 0) {
		str_args = new std::string[num_str];
	} else {
		str_args = nullptr;
	}
}
pgm_error::~pgm_error() {
	if (size_args != nullptr) {
		delete[] size_args;
	}
	if (str_args != nullptr) {
		delete[] str_args;
	}
}

void pgm_error::setSizeArgs(size_t arg,...) {
	if (num_size_args > 0) {
		size_args[0] = arg;
		if (num_size_args > 1) {
			va_list args;
			va_start(args,arg);
			for (int i = 1; i < num_size_args; i++) {
				size_args[i] = va_arg(args,size_t);
			}
			va_end(args);
		}
	}
}

void pgm_error::setStrArgs(std::string args[]) {
	if (num_str_args > 0) {
		for (int i = 0; i < num_str_args; i++) {
			str_args[i] = args[i];
		}
	}
}