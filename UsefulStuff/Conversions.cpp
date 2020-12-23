//"Software version 7.0"


/*********************** FUNCTIONS *************************
* reverse: reverses a string				     *
* toString: converts int into string - arbitrary base      *
* 							     *
* 							     *
* 							     *
***********************************************************/

const char* reverse(const char* str, char* buffer, int len){
	int i;
	for (i = 0; i <= len; i++){
		buffer[i] = str[len - i];
	}
	buffer[i] = '\0';
	return buffer;
}

char toStringRes[128];

const char* toString(int n, int base) {

	char* buffer = toStringRes;
	
	int m = n;
	int i = 0;
	while(m != 0){
		buffer[i] = (char)((m % base)+ (m % base > 9 ? 55 : 48));
		m = m / base;
		i++;
	}
	buffer[i] = '\0';
	char revBuf[128];
	//asm volatile("1: jmp 1b");
	return reverse(buffer, revBuf, i-1);
} 

