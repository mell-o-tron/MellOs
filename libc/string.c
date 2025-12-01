/*********************** FUNCTIONS ********************************
 * reverse: reverses a string                                      *
 * strlen: returns length of a string                              *
 * StringsEqu: returns true if strings are equal (max len 80)      *
 * StringStartsWith: returns true if s starts with t (max len 80)  *
 ******************************************************************/

#include "dynamic_mem.h"
#include "include/assert.h"
#include "include/stddef.h"

uint32_t strlen(const char* s) {
	uint32_t res;
	for (res = 0; s[res] != 0; res++)
		;
	return res;
}

void reverse(char s[]) {
	uint32_t length = strlen(s);
	uint32_t c, i, j;

	for (i = 0, j = length - 1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

uint32_t strcmp(const char* s1, const char* s2) {
	while (*s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}
	return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char* strcpy(char* strDest, const char* strSrc) {
	assert(strDest != NULL && strSrc != NULL);
	char* temp = strDest;
	while ((*strDest++ = *strSrc++) != '\0')
		;
	return temp;
}

bool string_starts_with(char* s, char* prefix) {
	while (*prefix)
		if (*s++ != *prefix++)
			return false;
	return true;
}

char* str_decapitate(char* s, uint32_t n) {
	size_t len = strlen(s);
	// fixme: libc-side malloc
	char* res = kmalloc(len - n + 1);

	if (n >= len)
		res[0] = 0;
	else {
		for (size_t i = 0; i <= len - n; i++)
			res[i] = s[i + n];
	}

	return res;
}

char* strdup(const char* s) {
	if (s == NULL)
		return NULL;
	// fixme: libc-side malloc
	char* res = kmalloc(strlen(s) + 1);
	strcpy(res, s);
	return res;
}

char* drop_after(const char delimiter, char* s, bool include) {

	size_t i = 0;
	ssize_t first = -1;
	while (s[i] != delimiter) {
		if (s[i] == '\0') {
			return NULL;
		}

		i++;
	}
	if (first == -1) {
		return NULL;
	}
	for (; first < i - (include ? 1 : 0); first++) {
		s[first] = '\0';
	}
	return s;
}

char* drop_after_last(const char delimiter, char* s, bool include) {
	size_t i = 0;
	ssize_t last = -1;
	while (s[i] != '\0') {
		if (s[i] == delimiter) {
			last = i + (include ? 1 : 0);
		}
		i++;
	}
	if (last == -1) {
		return NULL;
	}

	for (; last < i; last++) {
		s[last + 1] = '\0';
	}
	return s;
}

int memcmp(const void* ptr1, const void* ptr2, size_t size) {
	const uint8_t* p1 = (const uint8_t*)ptr1;
	const uint8_t* p2 = (const uint8_t*)ptr2;

	// 32-bit comparison for aligned data
	if (size >= 4 && ((uintptr_t)p1 & 3) == 0 && ((uintptr_t)p2 & 3) == 0) {
		const uint32_t* q1 = (const uint32_t*)p1;
		const uint32_t* q2 = (const uint32_t*)p2;

		while (size >= 4) {
			if (*q1 != *q2) {
				// Found difference, need to find which byte
				p1 = (const uint8_t*)q1;
				p2 = (const uint8_t*)q2;
				for (int i = 0; i < 4; i++) {
					if (p1[i] != p2[i]) {
						return p1[i] - p2[i];
					}
				}
			}
			q1++;
			q2++;
			size -= 4;
		}
		p1 = (const uint8_t*)q1;
		p2 = (const uint8_t*)q2;
	}

	// Compare remaining bytes
	while (size > 0) {
		if (*p1 != *p2) {
			return *p1 - *p2;
		}
		p1++;
		p2++;
		size--;
	}
	return 0;
}

/**
 * scan memory for byte
 * @param s memory area to scan for c
 * @param c byte to scan for, even if this is an int it gets cast to an unsigned char
 * @param n how many bytes to scan
 * @return pointer to the first occurrence of c in s, or NULL if not found
 */
void* memchr(const void* s, int c, size_t n) {

	const uint8_t* p = s;
	while (n--) {
		if (*p++ == c) {
			return (void*)(p - 1);
		}
	}
	return NULL;
}