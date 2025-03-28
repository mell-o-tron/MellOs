#include "typedefs.h"
#ifdef VGA_VESA
#include "../drivers/vesa/vesa_text.h"
#else
#include "../drivers/vga_text.h"
#endif
#include "math.h"


/* integer powers of a float */
float pow_f (float x, uint32_t n) {
    float res = 1.0;

    for (uint32_t i = 0; i < n; i++)
        res *= x;

    return res;
}

/* auxiliary function for ln */
float aux_ln (int x) {
    return ((float)x - 1) / ((float)x+1);
}


/* returns approx of ln */
/* does not quite scale, maybe implement this with newton*/
float ln (float x, uint32_t order) {
    float res = 0;
    for (uint32_t i = 1; i < order; i+=2){
        res += (1.0 / (float) i) * pow_f(aux_ln(x), i);
    }
    return 2 * (res);
}

/* lookup values for log */
float ln2  = 0.6931471805599453;
float ln10 = 2.302585092994046;
float ln16 = 2.772588722239781;

/* returns ceil of log approx */
uint32_t ceil_log (uint32_t x, uint32_t base) {
    if (base == 2)
        return CEILING(ln(x, 1000) / ln2);

    if (base == 10)
        return CEILING(ln(x, 1000) / ln10);

    if (base == 16)
        return CEILING(ln(x, 1000) / ln16);

    kprint("Error: non-implemented base for ceil log");

    for(;;);
}

int abs(int x) {
    return x < 0 ? -x : x;
}

int min(int a, int b) {
    return a < b ? a : b;
}

int max(int a, int b) {
    return a > b ? a : b;
}

int sqrt(int x) {
    int res = 0;
    int bit = 1 << 30; // The second-to-top bit is set: 1 << 30 for 32 bits
 
    // "bit" starts at the highest power of four <= the argument.
    while (bit > x)
        bit >>= 2;
 
    while (bit != 0) {
        if (x >= res + bit) {
            x -= res + bit;
            res = (res >> 1) + bit;
        } else
            res >>= 1;
 
        bit >>= 2;
    }
    return res;
}

int vector2i_distance(Vector2i a, Vector2i b) {
    return sqrt(pow_f(a.x - b.x, 2) + pow_f(a.y - b.y, 2));
}

bool vector2i_in_recti(Vector2i a, Recti b) {
    return a.x >= b.pos.x && a.x <= b.pos.x + b.size.x && a.y >= b.pos.y && a.y <= b.pos.y + b.size.y;
}

Recti recti_union(Recti a, Recti b) {
    Recti r;
    r.pos.x = min(a.pos.x, b.pos.x);
    r.pos.y = min(a.pos.y, b.pos.y);
    Vector2i a_end = {a.pos.x + a.size.x, a.pos.y + a.size.y};
    Vector2i b_end = {b.pos.x + b.size.x, b.pos.y + b.size.y};
    r.size.x = max(a_end.x, b_end.x) - r.pos.x;
    r.size.y = max(a_end.y, b_end.y) - r.pos.y;
    return r;
}

Recti recti_intersection(Recti a, Recti b) {
    Recti r;
    r.pos.x = max(a.pos.x, b.pos.x);
    r.pos.y = max(a.pos.y, b.pos.y);
    Vector2i a_end = {a.pos.x + a.size.x, a.pos.y + a.size.y};
    Vector2i b_end = {b.pos.x + b.size.x, b.pos.y + b.size.y};
    r.size.x = min(a_end.x, b_end.x) - r.pos.x;
    r.size.y = min(a_end.y, b_end.y) - r.pos.y;
    return r;
}