#include "Utils.h"
#ifdef ECLIPSE_BUILD
#include "Arduino/Wire.h"
#else
#include <Wire.h>
#endif
#include <inttypes.h>
#include <string.h>
uint8_t CountOccurance(char* sentence, char search) {
	uint8_t occurance = 0;
	while(*sentence) {
		if (*sentence == search)
			occurance++;
		++sentence;
	}
	return occurance;
}

char *strtok_single (char* str, char const* delims)
{
	static char* src = NULL;
	char* p;
	char* ret = 0;

	if (str != NULL)
		src = str;

	if (src == NULL)
		return NULL;

	if ((p = strpbrk (src, delims)) != NULL) {
		*p  = 0;
		ret = src;
		src = ++p;

	} else if (*src) {
		ret = src;
		src = NULL;
	}

	return ret;
}
