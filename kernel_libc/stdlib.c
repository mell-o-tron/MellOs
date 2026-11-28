#include "stdlib.h"

int atoi(char* str) {
	int result = 0;
	int i = 0;
	while (str[i] != 0) {
		result = result * 10 + ( str[i] - '0');
	}
	return result;
}