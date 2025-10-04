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
typedef void (*l_size_t__size_t__VESA_Colour__Framebuffer_ptr_to_void_r)(size_t, size_t, VESA_Colour, Framebuffer*);
typedef VESA_Colour* VESA_Colour_ptr;
typedef VESA_Colour (*l_int__int__int__int_to_VESA_Colour_r)(int, int, int, int);

typedef struct Mat4 {
    float_16_arr m;
} Mat4;

typedef Framebuffer* Framebuffer_ptr;
typedef Vec3 (*l_Vec3_to_Vec3_r)(Vec3);
typedef Vec3 (*l_Vec3__Vec3_to_Vec3_r)(Vec3, Vec3);

typedef struct LightSource {
    Vec3 position;
    float intensity;
    VESA_Colour colour;
} LightSource;


typedef struct Parallelepiped {
    Vec3 min;
    Vec3 max;
    VESA_Colour colour;
} Parallelepiped;

typedef struct Vec3_opt {int is_some; Vec3 contents;} Vec3_opt;

typedef struct PerspectiveCamera {
    Vec3 eye;
    Vec3 lookat;
    Vec3 up;
} PerspectiveCamera;


typedef struct Sphere {
    float radius;
    Vec3 position;
    VESA_Colour colour;
} Sphere;

typedef Vec3 (*l_Vec3__float_to_Vec3_r)(Vec3, float);

typedef struct Ray {
    Vec3 origin;
    Vec3 direction;
} Ray;

typedef float (*l_Vec3__Vec3_to_float_r)(Vec3, Vec3);
typedef Framebuffer* (*l_Window_ptr_to_Framebuffer_ptr_r)(Window*);
typedef Parallelepiped* Parallelepiped_ptr;
typedef Parallelepiped Parallelepiped_0_arr[0];
typedef PerspectiveCamera* PerspectiveCamera_ptr;
typedef Sphere* Sphere_ptr;
typedef void (*l_Sphere_ptr__int_to_void_r)(Sphere*, int);
typedef Sphere Sphere_3_arr[3];
typedef Vec3 (*l_Ray_ptr__float_to_Vec3_r)(Ray*, float);
typedef Ray* Ray_ptr;
typedef Vec3 (*l_Ray_ptr__Vec3__Parallelepiped_ptr_to_Vec3_r)(Ray*, Vec3, Parallelepiped*);
typedef Vec3_opt (*l_Ray_ptr__Parallelepiped_ptr_to_Vec3_opt_r)(Ray*, Parallelepiped*);
typedef Ray (*l_PerspectiveCamera_ptr__float__float_to_Ray_r)(PerspectiveCamera*, float, float);
typedef Vec3 (*l_Ray_ptr__Vec3__Sphere_ptr_to_Vec3_r)(Ray*, Vec3, Sphere*);
typedef VESA_Colour (*l_Ray_ptr__Sphere_3_arr__Parallelepiped_0_arr__int_to_VESA_Colour_r)(Ray*, Sphere_3_arr, Parallelepiped_0_arr, int);
typedef Vec3_opt (*l_Ray_ptr__Sphere_ptr_to_Vec3_opt_r)(Ray*, Sphere*);

size_t jb_get_height (Window*);
Framebuffer* jb_get_fb (Window*);
Vec3 jb_vec3_cross (Vec3, Vec3);
float jb_vec3_dot (Vec3, Vec3);
Vec3 jb_ray_sphere_hit_normal (Ray*, Vec3, Sphere*);
Vec3 jb_vec3_sum (Vec3, Vec3);
void jb_fb_set_pixel (size_t, size_t, VESA_Colour, Framebuffer*);
void jb_juggle_balls (char*);
VESA_Colour jb_cast_ray (Ray*, Sphere_3_arr, Parallelepiped_0_arr, int);
Vec3_opt jb_ray_sphere_intersection_point (Ray*, Sphere*);
VESA_Colour jb_make_VESA_Colour (int, int, int, int);
size_t jb_get_width (Window*);
Vec3 jb_ray_parallelepiped_hit_normal (Ray*, Vec3, Parallelepiped*);
Vec3 jb_vec3_normalized (Vec3);
Vec3_opt jb_ray_parallelepiped_intersection (Ray*, Parallelepiped*);
Ray jb_camera_get_ray (PerspectiveCamera*, float, float);
Vec3 jb_vec3_scale (Vec3, float);
Vec3 jb_ray_at (Ray*, float);
void jb_juggle (Sphere*, int);

VESA_Colour jb_make_VESA_Colour(int r, int g, int b, int a) {
    VESA_Colour* colour = ((VESA_Colour_ptr)(0));

    VESA_Colour vc = (VESA_Colour){ .r = r, .g = g, .b = b, .a = a };
    colour = &vc;
    
    return *colour;
}








Vec3 jb_vec3_sum(Vec3 a, Vec3 b) {
    return ((Vec3){((float)(a.x)) + ((float)(b.x)), ((float)(a.y)) + ((float)(b.y)), ((float)(a.z)) + ((float)(b.z))});
}

Vec3 jb_vec3_scale(Vec3 v, float s) {
    return ((Vec3){((float)(v.x)) * ((float)(s)), ((float)(v.y)) * ((float)(s)), ((float)(v.z)) * ((float)(s))});
}

Vec3 jb_ray_at(Ray* ray, float t) {
    return jb_vec3_sum((*ray).origin, jb_vec3_scale((*ray).direction, t));
}

Vec3_opt jb_ray_sphere_intersection_point(Ray* ray, Sphere* sphere) {
    Vec3_opt vecnone = ((Vec3_opt) {0, 0});
    Vec3 oc = jb_vec3_sum((*ray).origin, jb_vec3_scale((*sphere).position, -1.));
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
    return ((Vec3_opt) {1, jb_ray_at(ray, t)});
}

Vec3 jb_ray_sphere_hit_normal(Ray* ray, Vec3 hit_point, Sphere* sphere) {
    Vec3 normal = jb_vec3_scale(jb_vec3_sum(hit_point, jb_vec3_scale((*sphere).position, -1.)), ((float)(1.)) / ((float)((*sphere).radius)));
    return (jb_vec3_dot(normal, (*ray).direction) > 0. ? ((Vec3){0., 0., 0.}) : normal);
}

Ray jb_camera_get_ray(PerspectiveCamera* cam, float u, float v) {
    Vec3 forward = jb_vec3_sum((*cam).lookat, jb_vec3_scale((*cam).eye, -1.));
    Vec3 right = jb_vec3_cross(forward, (*cam).up);
    Vec3 up = (*cam).up;
    Vec3 direction = jb_vec3_normalized(jb_vec3_sum(jb_vec3_sum(jb_vec3_scale(right, u), jb_vec3_scale(up, v)), forward));
    return ((Ray){(*cam).eye, direction});
}

float jb_vec3_dot(Vec3 a, Vec3 b) {
    return ((float)(((float)(((float)(a.x)) * ((float)(b.x)))) + ((float)(((float)(a.y)) * ((float)(b.y)))))) + ((float)(((float)(a.z)) * ((float)(b.z))));
}

Vec3 jb_vec3_cross(Vec3 a, Vec3 b) {
    return ((Vec3){((float)(((float)(a.y)) * ((float)(b.z)))) - ((float)(((float)(a.z)) * ((float)(b.y)))), ((float)(((float)(a.z)) * ((float)(b.x)))) - ((float)(((float)(a.x)) * ((float)(b.z)))), ((float)(((float)(a.x)) * ((float)(b.y)))) - ((float)(((float)(a.y)) * ((float)(b.x))))});
}

Vec3 jb_vec3_normalized(Vec3 v) {
    float len = fsqrt(((float)(((float)(((float)(v.x)) * ((float)(v.x)))) + ((float)(((float)(v.y)) * ((float)(v.y)))))) + ((float)(((float)(v.z)) * ((float)(v.z)))));
    if (len == 0.) {
        return ((Vec3){0., 0., 0.});
    } else {

    }
    return ((Vec3){((float)(v.x)) / ((float)(len)), ((float)(v.y)) / ((float)(len)), ((float)(v.z)) / ((float)(len))});
}

Vec3_opt jb_ray_parallelepiped_intersection(Ray* ray, Parallelepiped* para) {
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
    return ((Vec3_opt) {1, jb_ray_at(ray, t)});
}

Vec3 jb_ray_parallelepiped_hit_normal(Ray* ray, Vec3 hit_point, Parallelepiped* para) {
    float epsilon = 0.0001;
    if (fabs(((float)(hit_point.x)) - ((float)((*para).min.x))) < epsilon) {
        return ((Vec3){-1., 0., 0.});
    } else {
        if (fabs(((float)(hit_point.x)) - ((float)((*para).max.x))) < epsilon) {
            return ((Vec3){1., 0., 0.});
        } else {
            if (fabs(((float)(hit_point.y)) - ((float)((*para).min.y))) < epsilon) {
                return ((Vec3){0., -1., 0.});
            } else {
                if (fabs(((float)(hit_point.y)) - ((float)((*para).max.y))) < epsilon) {
                    return ((Vec3){0., 1., 0.});
                } else {
                    if (fabs(((float)(hit_point.z)) - ((float)((*para).min.z))) < epsilon) {
                        return ((Vec3){0., 0., -1.});
                    } else {
                        if (fabs(((float)(hit_point.z)) - ((float)((*para).max.z))) < epsilon) {
                            return ((Vec3){0., 0., 1.});
                        } else {

                        }
                    }
                }
            }
        }
    }
    return ((Vec3){0., 0., 0.});
}

uint32_t jb_hres = ((uint32_t)(400));

uint32_t jb_vres = ((uint32_t)(400));

float jb_pixel_size = 0.02;

int jb_num_rays = 5;

int jb_ticks = 0;

Framebuffer* jb_get_fb(Window* w) {
    Framebuffer* fb = ((Framebuffer_ptr)(0));

    fb = (*w).fb;
    
    return fb;
}

size_t jb_get_width(Window* w) {
    size_t width = ((size_t)(0));

    width = (*w).width;
    
    return width;
}

size_t jb_get_height(Window* w) {
    size_t height = ((size_t)(0));

    height = (*w).height;
    
    return height;
}

VESA_Colour jb_cast_ray(Ray* ray, Sphere_3_arr spheres, Parallelepiped_0_arr parallelepipeds, int max_bounces) {
    if (max_bounces <= 0) {
        return jb_make_VESA_Colour(0, 0, 0, 255);
    } else {

    }
    Vec3_opt hit = ((Vec3_opt) {0, 0});
    for (size_t i = ((size_t)(0)); i < 3; i = ((size_t)(i)) + ((size_t)(1))) {
        hit = jb_ray_sphere_intersection_point(ray, &spheres[((int)(i))]);
        if (hit.is_some) {
            Vec3 normal = jb_ray_sphere_hit_normal(ray, ((Vec3)hit.contents), &spheres[((int)(i))]);
            return jb_make_VESA_Colour(((int)(((float)(((float)((normal.x))) * ((float)(0.5)))) * ((float)(255.)))), ((int)(((float)(((float)((normal.y))) * ((float)(0.5)))) * ((float)(255.)))), ((int)(((float)(((float)((normal.z))) * ((float)(0.5)))) * ((float)(255.)))), 255);
        } else {

        }
    }
    for (size_t j = ((size_t)(0)); j < 0; j = ((size_t)(j)) + ((size_t)(1))) {
        hit = jb_ray_parallelepiped_intersection(ray, &parallelepipeds[((int)(j))]);
        if (hit.is_some) {
            Vec3 normal = jb_ray_parallelepiped_hit_normal(ray, ((Vec3)hit.contents), &parallelepipeds[((int)(j))]);
            return jb_make_VESA_Colour(((int)(((float)(((float)((normal.x))) * ((float)(0.5)))) * ((float)(255.)))), ((int)(((float)(((float)((normal.y))) * ((float)(0.5)))) * ((float)(255.)))), ((int)(((float)(((float)((normal.z))) * ((float)(0.5)))) * ((float)(255.)))), 255);
        } else {

        }
    }
    return jb_make_VESA_Colour(0, 0, 0, 255);
}

void jb_juggle(Sphere* sphere, int ticks) {
    float pos_x = ((float)(20.)) * ((float)(sin(((float)(((float)(ticks)))) * ((float)(0.1)))));
    float pos_y = fabs(((float)(20.)) * ((float)(cos(((float)(((float)(ticks)))) * ((float)(0.1))))));
    (*sphere).position = ((Vec3){pos_x, pos_y, (*sphere).position.z});
}

void jb_juggle_balls(char* s) {
    if (!_vell_is_active()) {
        printf("You need to run vell\n");
        return;
    } else {

    }
    Window* w = create_window_with_size("Juggling...", jb_hres, jb_vres);
    set_window_dirty(w);
    fb_draw_rect(10, 10, ((size_t)(jb_get_width(w))) - ((size_t)(((size_t)(10)))), ((size_t)(jb_get_height(w))) - ((size_t)(((size_t)(10)))), ((size_t)(1)), jb_make_VESA_Colour(255, 255, 255, 255), jb_get_fb(w));
    Sphere_3_arr spheres = {((Sphere){8., ((Vec3){-60., 0., -100.}), jb_make_VESA_Colour(255, 0, 0, 255)}), ((Sphere){8., ((Vec3){0., 0., -100.}), jb_make_VESA_Colour(0, 255, 0, 255)}), ((Sphere){8., ((Vec3){60., 0., -100.}), jb_make_VESA_Colour(0, 0, 255, 255)})};
    Parallelepiped_0_arr parallelepipeds = {};
    PerspectiveCamera camera = ((PerspectiveCamera){((Vec3){0., 0., 0.}), ((Vec3){0., 0., -1.}), ((Vec3){0., -1., 0.})});
    LightSource light_source = ((LightSource){((Vec3){5., 5., 0.}), 1., jb_make_VESA_Colour(255, 255, 255, 255)});
    float aspect_ratio = ((float)(((float)(jb_hres)))) / ((float)(((float)(jb_vres))));
    float fov = 60.;
    float fov_rad = ((float)(((float)(fov)) * ((float)(3.14159)))) / ((float)(180.));
    float scale = tan(((float)(fov_rad)) * ((float)(0.5)));
    while (true) {
        for (size_t y = ((size_t)(0)); y < jb_vres; y = ((size_t)(y)) + ((size_t)(1))) {
            for (size_t x = ((size_t)(0)); x < jb_hres; x = ((size_t)(x)) + ((size_t)(1))) {
                float x_norm = ((float)(((float)((((float)(((float)(x)))) / ((float)(((float)(jb_hres))))))) * ((float)(2.)))) - ((float)(1.));
                float y_norm = ((float)(((float)((((float)(((float)(y)))) / ((float)(((float)(jb_vres))))))) * ((float)(2.)))) - ((float)(1.));
                float u = ((float)(((float)(x_norm)) * ((float)(aspect_ratio)))) * ((float)(scale));
                float v = ((float)(y_norm)) * ((float)(scale));
                Ray ray = jb_camera_get_ray(&camera, u, v);
                VESA_Colour colour = jb_cast_ray(&ray, spheres, parallelepipeds, 5);
                jb_fb_set_pixel(x, y, colour, jb_get_fb(w));
            }
        }
        set_window_dirty(w);
        jb_juggle(&spheres[1], jb_ticks);
        jb_juggle(&spheres[0], ((int)(jb_ticks)) + ((int)(20)));
        jb_juggle(&spheres[2], ((int)(jb_ticks)) + ((int)(40)));
        sleep(1);
        jb_ticks = ((int)(jb_ticks)) + ((int)(1));
    }
}

void jb_fb_set_pixel(size_t x, size_t y, VESA_Colour colour, Framebuffer* fb) {

    if (x < (*fb).width && y < (*fb).height) {
        size_t index = y * (*fb).width + x;
        (*fb).fb[index] = colour.val;
    }
    
}
#endif