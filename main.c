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
#define	CAMERA_CLIP_START		.1

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

typedef struct s_sphere {
	t_vec	pos;
	float	rad;
	float	srad;
}	t_sphere;

typedef enum e_obj_type {
	END_OBJ    = 0,
	SPHERE_OBJ = 1,
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

typedef enum e_mouse {
	LEFT_MOUSE=1,
	MIDDLE_MOUSE=3
}	t_mouse;

typedef struct s_scene {
	t_camera	*camera;
	t_obj		*obj;
	t_light 	*lights;
	int			ambient_color;
	t_mouse		button;
	t_mlx_data	*mlx;
}	t_scene;

inline float	dot(t_vec *a, t_vec *b)
{
//	asm("dpps %xmm1, %xmm2, 11100000b" : "=a" (ret) : "a" (a) , "b" (b));
	return (a->x * b->x + a->y * b->y + a->z * b->z);
}

inline t_vec	sub(t_vec *a, t_vec *b) 
{;
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

inline t_vec	add(t_vec *a, t_vec *b)
{
	return ((t_vec){ a->x + b->x, a->y + b->y, a->z + b->z });
}

inline t_vec	cross(t_vec *a, t_vec *b)
{
//	asm("VFMSUB132SS/VFMSUB213SS/VFMSUB231SS");
	return ((t_vec){ a->y * b->z - a->z * b->y, a->z * b->x - a->x * b->z, a->x * b->y - a->y * b->x });
}

t_vec	reflect(t_vec *ray, t_vec *normal)
{
	t_vec	tmp = mult(normal, 2 * dot(ray, normal));
	return (sub(ray, &tmp));
}
/*
inline float	sdist(t_vec *a)
{
	return (a->x * a->x + a->y * a->y + a->z * a->z);
}
*/
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

inline unsigned int	rgbmult(unsigned int color, int fac)
{
	return (
		((((color & (255 << 16)) * fac) & (255 << 24))
		| (((color & (255 << 8)) * fac) & (255 << 16))
		| (((color & (255 << 0)) * fac) & (255 << 8))) >> 8
	);
}

inline t_vec	radian_to_vector(t_vec *rot)
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
	});;
}

static inline void	ray_sphere(t_vec *orig, t_vec *ray, t_sphere *sphere, t_hit *hit)
{
/*	t_vec	os;
	t_vec	o;
	float	d;

	os = sub(&sphere->pos, orig);
	//os = sub(orig, &sphere->pos);
	os = cross(ray, &os);
	d = os.x * os.x + os.y * os.y + os.z * os.z;
	if (d > sphere->srad)
	{
		hit->dist = -1;
		return ;
	}
	//o = mult(ray, -sqrtf(sphere->srad - d));
	o = mult(ray, cos(sqrtf(d) / sphere->rad) * -sphere->rad);
	hit->pos = add3(&sphere->pos, &os, &o);
	hit->normal = normalize(sub(&hit->pos, &sphere->pos));
	o = sub(&hit->pos, orig);
	hit->dist = sqrt(o.x * o.x + o.y * o.y + o.z * o.z);*/
	t_vec	oc = sub(orig, &sphere->pos);
	float	a = dot(ray, ray);
	float	b = dot(&oc, ray);
	float	c = dot(&oc, &oc) - sphere->srad;
	float	d = b * b - a * c;
	if (d <= 0)
		return ((void)(hit->dist = -1));
	hit->dist = (-b - sqrt(b * b - a * c)) / a;
	hit->pos = mult(ray, hit->dist);
	hit->pos = add(orig, &hit->pos);
	hit->normal = normalize(sub(&hit->pos, &sphere->pos));
}

static inline int	ray_scene(t_vec *orig, t_vec *ray, t_scene *scene, t_hit *closest)
{
	t_hit			hit;
	register t_obj	*obj = scene->obj;

	closest->dist = -1;
	while (obj->type)
	{
		hit.dist = -1;
		if (obj->type == SPHERE_OBJ)
			ray_sphere(orig, ray, (t_sphere *)obj->data, &hit);
		if (hit.dist > CAMERA_CLIP_START
				&& (closest->dist == -1 || hit.dist < closest->dist))
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

unsigned int	ray_scene_color(t_vec *orig, t_vec *ray, t_scene *scene)
{
	t_hit	cam_hit, light_hit;
	t_vec	cam_to_light;
	t_vec	reflected;
	t_vec	hit_to_light;

	if (!ray_scene(orig, ray, scene, &cam_hit))
		return (scene->ambient_color);
	reflected = reflect(ray, &cam_hit.normal);
	//return (
	//		(int)(cam_hit.normal.x * 100 + 100)
	//		| (int)(cam_hit.normal.y * 100 + 100) << 8
	//		| (int)(cam_hit.normal.z * 100 + 100) << 16
	//		);
	//TODO law of light + multi light + color disruption
	hit_to_light = normalize(sub(&scene->lights->pos, &cam_hit.pos));
	if (ray_scene(&cam_hit.pos, &hit_to_light, scene, &light_hit)) //TODO check hit distance
		return (rgbmult(cam_hit.obj->color, 100));
	//	return (BLACK);//scene->ambient_color);
	cam_to_light = normalize(sub(&scene->lights->pos, orig));
	//printf("%f %d %x %x\n", dot(&cam_to_light, &cam_hit.normal), 240 - abs((int)(dot(&cam_to_light, &cam_hit.normal) * 120.0)), rgbmult(cam_hit.obj->color, 240 - abs((int)(dot(&cam_to_light, &cam_hit.normal) * 120.0))), cam_hit.obj->color);
	//return (rgbmult(cam_hit.obj->color, abs((int)(dot(&cam_to_light, &reflected) * 120.0)) + 100));
	//return (rgbmult(cam_hit.obj->color, 240 - abs((int)(dot(&cam_to_light, &cam_hit.normal) * 120.0))));
	return (cam_hit.obj->color);
}

void	render(t_scene *scene, int *buf)
{
	clock_t	start = clock();

	t_vec	up = UP;
	t_vec	dir = radian_to_vector(&scene->camera->rot);
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
			ray = normalize(add3(&dir, &xr, &yr));
			*buf++ = ray_scene_color(&scene->camera->pos, &ray, scene);
		}
	}
	mlx_put_image_to_window(scene->mlx->ptr, scene->mlx->win, scene->mlx->img, 0, 0);

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

int	on_mouse_move(int x, int y, t_scene *scene)
{
	static int	first = 1;
	static int	last[2];

	if (first)
		first = 0;
	else
	{
		if (scene->button == LEFT_MOUSE)
		{
			scene->camera->rot.z += (float)(last[0] - x) / 50.0;
			scene->camera->rot.y += (float)(last[1] - y) / 50.0;
		}
		render(scene, scene->mlx->buf);
	}
	last[0] = x;
	last[1] = y;
	return (1);
}

int	on_button_down(int button, int x, int y, t_scene *scene)
{
	(void)x;
	(void)y;
	scene->button = button;
	t_vec rad;
	rad = radian_to_vector(&scene->camera->rot);
	if (button == 5)
	//	scene->camera->fov_pixel /= .9;
		rad = mult(&rad, -1);
	//else if (button == 4)
	//	rad = mult(&rad, .1);
		//	scene->camera->fov_pixel *= .9;
		//scene->camera->pos = sub(scene->camera->pos, mult(radian_to_vector(&scene->camera->rot), .1));
	if (button == 4 || button == 5)
	{
		scene->camera->pos = sub(&scene->camera->pos, &rad);
		render(scene, scene->mlx->buf);
	}
	return (1);
}

int	on_button_up(int button, int x, int y, t_scene *scene)
{
	(void)x;
	(void)y;
	(void)button;
	scene->button = 0;
	return (1);
}

int	main()
{
	t_mlx_data	mlx;
	int			null;

	t_camera camera = create_camera(WIDTH, HEIGHT,
			(t_vec){ 8, -4, 5.5 }, (t_vec){ -PI / 4, 0, PI / 4 }, FIELD_OF_VIEW);
	t_sphere	sphere[] = {
		{ (t_vec){ 0, 0, 0 }, 2.0, 4.0 },
		{ (t_vec){ 0, 1, 0 }, 1.0, 1.0 },
		{ (t_vec){ 4, -4, 3.5 }, .5, .25 }
	};
	t_obj	objects[] = {
		{ SPHERE_OBJ, RED, (void *)(sphere + 0) },
		{ SPHERE_OBJ, GREEN, (void *)(sphere + 1) },
		{ SPHERE_OBJ, BLUE, (void *)(sphere + 2) },
		{ END_OBJ, BLACK, NULL }
	};
	t_light	lights[] = {
		{ { 5, -5, 5 }, RED, 2 }
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

	scene.mlx = &mlx;

	mlx_hook(mlx.win, 4, 1 << 2, on_button_down, &scene);
	mlx_hook(mlx.win, 5, 1 << 3, on_button_up, &scene);
	mlx_hook(mlx.win, 6, 64, on_mouse_move, &scene);

	render(&scene, scene.mlx->buf);

	mlx_loop(mlx.ptr);
	return (0);
}
