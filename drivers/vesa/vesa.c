// void draw_square(int x, int y, int size, VESA_Colour col){
// 	for (int i = 0; i < size; i++) {
// 		for (int j = 0; j < size; j++) {
// 			FRAMEBUFFER[(y + i) * 1920 + (x + j)] = col.val;
// 		}
// 	}
// }

// void draw_circle(int x, int y, int radius, VESA_Colour col){
// 	for (int i = -radius; i < radius; i++) {
// 		for (int j = -radius; j < radius; j++) {
// 			if (i * i + j * j < radius * radius) {
// 				FRAMEBUFFER[(y + i) * 1920 + (x + j)] = col.val;
// 			}
// 		}
// 	}
// }