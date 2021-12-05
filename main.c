#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <mlx.h>

//DEBUG LIBS
#include <time.h>
#include <stdio.h>

//MATH CONSTANT
#define	PI						3.1415926535
#define EPSILON					0.0001

//CAMERA
#define	WIDTH					1000
#define	HEIGHT					1000
#define	FIELD_OF_VIEW			PI / 2
#define	CAMERA_CLIP_START		.001

//COLORS
#define	RED						0xf598af
#define	GREEN					0xa2fac0
#define	BLUE					0xaec9f0
#define	WHITE					0xeeeeee
#define	BLACK					0x191a1f
#define	GREY					0x504d5e

//VECTOR
#define	ORIGIN					(t_vec){ 0, 0, 0 }
#define	UP						(t_vec){ 0, 0, 1 }

typedef struct s_vec {
	float	x;
	float	y;
	float	z;
}	t_vec;
/*
typedef struct s_tris {
	t_vec	*a;
	t_vec	*b;
	t_vec	*c;
	t_vec	normal;
}	t_tris;
*/
typedef struct s_sphere {
	t_vec	pos;
	float	rad;
}	t_sphere;

typedef enum e_obj_type {
	END_OBJ    = 0,
	SPHERE_OBJ = 1,
//	LINE_OBJ   = 2,
//	TRIS_OBJ   = 3,
//	QUAD_OBJ   = 4
}	t_obj_type;

typedef struct s_obj {
	t_obj_type	type;
	int			color;
	void		*data;
}	t_obj;

typedef struct s_camera {
	t_vec	pos;
	t_vec	rot;// euler radian
	int		width;
	int		height;
	float	fov_pixel;// field of view of a pixel
}	t_camera;

typedef struct s_light {
	t_vec	pos;
	float	intensity;
	int		color;
}	t_light;

typedef struct s_scene {
	t_camera		*camera;
	t_obj			*obj;
	t_light 		*lights;
	int				ambient_color;
}	t_scene;

typedef struct s_hit {
	t_vec	pos;
	t_vec	normal;
	float	dist;
	t_obj	*obj;
}	t_hit;

typedef struct s_mlx_data {
	void	*ptr;
	void	*win;
	void	*img;
	int		*buf;
}	t_mlx_data;

inline float	dot(t_vec *a, t_vec *b)
{
//	asm("dpps %xmm1, %xmm2, 11100000b" : "=a" (ret) : "a" (a) , "b" (b));
	return (a->x * b->x + a->y * b->y + a->z * b->z);
}

inline t_vec	sub(t_vec *a, t_vec *b) 
{
//	asm("subps");
	return ((t_vec){ a->x - b->x, a->y - b->y, a->z - b->z });
}

inline t_vec	mult(t_vec *a, float fac)
{
//	asm();
	return ((t_vec){ a->x * fac, a->y * fac, a->z * fac });
}

inline t_vec	add3(t_vec *a, t_vec *b, t_vec *c)
{
//	asm("addps");
	return ((t_vec){ a->x + b->x + c->x, a->y + b->y + c->y, a->z + b->z + c->z });
}

inline t_vec	cross(t_vec *a, t_vec *b)
{
//	asm("VFMSUB132SS/VFMSUB213SS/VFMSUB231SS");
	return ((t_vec){ a->y * b->z - a->z * b->y, a->z * b->x - a->x * b->z, a->x * b->y - a->y * b->x });
}

// https://en.wikipedia.org/wiki/Fast_inverse_square_root
inline float	q_rsqrt(float y)
{
	int			i;
	float		x2;
	const float	threehalfs = 1.5F;

	x2 = y * 0.5F;
	i = *(int *)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float *)&i;
	y = y * (threehalfs - (x2 * y * y));
	return (y);
}

static inline t_vec	normalize(t_vec vec)
{
	return (mult(&vec, q_rsqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z)));
}

inline int	rgbmult(int	color, int fac)
{
	return (
		((((color & (255 << 16)) * fac) & (255 << 24))
		| (((color & (255 << 8)) * fac) & (255 << 16))
		| (((color & (255 << 0)) * fac) & (255 << 8))) >> 8
	);
}

//euler radian to vector
inline t_vec	etov(t_vec *rot)
{
	float	sin_z = sinf(rot->z),
			cos_z = cosf(rot->z),
			sin_y = sinf(rot->y),
			sin_x = sinf(rot->x),
			cos_x = cosf(rot->x);

	return ((t_vec){
		-cos_z * sin_y * sin_x - sin_z * cos_x,
		-sin_z * sin_y * sin_x + cos_z * cos_x,
		cosf(rot->y) * sin_x
	});
}

//https://facultyweb.cs.wwu.edu/~wehrwes/courses/csci480_21w/lectures/L07/L07_notes.pdf
inline int	ray_sphere(t_vec *orig, t_vec *ray, t_sphere *sphere, t_hit *hit)
{
	hit->normal = (t_vec){0, 0, 0};
	hit->pos = (t_vec){0, 0, 0};
	hit->dist = 0;
	return (1);
}


//TODO fixme too slow
/*inline int	ray_tris(t_vec *orig, t_vec *ray, t_tris *tris, t_vec *intersect, float *t)
{
	t_vec	ba = sub(tris->b, tris->a);// vector from b to a
	t_vec	ca = sub(tris->c, tris->a);// vector from c to a
	t_vec	pvec, dist, qvec;
	float	det;

	pvec = cross(ray, &ca);
	det = dot(&ba, &pvec);
	if (det > -EPSILON && det < EPSILON)
		return (0);// ray and tris are parallel
	det = 1.0 / det;
	dist = sub(orig, tris->a); // dist between origin and point a
	//U
	float	u = dot(&dist, &pvec) * det;
	if (u < 0.0 || u > 1.0)
		return (0);
	//V
	qvec = cross(&dist, &ba);
	float	v = dot(ray, &qvec) * det;
	if (v < 0.0 || u + v > 1.0)
		return (0);
	//T	
	*t = dot(&ca, &qvec) * det;
	intersect->x = orig->x + ray->x * *t;
	intersect->y = orig->y + ray->y * *t;
	intersect->z = orig->z + ray->z * *t;
	return (1);
}*/

inline int	ray_scene(t_vec *orig, t_vec *ray, t_scene *scene, t_hit *closest)
{
	t_hit			hit;
	register t_obj	*obj = scene->obj;

	closest->dist = -1;
	while (obj->type)
	{
		hit.dist = -1;
		if (obj->type == SPHERE_OBJ)
		{
			if (!ray_sphere(orig, ray, (t_sphere *)obj->data, &hit))
				hit.dist = -1;
		}
/*		if (obj->type == TRIS_OBJ)
		{
			hit.normal = ((t_tris *)obj->data)->normal;
			if (!ray_tris(orig, ray, (t_tris *)obj->data, &hit.pos, &hit.dist))
				hit.dist = -1;
		}*/
		if (hit.dist > CAMERA_CLIP_START && (closest->dist == -1 || hit.dist < closest->dist))
		{
			closest->dist = hit.dist;
			closest->pos = hit.pos;
			closest->normal = hit.normal;
			closest->obj = obj;
		}
		obj++;
	}
	return (closest->dist != -1);
}

int	ray_scene_color(t_vec *orig, t_vec *ray, t_scene *scene)
{
	t_hit	cam_hit, light_hit;
	t_vec	cam_to_light;
	t_vec	hit_to_light;

	if (!ray_scene(orig, ray, scene, &cam_hit))
		return (scene->ambient_color);
	//TODO law of light + multi light + color disruption
	hit_to_light = sub(&scene->lights->pos, &cam_hit.pos);
	if (ray_scene(&cam_hit.pos, &hit_to_light, scene, &light_hit)) //TODO check hit distance
		return (scene->ambient_color);
	cam_to_light = normalize(sub(&scene->lights->pos, orig));
	return (rgbmult(cam_hit.obj->color, 240 - abs((int)(dot(&cam_to_light, &cam_hit.normal) * 120.0))));
}


void	render(t_scene *scene, int *buf)
{
	clock_t	start = clock();

	t_vec	up = UP;
	t_vec	dir = etov(&scene->camera->rot);
	t_vec	cv_right = normalize(cross(&dir, &up));
	t_vec	cv_up = normalize(cross(&cv_right, &dir));
	float	half_x = scene->camera->width / 2.0;
	float	half_y = scene->camera->height / 2.0;
	t_vec	ray;
	t_vec	yr, xr;

	register int	y = scene->camera->height;
	register int	x;
	while (y--)
	{
		yr = mult(&cv_up, (y - half_y) * scene->camera->fov_pixel);
		x = scene->camera->width;
		while (x--)
		{
			xr = mult(&cv_right, (half_x - x) * scene->camera->fov_pixel);
			ray = add3(&dir, &xr, &yr);
			*buf++ = ray_scene_color(&scene->camera->pos, &ray, scene);
		}
	}

	printf("rendering took %.2fms\n", (double)(clock() - start) / CLOCKS_PER_SEC * 1000);
}

t_camera	create_camera(int width, int height, 
		t_vec pos, t_vec rot, float fov)
{
	t_camera	camera;

	camera.width = width;
	camera.height = height;
	camera.pos = pos;
	camera.rot = rot;
	camera.fov_pixel = fov / width;
	return (camera);
}

t_scene	create_scene(t_camera *camera, int ambient_color, t_obj *obj, t_light *lights)
{
	t_scene	scene;

	scene.camera = camera;
	scene.ambient_color = ambient_color;
	scene.obj = obj;
	scene.lights = lights;
	return (scene);
}

int	main()
{
	t_mlx_data	mlx;
	int			null;

	t_camera camera = create_camera(WIDTH, HEIGHT,
			(t_vec){ 5, -4, 5.5 }, (t_vec){ -PI / 4, 0, PI / 4 }, FIELD_OF_VIEW);
/*	t_vec	points[] = {
		{  1,  1,  1  },
		{  1,  1, -1  },
		{  1, -1, -1  },
		{  1, -1,  1  },
		{ -1,  1,  1  },
		{ -1,  1, -1  },
		{ -1, -1, -1  },
		{ -1, -1,  1  },

		{ 2, -1,  1  },

		{ 100, 100, -3 },
		{ 100, -100, -3 },
		{ -100, -100, -3 },
		{ -100, 100, -3 },
	};
	t_tris	tris[] = {
		{ points + 0, points + 1, points + 2, {  1,  0,  0  } },//front x
		{ points + 0, points + 3, points + 2, {  1,  0,  0  } },
		{ points + 4, points + 5, points + 6, { -1,  0,  0  } },//back x
		{ points + 4, points + 7, points + 6, { -1,  0,  0  } },
		{ points + 4, points + 5, points + 1, {  0,  1,  0  } },//front y
		{ points + 4, points + 0, points + 1, {  0,  1,  0  } },
		{ points + 7, points + 6, points + 2, {  0, -1,  0  } },//back y
		{ points + 7, points + 3, points + 2, {  0, -1,  0  } },
		{ points + 7, points + 4, points + 0, {  0,  0,  1  } },//front z
		{ points + 7, points + 3, points + 0, {  0,  0,  1  } },
		{ points + 6, points + 5, points + 1, {  0,  0, -1  } },//back z
		{ points + 6, points + 2, points + 1, {  0,  0, -1  } },

		{ points + 8, points + 2, points + 3, {  0,  -1, 0  } },

		{ points + 9, points + 10, points + 11, {  0,  0, 1  } },
		{ points + 11, points + 12, points + 9, {  0,  0, 1  } },
	};*/
	t_sphere	sphere[] = {
		{ (t_vec){ 0.0, 0.0, 1.0 }, 5.0 }
	};
	t_obj	objects[] = {
		{ SPHERE_OBJ, BLUE, (void *)(sphere + 0) },
		/*{ TRIS_OBJ, WHITE, (void *)(tris + 0)  },
		{ TRIS_OBJ, BLUE, (void *)(tris + 1)  },
		{ TRIS_OBJ, WHITE, (void *)(tris + 2)  },
		{ TRIS_OBJ, WHITE, (void *)(tris + 3)  },
		{ TRIS_OBJ, GREEN, (void *)(tris + 4)  },
		{ TRIS_OBJ, WHITE, (void *)(tris + 5)  },
		{ TRIS_OBJ, WHITE, (void *)(tris + 6)  },
		{ TRIS_OBJ, WHITE, (void *)(tris + 7)  },
		{ TRIS_OBJ, RED, (void *)(tris + 8)  },
		{ TRIS_OBJ, WHITE, (void *)(tris + 9)  },
		{ TRIS_OBJ, WHITE, (void *)(tris + 10) },
		{ TRIS_OBJ, WHITE, (void *)(tris + 11) },

		{ TRIS_OBJ, GREEN, (void *)(tris + 12) },

		{ TRIS_OBJ, GREEN, (void *)(tris + 13) },
		{ TRIS_OBJ, GREEN, (void *)(tris + 14) },*/
		{ 0, BLACK, NULL }
	};
	t_light	lights[] = {
		{ { 5, 5, 5 }, RED, 2 }
	};
	t_scene	scene = create_scene(&camera, GREY, objects, lights);

	mlx.ptr = mlx_init();
	if (mlx.ptr == NULL)
		return (1);
	mlx.win = mlx_new_window(mlx.ptr, WIDTH, HEIGHT, "MINIRT");
	if (mlx.win == NULL)
		return (1);
	mlx.img = mlx_new_image(mlx.ptr, WIDTH, HEIGHT);
	mlx.buf = (int *)mlx_get_data_addr(mlx.img, &null, &null, &null);

	render(&scene, mlx.buf);
	mlx_put_image_to_window(mlx.ptr, mlx.win, mlx.img, 0, 0);

	mlx_loop(mlx.ptr);
	return (0);
}
