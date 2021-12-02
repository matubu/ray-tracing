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

//COUNT
#define	POINTS_COUNT		300
#define TRIS_COUNT			100
#define	OBJ_COUNT			100
#define LIGHT_COUNT			10

//CAMERA
#define	WIDTH				500
#define	HEIGHT				500
#define	CAMERA_FOCAL_LENGTH	1.0
#define	CAMERA_POSITION		{ 3, -4, 2 }
#define	CAMERA_ROTATION		{ -.2, 0, .5 }
#define	FIELD_OF_VIEW		PI / 2

//PERFORMANCE / SYSTEM
#define	THREAD_COUNT		16

//COLORS
#define	RED					{ 255, 0  , 0   }
#define	GREEN				{ 0  , 255, 0   }
#define	BLUE				{ 0  , 0  , 255 }
#define	WHITE				{ 255, 255, 255 }
#define	BLACK				{ 0  , 0  , 0   }

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
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
}	t_color;

typedef enum e_obj_type {
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

int		abs(int v)
{
	return v < 0 ? -v : v;
}

float	absf(float v)
{
	return v < 0 ? -v : v;
}

float	dot(t_vec a, t_vec b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

t_vec	sub(t_vec a, t_vec b) 
{
	t_vec	r = { a.x - b.x, a.y - b.y, a.z - b.z };
	return r;
}

t_vec	mult(t_vec a, float fac)
{
	t_vec	r = { a.x * fac, a.y * fac, a.z * fac };
	return r;
}

t_vec	add3(t_vec a, t_vec b, t_vec c)
{
	t_vec r = { a.x + b.x + c.x, a.y + b.y + c.y, a.z + b.z + c.z };
	return r;
}

t_vec	cross(t_vec a, t_vec b)
{
	t_vec r = { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
	return r;
}

t_color	mult_color(t_color *a, float fac)
{
	t_color	color;

	color.r = a->r * fac;
	color.g = a->g * fac;
	color.b = a->b * fac;
	return color;
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
	return color;
}

t_color	mix_color4(t_color a, t_color b, t_color c, t_color d)
{
	t_color	color;
	color.r = (a.r + b.r + c.r + d.r) >> 2;
	color.g = (a.g + b.g + c.g + d.g) >> 2;
	color.b = (a.b + b.b + c.b + d.b) >> 2;
	return color;
}

// https://en.wikipedia.org/wiki/Fast_inverse_square_root
float	q_rsqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;    // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
	return y;
}

t_vec	normalize(t_vec vec)
{
	float	fac = q_rsqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);

	vec.x *= fac;
	vec.y *= fac;
	vec.z *= fac;
	return (vec);
}

float	sinf(float	v)
{
	return (float)sin((double)v);
}

float	cosf(float	v)
{
	return (float)cos((double)v);
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
	return vec;
}

t_vec	rand_vec(float fac)
{
	t_vec	vec;

	vec.x = (float)rand() * fac;
	vec.y = (float)rand() * fac;
	vec.z = (float)rand() * fac;
	return vec;
}

int	ray_tris(t_vec *orig, t_vec *ray, t_tris *tris, t_vec *intersect, float *t)
{
	t_vec	ea = sub(*tris->b, *tris->a);// vector from b to a
	t_vec	eb = sub(*tris->c, *tris->a);// vector from c to a
	t_vec	pvec, dist, qvec;
	float	det, idet;

	pvec = cross(*ray, eb);
	det = dot(ea, pvec);
	if (det > -EPSILON && det < EPSILON)
		return (0);// ray and tris are parallel
	idet = 1.0 / det;
	dist = sub(*orig, *tris->a); // dist between origin and point a
	//U
	float	u = dot(dist, pvec) * idet;
	if (u < 0.0 || u > 1.0)
		return (0);
	//V
	qvec = cross(dist, ea);

	float	v = dot(*ray, qvec) * idet;
	if (v < 0.0 || u + v > 1.0)
		return (0);
	//T	
	*t = dot(eb, qvec) * idet;
	intersect->x = orig->x + ray->x * *t;
	intersect->y = orig->y + ray->y * *t;
	intersect->z = orig->z + ray->z * *t;

	return (1);
}

int	ray_scene(t_vec *orig, t_vec *ray, t_scene *scene, t_hit *closest)
{
	t_hit	hit;
	t_obj	*obj = scene->obj;

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
		if (hit.dist != -1 && (closest->dist == -1 || hit.dist < closest->dist))
		{
			closest->dist = hit.dist;
			closest->pos = hit.pos;
			closest->normal = hit.normal;
			closest->obj = obj;
		}
		obj++;
	}
	if (closest->dist == -1)
		return 0;
	return 1;
}

t_color	ray_scene_color(t_vec *orig, t_vec ray, t_scene *scene/*, float rand_fac*/)
{
	t_color	color;
	t_hit	hit;
	//ray = sub(ray, rand_vec(rand_fac));
	
	while (ray_scene(orig, &ray, scene, &hit))
	{
		t_vec	dir = normalize(sub(scene->lights[0].pos, hit.pos));
		float	fac = 0;
		t_hit	light_dir_hit;
		ray_scene(&hit.pos, &dir, scene, &light_dir_hit);
		if (ray_scene(&hit.pos, &dir, scene, &light_dir_hit))//TODO check hit dist
		//	printf();
		//{}
		//else
			fac = absf(dot(dir, hit.normal)) * .1 + .9;//TODO distance relative too
		//TODO take object color too
		//color = mix_color(&scene->lights[0].color, &hit.obj->color, 1 / scene->lights[0].intensity);//TODO fix that
		color = mult_color(&color, fac);
		return color;
	}
	return scene->ambient_color;
}

int	rgb(t_color color)
{
	return (
		((color.r & 255) << 16)
		| ((color.g & 255) << 8)
		| (color.b & 255)
	);
}

typedef struct s_mlx_data {
	void	*ptr;
	void	*win;
	void	*img;
	int		*buf;
	int		width;
	int		height;
}	t_mlx_data;

//TODO antialiasing
void	render(t_scene *scene, t_mlx_data *mlx)
{
	unsigned int		j = 0;

	t_vec	dir = etov(&scene->camera->rot);
	t_vec	cv_right = normalize(cross(dir, (t_vec){0, 0, 1}));
	t_vec	cv_up = normalize(cross(cv_right, dir));
	float	half_x = scene->camera->width / 2.0 - .5;
	float	half_y = scene->camera->height / 2.0 - .5;
	t_vec	ray;
	//t_vec	ray_cp;
	//float	rand_fac = scene->camera->fov_pixel / (float)(RAND_MAX);

	for (unsigned int y = scene->camera->height; y-- > 0;)
	{
		t_vec	yr = mult(cv_up, (y - half_y) * scene->camera->fov_pixel);
		for (unsigned int x = 0; x < scene->camera->width; x++)
		{
			t_vec	xr = mult(cv_right, (x - half_x) * scene->camera->fov_pixel);
			ray = normalize(add3(dir, xr, yr));
			mlx->buf[j] = rgb(ray_scene_color(&scene->camera->pos, ray, scene));
			j++;
		}
	}
	mlx_put_image_to_window(mlx->ptr, mlx->win, mlx->img, 0, 0);
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
	return camera;
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
	return scene;
}

int	main()
{
	t_mlx_data	mlx;
	int			null;

	t_camera camera = create_camera(WIDTH, HEIGHT,
			(t_vec)CAMERA_POSITION, (t_vec)CAMERA_ROTATION, FIELD_OF_VIEW);
	t_vec	points[POINTS_COUNT] = {
		{ 1, 1, 1    },
		{ 1, 1, -1   },
		{ 1, -1, -1  },
		{ 1, -1, 1   },
		{ -1, 1, 1   },
		{ -1, 1, -1  },
		{ -1, -1, -1 },
		{ -1, -1, 1  }
	};
	t_tris	tris[TRIS_COUNT] = {
		{ points + 0, points + 1, points + 2, { 1, 0, 0  } },//front x
		{ points + 0, points + 3, points + 2, { 1, 0, 0  } },
		{ points + 4, points + 5, points + 6, { -1, 0, 0 } },//back x
		{ points + 4, points + 8, points + 6, { -1, 0, 0 } },
		{ points + 4, points + 5, points + 1, { 0, 1, 0  } },//front y
		{ points + 4, points + 0, points + 1, { 0, 1, 0  } },
		{ points + 7, points + 6, points + 2, { 0, -1, 0 } },//back y
		{ points + 7, points + 3, points + 2, { 0, -1, 0 } },
		{ points + 7, points + 4, points + 0, { 0, 0, 1  } },//front z
		{ points + 7, points + 3, points + 0, { 0, 0, 1  } },
		{ points + 6, points + 5, points + 1, { 0, 0, -1 } },//back z
		{ points + 6, points + 2, points + 1, { 0, 0, -1 } }
	};
	t_obj	objects[OBJ_COUNT] = {
		{ TRIS_OBJ, WHITE, (void *)(tris + 0)  },
		{ TRIS_OBJ, WHITE, (void *)(tris + 1)  },
		{ TRIS_OBJ, WHITE, (void *)(tris + 2)  },
		{ TRIS_OBJ, WHITE, (void *)(tris + 3)  },
		{ TRIS_OBJ, WHITE, (void *)(tris + 4)  },
		{ TRIS_OBJ, WHITE, (void *)(tris + 5)  },
		{ TRIS_OBJ, WHITE, (void *)(tris + 6)  },
		{ TRIS_OBJ, WHITE, (void *)(tris + 7)  },
		{ TRIS_OBJ, WHITE, (void *)(tris + 8)  },
		{ TRIS_OBJ, WHITE, (void *)(tris + 9)  },
		{ TRIS_OBJ, WHITE, (void *)(tris + 10) },
		{ TRIS_OBJ, WHITE, (void *)(tris + 11) },
		{ 0, BLACK, NULL }
	};
	t_light	lights[LIGHT_COUNT] = {
		{ { 2, -4, 3 }, { 255, 0, 0 }, 2 }
	};
	t_scene	scene = create_scene(&camera, (t_color){ 23, 20, 33 }, objects, lights);

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

	clock_t	start = clock();
	render(&scene, &mlx);
	printf("actual render time was %fms\n", (double)(clock() - start) / CLOCKS_PER_SEC * 1000);

	mlx_loop(mlx.ptr);
}
