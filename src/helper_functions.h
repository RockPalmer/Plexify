#ifndef HELPER_FNS
#define HELPER_FNS

#include <string>

std::string trim(std::string str) {
	std::string result = str;
	if (str.compare("") != 0) {
		for (int i = 0; i < result.length(); i++) {
			if (result[i] != 32 && result[i] != 9) {
				result = result.substr(i);
				break;
			}
		}

		for (int i = result.length(); i > 0; i--) {
			if (result[i-1] != 32 && result[i-1] != 9) {
				result = result.substr(0,i);
				break;
			}
		}
	}
	return result;
}
std::string toBin(size_t v, int max_bits = -1) {
	size_t w = v;
	std::string result = "";
	int tot = (max_bits == -1)?64:max_bits;
	for (int i = 0; i < tot; i++) {
		if (w & 1) {
			result = '1' + result;
		} else {
			result = '0' + result;
		}
		w >>= 1;
	}
	return result;
}

#endif
