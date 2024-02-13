/* 
 * This header file defines basic functions that are used throughout 
 * the program to perform various basic functions 
 */

#ifndef HELPER_FNS
#define HELPER_FNS

#include <string>

/* 
 * This function takes in a string as a parameter and returns a copy
 * of that string with all the whitespace at the beginning and end of
 * the string resmoved.
 */
std::string trim(std::string str) {

	// Copy the string
	std::string result = str;

	// If str is not empty
	if (str.compare("") != 0) {

		// Remove all whitespace from the beginning
		for (int i = 0; i < result.length(); i++) {
			if (result[i] != 32 && result[i] != 9) {
				result = result.substr(i);
				break;
			}
		}

		// Remove all whitespace from the end of the string
		for (int i = result.length(); i > 0; i--) {
			if (result[i-1] != 32 && result[i-1] != 9) {
				result = result.substr(0,i);
				break;
			}
		}
	}
	return result;
}

/*
 * Returns a string of binary digits representing v, with <max_bits> bits
 * If max_bits is -1, use 64 as the maximum bits
 */
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
