void DrawPixel(int x, int y);
void _start(){
	DrawPixel(1,0);
	DrawPixel(2,0);
	DrawPixel(3,0);
	
}

void DrawPixel(int x, int y){
	int* VRAM = (int*)0xA0000;
	
	int* Pixel = VRAM + (x*1) + (y*80);
	*Pixel = 0x0e;
	return;
}
