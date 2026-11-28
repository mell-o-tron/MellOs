#include "colours.h"

VESA_Colour vga2vesa(Colour colour){
    // Standard VGA 4-bit palette
    static VESA_Colour vga_palette[256];
    if (vga_palette[0].a == 0) {
        generate_vga_palette(vga_palette);
    }

    if (colour < 256) {
        return vga_palette[colour];
    } else {
        // Return black for invalid VGA colors
        return (VESA_Colour){0xFF, 0xFF, 0, 0xFF};
    }
}

void generate_vga_palette(VESA_Colour palette[256]) {
    int i = 0;

    // Standard 16-color palette
    uint8_t base_palette[16][3] = {
        {0, 0, 0}, {0, 0, 170}, {0, 170, 0}, {0, 170, 170},
        {170, 0, 0}, {170, 0, 170}, {170, 85, 0}, {170, 170, 170},
        {85, 85, 85}, {85, 85, 255}, {85, 255, 85}, {85, 255, 255},
        {255, 85, 85}, {255, 85, 255}, {255, 255, 85}, {0, 0, 0}
    };

    for (i = 0; i < 16; i++) {
        palette[i] = (VESA_Colour){base_palette[i][2], base_palette[i][1], base_palette[i][0], 0xFF};
    }

    // Generate 216-color cube (6×6×6 colors)
    int r, g, b;
    for (r = 0; r < 6; r++) {
        for (g = 0; g < 6; g++) {
            for (b = 0; b < 6; b++) {
                palette[16 + (r * 36) + (g * 6) + b] = (VESA_Colour){
                    (b * 51), (g * 51), (r * 51), 0xFF  // 51 = 255 / 5 (expand 0–5 scale to 0–255)
                };
            }
        }
    }

    // Generate grayscale ramp (24 shades)
    for (i = 0; i < 24; i++) {
        uint8_t intensity = (i * 255) / 23;  // Scale 0–23 to 0–255
        palette[232 + i] = (VESA_Colour){intensity, intensity, intensity, 0xFF};
    }
}