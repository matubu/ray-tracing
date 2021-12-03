#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <mlx.h>
//TODO pass more things by reference

//DEBUG LIBS
#include <time.h>
#include <stdio.h>

//MATH CONSTANT
#define	PI					3.1415926535
#define EPSILON				0.0000001

//CAMERA
#define	WIDTH				1000
#define	HEIGHT				1000
//#define	WIDTH				960
//#define	HEIGHT				540
//#define	WIDTH				1920
//#define	HEIGHT				1080
#define	FIELD_OF_VIEW		PI / 2
#define	SAMPLING			2
#define	CAMERA_CLIP_START	.001

//COLORS
#define	RED					(t_color){ 245, 152, 175 }
#define	GREEN				(t_color){ 162, 250, 192 }
#define	BLUE				(t_color){ 174, 201, 240 }
#define	WHITE				(t_color){ 255, 255, 255 }
#define	BLACK				(t_color){ 80 , 77 , 94  }

//VECTOR
#define	ORIGIN				(t_vec){ 0  , 0  , 0   }

//SHADOW
#define	SHADOW_DIFFUSENESS	.01

typedef struct s_vec {
	float	x;
	float	y;
	float	z;
}	t_vec;

typedef struct s_tris {
	t_vec	*a;
	t_vec	*b;
	t_vec	*c;
	t_vec	normal;
}	t_tris;

typedef struct s_color {
	unsigned int	r;
	unsigned int	g;
	unsigned int	b;
}	t_color;

typedef enum e_obj_type {
	END_OBJ    = 0,
	SPHERE_OBJ = 1,
	LINE_OBJ   = 2,
	TRIS_OBJ   = 3,
	QUAD_OBJ   = 4
}	t_obj_type;

typedef struct s_obj {
	t_obj_type	type;
	t_color		color;
	void		*data;
}	t_obj;

typedef struct s_camera {
	unsigned int	width;
	unsigned int	height;
	float			aspect_ratio;
	t_vec			pos;
	t_vec			rot;// euler radian
	float			fovx;// field of view
	float			fovy;
	float			fov_pixel;// field of view of a pixel
}	t_camera;

typedef struct s_light {
	t_vec	pos;
	t_color	color;
	float	intensity;
}	t_light;

typedef struct s_scene {
	t_camera		*camera;
	t_color			ambient_color;
	unsigned int	obj_count;
	unsigned int	lights_count;
	t_obj			*obj;
	t_light 		*lights;
}	t_scene;

typedef struct s_hit {
	float	dist;
	t_vec	pos;
	t_vec	normal;
	t_obj	*obj;
}	t_hit;

float	dot(t_vec *a, t_vec *b)
{
	return (a->x * b->x + a->y * b->y + a->z * b->z);
}

t_vec	sub(t_vec *a, t_vec *b) 
{
	return ((t_vec){ a->x - b->x, a->y - b->y, a->z - b->z });
}

t_vec	mult(t_vec *a, float fac)
{
	return ((t_vec){ a->x * fac, a->y * fac, a->z * fac });
}

t_vec	add3(t_vec *a, t_vec *b, t_vec *c)
{
	return ((t_vec){ a->x + b->x + c->x, a->y + b->y + c->y, a->z + b->z + c->z });
}

t_vec	cross(t_vec a, t_vec b)
{
	return ((t_vec){ a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x });
}

t_color	mult_color(t_color *a, float fac)
{
	t_color	color;

	color.r = a->r * fac;
	color.g = a->g * fac;
	color.b = a->b * fac;
	return (color);
}

t_color	mix_color(t_color *a, t_color *b, float fac)
{
	t_color	color;

	if (fac > 1.0)
		fac = 1.0;
	else if (fac < 0.0)
		fac = 0.0;
	float	facb = 1 - fac;
	color.r = a->r * fac + b->r * facb;
	color.g = a->g * fac + b->g * facb;
	color.b = a->b * fac + b->b * facb;
	return (color);
}

// https://en.wikipedia.org/wiki/Fast_inverse_square_root
float	q_rsqrt(float y)
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

t_vec	normalize(t_vec vec)
{
	float	fac = q_rsqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);

	vec.x *= fac;
	vec.y *= fac;
	vec.z *= fac;
	return (vec);
}

//euler radian to vector
t_vec	etov(t_vec *rot)
{
	t_vec	vec;
	float	sin_z = sinf(rot->z),
			cos_z = cosf(rot->z),
			sin_y = sinf(rot->y),
			cos_y = cosf(rot->y),
			sin_x = sinf(rot->x),
			cos_x = cosf(rot->x);

	vec.x = -cos_z * sin_y * sin_x - sin_z * cos_x;
	vec.y = -sin_z * sin_y * sin_x + cos_z * cos_x;
	vec.z =  cos_y * sin_x;
	return (vec);
}

int	ray_tris(t_vec *orig, t_vec *ray, t_tris *tris, t_vec *intersect, float *t)
{
	t_vec	ea = sub(tris->b, tris->a);// vector from b to a
	t_vec	eb = sub(tris->c, tris->a);// vector from c to a
	t_vec	pvec, dist, qvec;
	float	det, idet;

	pvec = cross(*ray, eb);
	det = dot(&ea, &pvec);
	if (det > -EPSILON && det < EPSILON)
		return (0);// ray and tris are parallel
	idet = 1.0 / det;
	dist = sub(orig, tris->a); // dist between origin and point a
	//U
	float	u = dot(&dist, &pvec) * idet;
	if (u < 0.0 || u > 1.0)
		return (0);
	//V
	qvec = cross(dist, ea);

	float	v = dot(ray, &qvec) * idet;
	if (v < 0.0 || u + v > 1.0)
		return (0);
	//T	
	*t = dot(&eb, &qvec) * idet;
	intersect->x = orig->x + ray->x * *t;
	intersect->y = orig->y + ray->y * *t;
	intersect->z = orig->z + ray->z * *t;

	return (1);
}

int	ray_scene(t_vec *orig, t_vec *ray, t_scene *scene, t_hit *closest)
{
	t_hit			hit;
	register t_obj	*obj = scene->obj;

	closest->dist = -1;
	while (obj->type)
	{
		hit.dist = -1;
		if (obj->type == TRIS_OBJ)
		{
			hit.normal = ((t_tris *)obj->data)->normal;
			if (!ray_tris(orig, ray, (t_tris *)obj->data, &hit.pos, &hit.dist))
				hit.dist = -1;
		}
		if (hit.dist > CAMERA_CLIP_START && (closest->dist == -1 || hit.dist < closest->dist))
		{
			closest->dist = hit.dist;
			closest->pos = hit.pos;
			closest->normal = hit.normal;
			closest->obj = obj;
		}
		obj++;
	}
	if (closest->dist == -1)
		return (0);
	return (1);
}

int	rgb(t_color color)
{
	return (((color.r & 255) << 16) | ((color.g & 255) << 8) | (color.b & 255));
}

int	rgbmult(int color, float fac)
{
	return (
		(int)((float)((color & (255 << 16)) >> 16) * fac) << 16
		| (int)((float)((color & (255 << 8)) >> 8) * fac) << 8
		| (int)((float)((color & (255 << 0)) >> 0) * fac) << 0
	);
}

t_vec	*rand_vec(float fac)
{
	static t_vec	vec;

	vec.x = (float)rand() * fac;
	vec.y = (float)rand() * fac;
	vec.z = (float)rand() * fac;
	return (&vec);
}

int	ray_scene_color(t_vec *orig, t_vec *ray, t_scene *scene)
{
	t_hit	cam_hit, light_hit;
	t_vec	cam_to_light = normalize(sub(&scene->lights[0].pos, orig));
	t_vec	hit_to_light;

	if (!ray_scene(orig, ray, scene, &cam_hit))
		return (rgb(scene->ambient_color));
	//TODO light reduce by distance + multi light + color disruption
	hit_to_light = normalize(sub(&scene->lights[0].pos, &cam_hit.pos));
	hit_to_light = sub(&hit_to_light, rand_vec(SHADOW_DIFFUSENESS / (float)RAND_MAX));
	if (ray_scene(&cam_hit.pos, &hit_to_light, scene, &light_hit))
		return (rgb(scene->ambient_color));
	return (rgb(mult_color(&cam_hit.obj->color, fabsf(dot(&cam_to_light, &cam_hit.normal)) * .4 + .6)));
}

typedef struct s_mlx_data {
	void	*ptr;
	void	*win;
	void	*img;
	int		*buf;
	int		width;
	int		height;
}	t_mlx_data;

void	render(t_scene *scene, t_mlx_data *mlx, int i)
{

	clock_t	start = clock();

	register unsigned int		j = 0;
	t_vec	dir = etov(&scene->camera->rot);
	t_vec	cv_right = normalize(cross(dir, (t_vec){0, 0, 1}));
	t_vec	cv_up = normalize(cross(cv_right, dir));
	float	half_x = scene->camera->width / 2.0;
	float	half_y = scene->camera->height / 2.0;
	t_vec	ray;

	register int	y = scene->camera->height;
	register int	x;
	while (y--)
	{
		t_vec	yr = mult(&cv_up, (y - half_y) * scene->camera->fov_pixel);
		x = -1;
		while ((unsigned int)++x < scene->camera->width)
		{
			t_vec	xr = mult(&cv_right, (x - half_x) * scene->camera->fov_pixel);
			ray = normalize(add3(&dir, &xr, &yr));
			mlx->buf[j] = 
				rgbmult(ray_scene_color(&scene->camera->pos, &ray, scene), 1.0 / (float)i)
				+ rgbmult(mlx->buf[j], (float)(i - 1) / (float)i);
			j++;
		}
	}
	mlx_put_image_to_window(mlx->ptr, mlx->win, mlx->img, 0, 0);

	printf("iteration %i took %.2fms ⚡\n", i, (double)(clock() - start) / CLOCKS_PER_SEC * 1000);
}

t_camera	create_camera(unsigned int width, unsigned int height, 
		t_vec pos, t_vec rot, float fov)
{
	t_camera	camera;

	camera.width = width;
	camera.height = height;
	camera.aspect_ratio = width / height;
	camera.pos = pos;
	camera.rot = rot;
	camera.fovx = fov;
	camera.fovy = fov / width * height;
	camera.fov_pixel = fov / width;
	return (camera);
}

t_scene	create_scene(t_camera *camera, t_color ambient_color, t_obj *obj, t_light *lights)
{
	t_scene	scene;

	scene.camera = camera;
	scene.ambient_color = ambient_color;
	scene.obj = obj;
	scene.obj_count = 0;
	while (obj++->type)
		scene.obj_count++;
	scene.lights = lights;
	return (scene);
}
/*
void	denoise(t_mlx_data *mlx)
{
	clock_t	start = clock();

	int	x;
	int	y;
	int	j;

	y = 0;
	while (++y < mlx->height - 1)
	{
		x = 0;
		while (++x < mlx->width - 1)
		{
			j = y * mlx->width + x;
			mlx->buf[j] = 
				rgbmult(mlx->buf[j - 1 - mlx->width], 1.0 / 16.0)
				+ rgbmult(mlx->buf[j - mlx->width], 2.0 / 16.0)
				+ rgbmult(mlx->buf[j + 1 - mlx->width], 1.0 / 16.0)
				+ rgbmult(mlx->buf[j - 1], 2.0 / 16.0)
				+ rgbmult(mlx->buf[j], 4.0 / 16.0)
				+ rgbmult(mlx->buf[j + 1], 2.0 / 16.0)
				+ rgbmult(mlx->buf[j - 1 + mlx->width], 1.0 / 16.0)
				+ rgbmult(mlx->buf[j + mlx->width], 2.0 / 16.0)
				+ rgbmult(mlx->buf[j + 1 + mlx->width], 1.0 / 16.0);
		}
	}
	mlx_put_image_to_window(mlx->ptr, mlx->win, mlx->img, 0, 0);
	printf("denoising ended after %.2fms 🌈\n", (double)(clock() - start) / CLOCKS_PER_SEC * 1000);
}
*/
int	main()
{
	t_mlx_data	mlx;
	int			null;

	t_camera camera = create_camera(WIDTH, HEIGHT,
			(t_vec){ 5, -4, 5.5 }, (t_vec){ -PI / 4, 0, PI / 4 }, FIELD_OF_VIEW);
	t_vec	points[] = {
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
	};
	t_obj	objects[] = {
		{ TRIS_OBJ, WHITE, (void *)(tris + 0)  },
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

		{ TRIS_OBJ, WHITE, (void *)(tris + 13) },
		{ TRIS_OBJ, WHITE, (void *)(tris + 14) },
		{ 0, BLACK, NULL }
	};
	t_light	lights[] = {
		{ { 5, 5, 5 }, { 255, 0, 0 }, 2 }
	};
	t_scene	scene = create_scene(&camera, BLACK, objects, lights);

	mlx.ptr = mlx_init();
	if (mlx.ptr == NULL)
		return (1);
	mlx.width = WIDTH;
	mlx.height = HEIGHT;
	mlx.win = mlx_new_window(mlx.ptr, mlx.width, mlx.height, "MINIRT");
	if (mlx.win == NULL)
		return (1);
	mlx.img = mlx_new_image(mlx.ptr, mlx.width, mlx.height);
	mlx.buf = (int *)mlx_get_data_addr(mlx.img, &null, &null, &null);

	int	i = 0;
	while (i++ < SAMPLING)
		render(&scene, &mlx, i);

	//denoise(&mlx);

	mlx_loop(mlx.ptr);
	return (0);
}
