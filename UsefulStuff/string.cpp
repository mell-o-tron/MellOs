/*********************** FUNCTIONS ****************************
* reverse: reverses a string                                  *
* strLen: returns length of a string                          *
* StringsEqu: returns true if strings are equal (max len 80)  *
**************************************************************/

const char* reverse(const char* str, char* buffer, int len){
	int i;
	for (i = 0; i <= len; i++){
		buffer[i] = str[len - i];
	}
	buffer[i] = '\0';
	return buffer;
}

int strLen(const char* s){
	int res;
	for(res = 0; s[res] != 0; res++);
	return res;
}


bool StringsEqu(const char* s, const char* t){
	const char* g = strLen(s) > strLen(t)? s : t;
	
	bool res = true;
	int i = 0;
	do{
		if(s[i] != t[i] || i > 80){
			res = false;
			break;
		}
		else i++;
	}while(g[i] != 0);
	return res;
}
