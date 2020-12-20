/*********************
* TEXT MODE: 0xB8000 *
* GR.  MODE: 0xA000  *
*********************/

#define VRAM	0xB8000

void RawPrint(char s[], int curspos);

void main(){
	char string[] = "There Are Two Colors In My Head";
	RawPrint(string, 0);
	return;
}


void RawPrint(char s[], int curspos){
	char* letter = (char*) (VRAM + 2*curspos);
	char c = s[0];
	int i = 0;
	while(c != (char)0){
		
		*letter = c;
		i++;
		letter += 2;
		c = s[i];
	}
	return;
}
