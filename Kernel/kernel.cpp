/*********************
* TEXT MODE: 0xB8000 *
* GR.  MODE: 0xA000  *
*********************/

#include "../Drivers/Typedefs.cpp"
#include "../Drivers/VGA_Text.cpp"


void main(){
	char string[] = "There Are Two\n\rColors In My Head";
	SetCursorPos(0,0);
	print(string);
	
	return;
}


