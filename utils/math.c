#include "typedefs.h"
#ifdef VGA_VESA
#include "../drivers/vesa/vesa_text.h"
#else
#include "../drivers/vga_text.h"
#endif
#include "math.h"

const float PI = 3.14159265358979323846f;
const float UAT = PI/2.f;
const float TAU = PI*2.f;

/* integer powers of a float */
float pow_f(const float x, const uint32_t n) {
    float res = 1.0f;

    for (uint32_t i = 0; i < n; i++)
        res *= x;

    return res;
}

/* auxiliary function for ln */
float aux_ln (const float x) {
    return (x - 1) / (x + 1);
}


/* returns approx of ln */
/* does not quite scale, maybe implement this with newton*/
float ln (float x, uint32_t order) {
    float res = 0;
    for (uint32_t i = 1; i < order; i+=2){
        res += (1.0f / (float) i) * pow_f(aux_ln(x), i);
    }
    return 2 * (res);
}

/* lookup values for log */
float ln2  = 0.6931471805599453f;
float ln10 = 2.302585092994046f;
float ln16 = 2.772588722239781f;

/* returns ceil of log approx */
uint32_t ceil_log (const uint32_t x, const uint32_t base) {
    if (base == 2)
        return CEILING(ln(x, 1000) / ln2);

    if (base == 10)
        return CEILING(ln(x, 1000) / ln10);

    if (base == 16)
        return CEILING(ln(x, 1000) / ln16);

    kprint("Error: non-implemented base for ceil log");

    for(;;);
}

int abs(const int x) {
    return x < 0 ? -x : x;
}

int min(const int a, const int b) {
    return a < b ? a : b;
}

int max(const int a, const int b) {
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

float fsqrt(float x) {
    const double difference = 0.00001;
    double guess = 1.0;
    while(dabs(guess * guess - x) >= difference){
        guess = (x/guess + guess)/2.0;
    }
    return guess;
}

float sin(float x) {
    return cos(x - (3.14159265358979323846f / 2.0f));
}

/* ATAN LOOKUP
 * contains values fot atan, used for compensating the tangent approx
 */
float atan_lookup (float tan){
    
    if(tan == 1)                return 0.7853981633974483;
    if(tan == .5)               return 0.4636476090008061;
    if(tan == (.25))            return 0.24497866312686414;
    if(tan == (0.125))          return 0.12435499454676144;
    if(tan == (0.0625))         return 0.06241880999595735;
    if(tan == (0.03125))        return 0.031239833430268277;
    if(tan == (0.015625))       return 0.015623728620476831;
    if(tan == (0.0078125))      return 0.007812341060101111;
    if(tan == (0.00390625))     return 0.0039062301319669718;
    if(tan == (0.001953125))    return 0.0019531225164788188;
    
    
    return 1;
}

/* CORDIC COSINE - FLOAT
 * uses a lookup table to compensate for the tan error, nice!
 */
float cos(float theta){
    float sign = (float)((int)floor(theta/PI + .5) % 2 == 0 ? 1 : -1);

    if((int)(2 * theta / PI) != 0) {        // extend to all inputs angles
        theta += UAT;
        theta = fabs(fmod(theta, PI));
        theta -= UAT;
    }
    
    float x = 0.60725293;                   // cosine product
    float y = 0;
    float z = 0;   
    
    for (int i = 0; i < 10; i++){
        
        float d = (z > theta) ? 1 : -1;     // "binary search" direction
        float x_temp = x;
        float pseudotan = powf(2, -i);      // tangent approx
        
        x = x - y * d * pseudotan;
        y = y + x_temp * d * pseudotan;
        
        z = z - d * atan_lookup(pseudotan);
        
    }
    
    return x * sign ;
}

float tan(float x) {
    return sin(x) / cos(x);
}

float floor(float x) {
    int xi = (int)x;
    return (x < 0 && x != (float)xi) ? (float)(xi - 1) : (float)xi;
}

float fabs(float x) {
    return x < 0 ? -x : x;
}

double dabs(double x) {
    return x < 0 ? -x : x;
}

float fmod(float x, float y) {
    if (y == 0.0f) return 0.0f; // Handle division by zero as needed
    int quotient = (int)(x / y);
    return x - (float)quotient * y;
}

float powf(float x, float n) {
    float result = 1.0f;
    int is_negative = 0;

    if (n < 0.0f) {
        is_negative = 1;
        n = -n;
    }

    int int_part = (int)n;
    float frac_part = n - (float)int_part;

    // Integer exponentiation
    for (int i = 0; i < int_part; i++) {
        result *= x;
    }

    // Fractional exponentiation using a simple approximation (e.g., linear interpolation)
    // This is a rough approximation: x^frac_part ≈ 1 + frac_part * (x - 1)
    if (frac_part != 0.0f) {
        result *= (1.0f + frac_part * (x - 1.0f));
    }

    if (is_negative) {
        result = 1.0f / result;
    }

    return result;
}

float fclamp(float x, float min, float max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

uint32_t uclamp(int x, int min, int max) {
    if (x < min) return (uint32_t)min;
    if (x > max) return (uint32_t)max;
    return (uint32_t)x;
}

int clamp(int x, int min, int max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

float fmin(float a, float b) {
    return a < b ? a : b;
}

float fmax(float a, float b) {
    return a > b ? a : b;
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