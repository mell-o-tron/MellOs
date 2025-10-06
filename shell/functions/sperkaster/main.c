#ifdef VGA_VESA
#include "includes.h"

typedef float float_16_arr[16];
typedef void (*l_char_ptr_to_void_r)(char*);

typedef struct Vec3 {
    float x;
    float y;
    float z;
} Vec3;

typedef Window* Window_ptr;
typedef size_t (*l_Window_ptr_to_size_t_r)(Window*);
typedef VESA_Colour* VESA_Colour_ptr;
typedef VESA_Colour (*l_int__int__int__int_to_VESA_Colour_r)(int, int, int, int);

typedef struct Mat4 {
    float_16_arr m;
} Mat4;

typedef VESA_Colour (*l_VESA_Colour__float_to_VESA_Colour_r)(VESA_Colour, float);
typedef VESA_Colour (*l_VESA_Colour__VESA_Colour_to_VESA_Colour_r)(VESA_Colour, VESA_Colour);

typedef struct HDColour {
    int r;
    int g;
    int b;
    int a;
} HDColour;

typedef Framebuffer* Framebuffer_ptr;
typedef Vec3 (*l__to_Vec3_r)();
typedef VESA_Colour (*l_Vec3_to_VESA_Colour_r)(Vec3);
typedef Vec3 (*l_Vec3_to_Vec3_r)(Vec3);
typedef Vec3 (*l_Vec3__Vec3_to_Vec3_r)(Vec3, Vec3);
typedef Vec3 (*l_Vec3__Vec3__float_to_Vec3_r)(Vec3, Vec3, float);
typedef struct {Vec3 _0; bool _1;} tup_Vec3__bool_le;
typedef struct Vec3_opt {int is_some; Vec3 contents;} Vec3_opt;

typedef struct PerspectiveCamera {
    Vec3 eye;
    Vec3 lookat;
    Vec3 up;
} PerspectiveCamera;

typedef Vec3 (*l_Vec3__float_to_Vec3_r)(Vec3, float);

typedef struct Ray {
    Vec3 origin;
    Vec3 direction;
} Ray;

typedef float (*l_Vec3__Vec3_to_float_r)(Vec3, Vec3);
typedef float (*l_Vec3_to_float_r)(Vec3);

typedef struct Material {
    int type;
    HDColour colour;
    float refractive_index;
    float diffusion;
} Material;

typedef HDColour (*l_HDColour__HDColour_to_HDColour_r)(HDColour, HDColour);
typedef HDColour (*l_HDColour__float_to_HDColour_r)(HDColour, float);
typedef void (*l_size_t__size_t__HDColour__Framebuffer_ptr_to_void_r)(size_t, size_t, HDColour, Framebuffer*);
typedef VESA_Colour (*l_HDColour_to_VESA_Colour_r)(HDColour);
typedef HDColour (*l_Vec3_to_HDColour_r)(Vec3);

typedef struct LightSource {
    Vec3 position;
    float intensity;
    HDColour colour;
} LightSource;

typedef Framebuffer* (*l_Window_ptr_to_Framebuffer_ptr_r)(Window*);
typedef PerspectiveCamera* PerspectiveCamera_ptr;
typedef Vec3 (*l_Ray_ptr__float_to_Vec3_r)(Ray*, float);
typedef Ray* Ray_ptr;
typedef Ray (*l_PerspectiveCamera_ptr__float__float_to_Ray_r)(PerspectiveCamera*, float, float);

typedef struct Parallelepiped {
    Vec3 min;
    Vec3 max;
    Material material;
} Parallelepiped;


typedef struct Sphere {
    float radius;
    Vec3 position;
    Material material;
} Sphere;

typedef Parallelepiped* Parallelepiped_ptr;
typedef Parallelepiped Parallelepiped_1_arr[1];
typedef tup_Vec3__bool_le (*l_Ray_ptr__Vec3__Parallelepiped_ptr_to_tup_Vec3__bool_le_r)(Ray*, Vec3, Parallelepiped*);
typedef Vec3_opt (*l_Ray_ptr__Parallelepiped_ptr_to_Vec3_opt_r)(Ray*, Parallelepiped*);
typedef Sphere Sphere_4_arr[4];
typedef Sphere* Sphere_ptr;
typedef void (*l_Sphere_ptr__int_to_void_r)(Sphere*, int);
typedef tup_Vec3__bool_le (*l_Ray_ptr__Vec3__Sphere_ptr_to_tup_Vec3__bool_le_r)(Ray*, Vec3, Sphere*);
typedef Vec3_opt (*l_Ray_ptr__Sphere_ptr_to_Vec3_opt_r)(Ray*, Sphere*);
typedef HDColour (*l_Ray_ptr__Sphere_4_arr__Parallelepiped_1_arr__int_to_HDColour_r)(Ray*, Sphere_4_arr, Parallelepiped_1_arr, int);
typedef HDColour (*l_Material__Vec3__Ray_ptr__tup_Vec3__bool_le__Sphere_4_arr__Parallelepiped_1_arr__int_to_HDColour_r)(Material, Vec3, Ray*, tup_Vec3__bool_le, Sphere_4_arr, Parallelepiped_1_arr, int);

size_t get_height (Window*);
Framebuffer* get_fb (Window*);
Vec3 vec3_sub (Vec3, Vec3);
HDColour HDColour_scale (HDColour, float);
VESA_Colour VESA_Colour_from_vec3 (Vec3);
float vec3_length_squared (Vec3);
Vec3 vec3_add (Vec3, Vec3);
void fb_set_pixel (size_t, size_t, HDColour, Framebuffer*);
void juggle_balls (Sphere*, int);
Vec3_opt ray_sphere_intersection_point (Ray*, Sphere*);
Vec3 vec3_perturb (Vec3, float);
VESA_Colour make_VESA_Colour (int, int, int, int);
HDColour HDColour_add (HDColour, HDColour);
Vec3 vec3_random_in_unit_sphere ();
Vec3_opt ray_parallelepiped_intersection (Ray*, Parallelepiped*);
Vec3 vec3_scale (Vec3, float);
VESA_Colour HDColour_to_VESA_Colour (HDColour);
VESA_Colour VESA_Colour_add (VESA_Colour, VESA_Colour);
Vec3 vec3_cross (Vec3, Vec3);
float vec3_dot (Vec3, Vec3);
tup_Vec3__bool_le ray_sphere_hit_normal (Ray*, Vec3, Sphere*);
HDColour HDColour_from_vec3 (Vec3);
VESA_Colour VESA_Colour_multiply (VESA_Colour, VESA_Colour);
HDColour HDColour_multiply (HDColour, HDColour);
Vec3 vec3_random_in_hemisphere (Vec3);
void sperkaster (char*);
HDColour cast_ray (Ray*, Sphere_4_arr, Parallelepiped_1_arr, int);
size_t get_width (Window*);
tup_Vec3__bool_le ray_parallelepiped_hit_normal (Ray*, Vec3, Parallelepiped*);
Vec3 vec3_normalized (Vec3);
VESA_Colour VESA_Colour_scale (VESA_Colour, float);
HDColour compute_colour (Material, Vec3, Ray*, tup_Vec3__bool_le, Sphere_4_arr, Parallelepiped_1_arr, int);
Vec3 vec3_reflect (Vec3, Vec3);
Vec3 vec3_rotate (Vec3, Vec3, float);
Ray camera_get_ray (PerspectiveCamera*, float, float);
Vec3 vec3_random ();
float vec3_length (Vec3);
Vec3 ray_at (Ray*, float);

int colour_max = 65535;

int colour_size = 16;

int shift_amt = 8;

int divider = 256;

VESA_Colour make_VESA_Colour(int r, int g, int b, int a) {
    VESA_Colour* colour = ((VESA_Colour_ptr)(0));

    VESA_Colour vc = (VESA_Colour){ .r = r, .g = g, .b = b, .a = a };
    colour = &vc;
    
    return *colour;
}

VESA_Colour VESA_Colour_scale(VESA_Colour c, float s) {
    int r = 0;
    int g = 0;
    int b = 0;
    int a = 0;

    r = fclamp(c.r * s, 0, 255);
    g = fclamp(c.g * s, 0, 255);
    b = fclamp(c.b * s, 0, 255);
    a = c.a;
    
    return make_VESA_Colour(r, g, b, a);
}

VESA_Colour VESA_Colour_add(VESA_Colour c1, VESA_Colour c2) {
    int r = 0;
    int g = 0;
    int b = 0;
    int a = 0;

    r = fclamp(c1.r + c2.r, 0, 255);
    g = fclamp(c1.g + c2.g, 0, 255);
    b = fclamp(c1.b + c2.b, 0, 255);
    a = fclamp(c1.a + c2.a, 0, 255);
    
    return make_VESA_Colour(r, g, b, a);
}

VESA_Colour VESA_Colour_multiply(VESA_Colour c1, VESA_Colour c2) {
    int r = 0;
    int g = 0;
    int b = 0;
    int a = 0;

    r = fclamp((c1.r / 255.0) * (c2.r / 255.0) * 255.0, 0, 255);
    g = fclamp((c1.g / 255.0) * (c2.g / 255.0) * 255.0, 0, 255);
    b = fclamp((c1.b / 255.0) * (c2.b / 255.0) * 255.0, 0, 255);
    a = fclamp((c1.a / 255.0) * (c2.a / 255.0) * 255.0, 0, 255);
    
    return make_VESA_Colour(r, g, b, a);
}

VESA_Colour VESA_Colour_from_vec3(Vec3 v) {
    int r = ((int)(fclamp(((float)(((float)((((float)(v.x)) + ((float)(1.))))) * ((float)(0.5)))) * ((float)(255.)), 0., 255.)));
    int g = ((int)(fclamp(((float)(((float)((((float)(v.y)) + ((float)(1.))))) * ((float)(0.5)))) * ((float)(255.)), 0., 255.)));
    int b = ((int)(fclamp(((float)(((float)((((float)(v.z)) + ((float)(1.))))) * ((float)(0.5)))) * ((float)(255.)), 0., 255.)));
    return make_VESA_Colour(r, g, b, 255);
}


VESA_Colour HDColour_to_VESA_Colour(HDColour hd) {
    int r = clamp(((int)(hd.r)) / ((int)(divider)), 0, 255);
    int g = clamp(((int)(hd.g)) / ((int)(divider)), 0, 255);
    int b = clamp(((int)(hd.b)) / ((int)(divider)), 0, 255);
    int a = clamp(((int)(hd.a)) / ((int)(divider)), 0, 255);
    return make_VESA_Colour(r, g, b, a);
}

HDColour HDColour_scale(HDColour c, float s) {
    return ((HDColour){clamp(((int)(((float)(c.r)) * ((float)(s)))), 0, colour_max), clamp(((int)(((float)(c.g)) * ((float)(s)))), 0, colour_max), clamp(((int)(((float)(c.b)) * ((float)(s)))), 0, colour_max), c.a});
}

HDColour HDColour_multiply(HDColour c1, HDColour c2) {
    return ((HDColour){clamp((((int)((c1.r >> 8))) * ((int)((c2.r >> 8)))), 0, colour_max), clamp((((int)((c1.g >> 8))) * ((int)((c2.g >> 8)))), 0, colour_max), clamp((((int)((c1.b >> 8))) * ((int)((c2.b >> 8)))), 0, colour_max), clamp((((int)((c1.a >> 8))) * ((int)((c2.a >> 8)))), 0, colour_max)});
}

HDColour HDColour_from_vec3(Vec3 v) {
    return ((HDColour){clamp(((int)(((float)(((float)((((float)(v.x)) + ((float)(1.))))) * ((float)(0.5)))) * ((float)(colour_max)))), 0, colour_max), clamp(((int)(((float)(((float)((((float)(v.y)) + ((float)(1.))))) * ((float)(0.5)))) * ((float)(colour_max)))), 0, colour_max), clamp(((int)(((float)(((float)((((float)(v.z)) + ((float)(1.))))) * ((float)(0.5)))) * ((float)(colour_max)))), 0, colour_max), colour_max});
}

HDColour HDColour_add(HDColour c1, HDColour c2) {
    return ((HDColour){clamp(((int)(c1.r)) + ((int)(c2.r)), 0, colour_max), clamp(((int)(c1.g)) + ((int)(c2.g)), 0, colour_max), clamp(((int)(c1.b)) + ((int)(c2.b)), 0, colour_max), clamp(((int)(c1.a)) + ((int)(c2.a)), 0, colour_max)});
}









Vec3 vec3_add(Vec3 a, Vec3 b) {
    return ((Vec3){((float)(a.x)) + ((float)(b.x)), ((float)(a.y)) + ((float)(b.y)), ((float)(a.z)) + ((float)(b.z))});
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
    return ((Vec3){((float)(a.x)) - ((float)(b.x)), ((float)(a.y)) - ((float)(b.y)), ((float)(a.z)) - ((float)(b.z))});
}

Vec3 vec3_scale(Vec3 v, float s) {
    return ((Vec3){((float)(v.x)) * ((float)(s)), ((float)(v.y)) * ((float)(s)), ((float)(v.z)) * ((float)(s))});
}

Vec3 ray_at(Ray* ray, float t) {
    return vec3_add((*ray).origin, vec3_scale((*ray).direction, t));
}

Vec3_opt ray_sphere_intersection_point(Ray* ray, Sphere* sphere) {
    Vec3_opt vecnone = ((Vec3_opt) {0, 0});
    Vec3 oc = vec3_sub((*ray).origin, (*sphere).position);
    float a = ((float)(((float)(((float)((*ray).direction.x)) * ((float)((*ray).direction.x)))) + ((float)(((float)((*ray).direction.y)) * ((float)((*ray).direction.y)))))) + ((float)(((float)((*ray).direction.z)) * ((float)((*ray).direction.z))));
    float b = ((float)(2.)) * ((float)((((float)(((float)(((float)(oc.x)) * ((float)((*ray).direction.x)))) + ((float)(((float)(oc.y)) * ((float)((*ray).direction.y)))))) + ((float)(((float)(oc.z)) * ((float)((*ray).direction.z)))))));
    float c = ((float)(((float)(((float)(((float)(oc.x)) * ((float)(oc.x)))) + ((float)(((float)(oc.y)) * ((float)(oc.y)))))) + ((float)(((float)(oc.z)) * ((float)(oc.z)))))) - ((float)(((float)((*sphere).radius)) * ((float)((*sphere).radius))));
    float discriminant = ((float)(((float)(b)) * ((float)(b)))) - ((float)(((float)(((float)(4.)) * ((float)(a)))) * ((float)(c))));
    if (discriminant < 0.) {
        return vecnone;
    } else {

    }
    float sqrt_disc = fsqrt(discriminant);
    float t1 = ((float)((((float)(-b)) - ((float)(sqrt_disc))))) / ((float)((((float)(2.)) * ((float)(a)))));
    float t2 = ((float)((((float)(-b)) + ((float)(sqrt_disc))))) / ((float)((((float)(2.)) * ((float)(a)))));
    float t = (t1 >= 0. ? t1 : t2);
    if (t < 0.) {
        return vecnone;
    } else {

    }
    return ((Vec3_opt) {1, ray_at(ray, t)});
}

tup_Vec3__bool_le ray_sphere_hit_normal(Ray* ray, Vec3 hit_point, Sphere* sphere) {
    Vec3 normal = vec3_normalized(vec3_sub(hit_point, (*sphere).position));
    return (vec3_dot(normal, (*ray).direction) > 0. ? (tup_Vec3__bool_le){vec3_scale(normal, -1.), true} : (tup_Vec3__bool_le){normal, false});
}

Ray camera_get_ray(PerspectiveCamera* cam, float u, float v) {
    Vec3 forward = vec3_sub((*cam).lookat, (*cam).eye);
    Vec3 right = vec3_cross(forward, (*cam).up);
    Vec3 up = (*cam).up;
    Vec3 direction = vec3_normalized(vec3_add(vec3_add(vec3_scale(right, u), vec3_scale(up, v)), forward));
    return ((Ray){(*cam).eye, direction});
}

float vec3_dot(Vec3 a, Vec3 b) {
    return ((float)(((float)(((float)(a.x)) * ((float)(b.x)))) + ((float)(((float)(a.y)) * ((float)(b.y)))))) + ((float)(((float)(a.z)) * ((float)(b.z))));
}

Vec3 vec3_cross(Vec3 a, Vec3 b) {
    return ((Vec3){((float)(((float)(a.y)) * ((float)(b.z)))) - ((float)(((float)(a.z)) * ((float)(b.y)))), ((float)(((float)(a.z)) * ((float)(b.x)))) - ((float)(((float)(a.x)) * ((float)(b.z)))), ((float)(((float)(a.x)) * ((float)(b.y)))) - ((float)(((float)(a.y)) * ((float)(b.x))))});
}

Vec3 vec3_normalized(Vec3 v) {
    float len = vec3_length(v);
    if (len == 0.) {
        return ((Vec3){0., 0., 0.});
    } else {

    }
    return ((Vec3){((float)(v.x)) / ((float)(len)), ((float)(v.y)) / ((float)(len)), ((float)(v.z)) / ((float)(len))});
}

Vec3_opt ray_parallelepiped_intersection(Ray* ray, Parallelepiped* para) {
    Vec3_opt vecnone = ((Vec3_opt) {0, 0});
    float tmin = ((float)((((float)((*para).min.x)) - ((float)((*ray).origin.x))))) / ((float)((*ray).direction.x));
    float tmax = ((float)((((float)((*para).max.x)) - ((float)((*ray).origin.x))))) / ((float)((*ray).direction.x));
    if (tmin > tmax) {
        float temp = tmin;
        tmin = tmax;
        tmax = temp;
    } else {

    }
    float tymin = ((float)((((float)((*para).min.y)) - ((float)((*ray).origin.y))))) / ((float)((*ray).direction.y));
    float tymax = ((float)((((float)((*para).max.y)) - ((float)((*ray).origin.y))))) / ((float)((*ray).direction.y));
    if (tymin > tymax) {
        float temp = tymin;
        tymin = tymax;
        tymax = temp;
    } else {

    }
    if ((tmin > tymax) || (tymin > tmax)) {
        return vecnone;
    } else {

    }
    if (tymin > tmin) {
        tmin = tymin;
    } else {

    }
    if (tymax < tmax) {
        tmax = tymax;
    } else {

    }
    float tzmin = ((float)((((float)((*para).min.z)) - ((float)((*ray).origin.z))))) / ((float)((*ray).direction.z));
    float tzmax = ((float)((((float)((*para).max.z)) - ((float)((*ray).origin.z))))) / ((float)((*ray).direction.z));
    if (tzmin > tzmax) {
        float temp = tzmin;
        tzmin = tzmax;
        tzmax = temp;
    } else {

    }
    if ((tmin > tzmax) || (tzmin > tmax)) {
        return vecnone;
    } else {

    }
    if (tzmin > tmin) {
        tmin = tzmin;
    } else {

    }
    if (tzmax < tmax) {
        tmax = tzmax;
    } else {

    }
    if (tmin < 0. && tmax < 0.) {
        return vecnone;
    } else {

    }
    float t = (tmin >= 0. ? tmin : tmax);
    return ((Vec3_opt) {1, ray_at(ray, t)});
}

tup_Vec3__bool_le ray_parallelepiped_hit_normal(Ray* ray, Vec3 hit_point, Parallelepiped* para) {
    float epsilon = 0.0001;
    if (fabs(((float)(hit_point.x)) - ((float)((*para).min.x))) < epsilon) {
        return (tup_Vec3__bool_le){((Vec3){-1., 0., 0.}), true};
    } else {
        if (fabs(((float)(hit_point.x)) - ((float)((*para).max.x))) < epsilon) {
            return (tup_Vec3__bool_le){((Vec3){1., 0., 0.}), true};
        } else {
            if (fabs(((float)(hit_point.y)) - ((float)((*para).min.y))) < epsilon) {
                return (tup_Vec3__bool_le){((Vec3){0., -1., 0.}), true};
            } else {
                if (fabs(((float)(hit_point.y)) - ((float)((*para).max.y))) < epsilon) {
                    return (tup_Vec3__bool_le){((Vec3){0., 1., 0.}), true};
                } else {
                    if (fabs(((float)(hit_point.z)) - ((float)((*para).min.z))) < epsilon) {
                        return (tup_Vec3__bool_le){((Vec3){0., 0., -1.}), true};
                    } else {
                        if (fabs(((float)(hit_point.z)) - ((float)((*para).max.z))) < epsilon) {
                            return (tup_Vec3__bool_le){((Vec3){0., 0., 1.}), true};
                        } else {

                        }
                    }
                }
            }
        }
    }
    return (tup_Vec3__bool_le){((Vec3){0., 0., 0.}), false};
}

Vec3 vec3_rotate(Vec3 vector, Vec3 axis, float angle) {
    float cos_theta = cos(angle);
    float sin_theta = sin(angle);
    return vec3_add(vec3_scale(vector, cos_theta), vec3_add(vec3_scale(vec3_cross(axis, vector), sin_theta), vec3_scale(axis, ((float)(vec3_dot(axis, vector))) * ((float)((((float)(1.)) - ((float)(cos_theta))))))));
}

float vec3_length(Vec3 v) {
    return fsqrt(((float)(((float)(((float)(v.x)) * ((float)(v.x)))) + ((float)(((float)(v.y)) * ((float)(v.y)))))) + ((float)(((float)(v.z)) * ((float)(v.z)))));
}

float vec3_length_squared(Vec3 v) {
    return ((float)(((float)(((float)(v.x)) * ((float)(v.x)))) + ((float)(((float)(v.y)) * ((float)(v.y)))))) + ((float)(((float)(v.z)) * ((float)(v.z))));
}

Vec3 vec3_random() {
    return ((Vec3){((float)(((float)(frand())) * ((float)(2.)))) - ((float)(1.)), ((float)(((float)(frand())) * ((float)(2.)))) - ((float)(1.)), ((float)(((float)(frand())) * ((float)(2.)))) - ((float)(1.))});
}

Vec3 vec3_random_in_unit_sphere() {
    return vec3_normalized(vec3_random());
}

Vec3 vec3_random_in_hemisphere(Vec3 normal) {
    Vec3 in_unit_sphere = vec3_random_in_unit_sphere();
    if (vec3_dot(in_unit_sphere, normal) > 0.) {
        return in_unit_sphere;
    } else {
        return vec3_scale(in_unit_sphere, -1.);
    }
}

Vec3 vec3_reflect(Vec3 v, Vec3 n) {
    return vec3_sub(v, vec3_scale(n, ((float)(2.)) * ((float)(vec3_dot(v, n)))));
}

Vec3 vec3_perturb(Vec3 v, float magnitude) {
    return ((Vec3){((float)(v.x)) + ((float)(((float)((((float)(((float)(frand())) * ((float)(2.)))) - ((float)(1.))))) * ((float)(magnitude)))), ((float)(v.y)) + ((float)(((float)((((float)(((float)(frand())) * ((float)(2.)))) - ((float)(1.))))) * ((float)(magnitude)))), ((float)(v.z)) + ((float)(((float)((((float)(((float)(frand())) * ((float)(2.)))) - ((float)(1.))))) * ((float)(magnitude))))});
}


int pixel_size = 2;

uint32_t hres = ((uint32_t)(((int)(600)) / ((int)(2))));

uint32_t vres = ((uint32_t)(((int)(400)) / ((int)(2))));

HDColour sky_colour = ((HDColour){144 << 8, 227 << 8, 252 << 8, 255 << 8});

HDColour no_colour = ((HDColour){0, 0, 0, 65535});

int num_rays = 5;

int num_bounces = 10;

float scatter_amount = 0.001;

float acne_bias = 0.001;

int initial_ticks = 78;

int ticks = 0;

bool animate = true;

bool done = false;

int sleep_time = 1;

Framebuffer* get_fb(Window* w) {
    Framebuffer* fb = ((Framebuffer_ptr)(0));

    fb = (*w).fb;
    
    return fb;
}

size_t get_width(Window* w) {
    size_t width = ((size_t)(0));

    width = (*w).width;
    
    return width;
}

size_t get_height(Window* w) {
    size_t height = ((size_t)(0));

    height = (*w).height;
    
    return height;
}

HDColour compute_colour(Material mat, Vec3 normal, Ray* ray, tup_Vec3__bool_le hit, Sphere_4_arr spheres, Parallelepiped_1_arr parallelepipeds, int max_bounces) {
    Vec3 reflected_dir = vec3_reflect((*ray).direction, normal);
    Vec3 new_origin = vec3_add(hit._0, vec3_scale(reflected_dir, acne_bias));
    if (mat.type == 0) {
        return mat.colour;
    } else {
        if (mat.type == 1) {
            Vec3 direction = vec3_add(vec3_random_in_unit_sphere(), normal);
            HDColour sample = HDColour_scale(cast_ray(&(((Ray){new_origin, direction})), spheres, parallelepipeds, ((int)(max_bounces)) - ((int)(1))), mat.diffusion);
            return HDColour_multiply(sample, mat.colour);
        } else {
            if (mat.type == 2) {
                    return HDColour_multiply(cast_ray(&(((Ray){new_origin, reflected_dir})), spheres, parallelepipeds, ((int)(max_bounces)) - ((int)(1))), mat.colour);
            } else {
                if (mat.type == 3) {
                    float etai_over_etat = (hit._1 ? mat.refractive_index : ((float)(1.)) / ((float)(mat.refractive_index)));
                    float cos_theta = fmin(vec3_dot(vec3_scale((*ray).direction, -1.), normal), 1.);
                    float sin_theta = fsqrt(((float)(1.)) - ((float)(((float)(cos_theta)) * ((float)(cos_theta)))));
                    if (((float)(etai_over_etat)) * ((float)(sin_theta)) > 1.) {
                        return HDColour_multiply(cast_ray(&(((Ray){new_origin, reflected_dir})), spheres, parallelepipeds, ((int)(max_bounces)) - ((int)(1))), mat.colour);
                    } else {

                    }
                    Vec3 r_out_perp = vec3_scale(vec3_add((*ray).direction, vec3_scale(normal, cos_theta)), etai_over_etat);
                    Vec3 r_out_parallel = vec3_scale(normal, -fsqrt(fabs(((float)(1.)) - ((float)(vec3_length_squared(r_out_perp))))));
                    Vec3 refracted = vec3_add(r_out_perp, r_out_parallel);
                    return cast_ray(&(((Ray){vec3_add(hit._0, vec3_scale(refracted, acne_bias)), refracted})), spheres, parallelepipeds, ((int)(max_bounces)) - ((int)(1)));
                } else {
                    if (mat.type == -1) {
                        return HDColour_from_vec3(normal);
                    } else {
                        if (mat.type == -2) {
                            float intensity = fclamp(fabs(vec3_dot(normal, (*ray).direction)), 0., 1.);
                            return HDColour_scale(HDColour_from_vec3(normal), intensity);
                        } else {
                            if (mat.type == -3) {
                                return HDColour_from_vec3(reflected_dir);
                            } else {
                                if (mat.type == -4) {
                                    return HDColour_from_vec3(hit._0);
                                } else {

                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return sky_colour;
}

HDColour cast_ray(Ray* ray, Sphere_4_arr spheres, Parallelepiped_1_arr parallelepipeds, int max_bounces) {
    if (max_bounces <= 0) {
        return no_colour;
    } else {

    }
    Vec3_opt hit = ((Vec3_opt) {0, 0});
    for (size_t i = ((size_t)(0)); i < 4; i = ((size_t)(i)) + ((size_t)(1))) {
        hit = ray_sphere_intersection_point(ray, &spheres[((int)(i))]);
        if (hit.is_some) {
            tup_Vec3__bool_le normal = ray_sphere_hit_normal(ray, ((Vec3)hit.contents), &spheres[((int)(i))]);
            Material mat = spheres[((int)(i))].material;
            return compute_colour(mat, normal._0, ray, (tup_Vec3__bool_le){((Vec3)hit.contents), normal._1}, spheres, parallelepipeds, max_bounces);
        } else {

        }
    }
    for (size_t j = ((size_t)(0)); j < 1; j = ((size_t)(j)) + ((size_t)(1))) {
        hit = ray_parallelepiped_intersection(ray, &parallelepipeds[((int)(j))]);
        if (hit.is_some) {
            Material mat = parallelepipeds[((int)(j))].material;
            tup_Vec3__bool_le normal = ray_parallelepiped_hit_normal(ray, ((Vec3)hit.contents), &parallelepipeds[((int)(j))]);
            return compute_colour(mat, normal._0, ray, (tup_Vec3__bool_le){((Vec3)hit.contents), normal._1}, spheres, parallelepipeds, max_bounces);
        } else {

        }
    }
    return sky_colour;
}

void juggle_balls(Sphere* sphere, int ticks) {
    float pos_x = ((float)(20.)) * ((float)(sin(((float)(((float)(((int)(initial_ticks)) + ((int)(ticks)))))) * ((float)(0.1)))));
    float pos_y = fabs(((float)(20.)) * ((float)(cos(((float)(((float)(((int)(initial_ticks)) + ((int)(ticks)))))) * ((float)(0.1))))));
    (*sphere).position = ((Vec3){pos_x, pos_y, (*sphere).position.z});
}

void sperkaster(char* s) {
    bool in_vell = false;
    Framebuffer* fb = ((Framebuffer_ptr)(0));
    Window* w = ((Window_ptr)(0));
    if (!_vell_is_active()) {
        in_vell = false;
        fb = vga_fb;
    } else {
        in_vell = true;
        w = create_window_with_size("Sperkaster", ((uint32_t)(hres)) * ((uint32_t)(pixel_size)), ((uint32_t)(vres)) * ((uint32_t)(pixel_size)));
        set_window_dirty(w);
        fb = get_fb(w);
    }
    Sphere_4_arr spheres = {((Sphere){8., ((Vec3){0., 0., -50.}), ((Material){1, ((HDColour){colour_max, 0, 0, colour_max}), 1., 0.5})}), ((Sphere){8., ((Vec3){0., 0., -60.}), ((Material){2, ((HDColour){200 << shift_amt, 250 << shift_amt, 200 << shift_amt, colour_max}), 1., 0.5})}), ((Sphere){8., ((Vec3){-40., 0., -70.}), ((Material){3, ((HDColour){250 << shift_amt, 250 << shift_amt, 250 << shift_amt, colour_max}), ((float)(1.)) / ((float)(1.33)), 1.})}), ((Sphere){8., ((Vec3){40., 0., -80.}), ((Material){1, ((HDColour){0, 0, colour_max, colour_max}), 1., 1.})})};
    Parallelepiped_1_arr parallelepipeds = {((Parallelepiped){((Vec3){-4000., -10., -4000.}), ((Vec3){4000., -8., 4000.}), ((Material){1, ((HDColour){80 << shift_amt, 96 << shift_amt, 80 << shift_amt, 255 << shift_amt}), 1., 1.})})};
    PerspectiveCamera camera = ((PerspectiveCamera){((Vec3){5., 10., -10.}), ((Vec3){5., 10., -11.}), ((Vec3){0., -1., 0.})});
    LightSource light_source = ((LightSource){((Vec3){5., 5., 0.}), 1., ((HDColour){colour_max, colour_max, colour_max, colour_max})});
    float aspect_ratio = ((float)(((float)(hres)))) / ((float)(((float)(vres))));
    float fov = 60.;
    float fov_rad = ((float)(((float)(fov)) * ((float)(3.14159)))) / ((float)(180.));
    float scale = tan(((float)(fov_rad)) * ((float)(0.5)));
    float inverse_num_rays = ((float)(1.)) / ((float)(((float)(num_rays))));
    juggle_balls(&spheres[0], ticks);
    juggle_balls(&spheres[1], ((int)(ticks)) + ((int)(10)));
    juggle_balls(&spheres[2], ((int)(ticks)) + ((int)(20)));
    juggle_balls(&spheres[3], ((int)(ticks)) + ((int)(30)));
    ticks = ((int)(ticks)) + ((int)(1));
    while (true) {
        if (!done) {
        for (size_t y = ((size_t)(0)); y < vres; y = ((size_t)(y)) + ((size_t)(1))) {
            for (size_t x = ((size_t)(0)); x < hres; x = ((size_t)(x)) + ((size_t)(1))) {
                float x_norm = ((float)(((float)((((float)(((float)(x)))) / ((float)(((float)(hres))))))) * ((float)(2.)))) - ((float)(1.));
                float y_norm = ((float)(((float)((((float)(((float)(y)))) / ((float)(((float)(vres))))))) * ((float)(2.)))) - ((float)(1.));
                float u = -((float)(((float)(x_norm)) * ((float)(aspect_ratio)))) * ((float)(scale));
                float v = ((float)(y_norm)) * ((float)(scale));
                Ray ray = camera_get_ray(&camera, u, v);
                HDColour colour = ((HDColour){0, 0, 0, colour_max});
                for (int r = 0; r < num_rays; r = ((int)(r)) + ((int)(1))) {
                    Ray ray_i = ((Ray){ray.origin, vec3_normalized(vec3_perturb(ray.direction, scatter_amount))});
                    HDColour sample = HDColour_scale(cast_ray(&ray_i, spheres, parallelepipeds, num_bounces), inverse_num_rays);
                    colour = HDColour_add(colour, sample);
                }
                fb_set_pixel(x, y, colour, fb);
                char c = get_from_kb_buffer();
                if (c == 'q') {
                    if (in_vell) {
                        destroy_window(w);
                    } else {

                    }
                        printf("%d ticks rendered\n", ticks);
                        ticks = 0;
                    return;
                } else {

                }
            }
        }
        if (in_vell) {
            set_window_dirty(w);
            } else {

            }
        } else {

        }
        if (animate) {
            juggle_balls(&spheres[0], ticks);
            juggle_balls(&spheres[1], ((int)(ticks)) + ((int)(10)));
            juggle_balls(&spheres[2], ((int)(ticks)) + ((int)(20)));
            juggle_balls(&spheres[3], ((int)(ticks)) + ((int)(30)));
            ticks = ((int)(ticks)) + ((int)(1));
        } else {
            done = true;
        }
        sleep(sleep_time);
    }
}

void fb_set_pixel(size_t x, size_t y, HDColour colour, Framebuffer* fb) {
    VESA_Colour colourVESA = HDColour_to_VESA_Colour(colour);

    if ((x * pixel_size) < (*fb).width && (y * pixel_size) < (*fb).height) {
        if (pixel_size == 1) {
            size_t index = y * (*fb).width + x;
            (*fb).fb[index] = colourVESA.val;
            return;
        }
        for (size_t dy = 0; dy < pixel_size; dy = dy + 1) {
            for (size_t dx = 0; dx < pixel_size; dx = dx + 1) {
            size_t index = (y * pixel_size + dy) * (*fb).width + (x * pixel_size + dx);
            (*fb).fb[index] = colourVESA.val;
            }
        }
    }
    
}
#endif