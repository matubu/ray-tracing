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

typedef struct s_scene {
	t_camera		*camera;
	t_obj			*obj;
	t_light 		*lights;
	int				ambient_color;
}	t_scene;

typedef struct s_hit {
	t_vec	pos;
	t_vec	normal;
	float	sdist;
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

inline t_vec	cross(t_vec *a, t_vec *b)
{
//	asm("VFMSUB132SS/VFMSUB213SS/VFMSUB231SS");
	return ((t_vec){ a->y * b->z - a->z * b->y, a->z * b->x - a->x * b->z, a->x * b->y - a->y * b->x });
}

inline float	sdist(t_vec *a)
{
	return (a->x * a->x + a->y * a->y + a->z * a->z);
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

inline unsigned int	rgbmult(unsigned int	color, int fac)
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

inline int	ray_sphere(t_vec *orig, t_vec *ray, t_sphere *sphere, t_hit *hit)
{
	t_vec	os;
	t_vec	o;
	float	d;

	os = sub(&sphere->pos, orig);
	os = cross(ray, &os);
	d = os.x * os.x + os.y * os.y + os.z * os.z;
	if (d > sphere->srad)
		return (0);
	o = mult(ray, -sqrtf(sphere->srad - d));
	hit->pos = add3(&sphere->pos, &os, &o);
	hit->normal = normalize(sub(&hit->pos, &sphere->pos));
	o = sub(&hit->pos, orig);
	hit->sdist = sdist(&o);
	return (1);
}

static inline int	ray_scene(t_vec *orig, t_vec *ray, t_scene *scene, t_hit *closest)
{
	t_hit			hit;
	register t_obj	*obj = scene->obj;

	closest->sdist = -1;
	while (obj->type)
	{
		hit.sdist = -1;
		if (obj->type == SPHERE_OBJ)
		{
			if (!ray_sphere(orig, ray, (t_sphere *)obj->data, &hit))
				hit.sdist = -1;
		}
		if (hit.sdist > CAMERA_CLIP_START && (closest->sdist == -1 || hit.sdist < closest->sdist))
		{
			closest->sdist = hit.sdist;
			closest->pos = hit.pos;
			closest->normal = hit.normal;
			closest->obj = obj;
		}
		obj++;
	}
	return (closest->sdist != -1);
}

int	ray_scene_color(t_vec *orig, t_vec *ray, t_scene *scene)
{
	t_hit	cam_hit;//, light_hit;
	t_vec	cam_to_light;
	t_vec	hit_to_light;

	if (!ray_scene(orig, ray, scene, &cam_hit))
		return (scene->ambient_color);
	//TODO law of light + multi light + color disruption
	hit_to_light = normalize(sub(&scene->lights->pos, &cam_hit.pos));
	//if (ray_scene(&cam_hit.pos, &hit_to_light, scene, &light_hit)) //TODO check hit distance
	//	return (BLACK);//scene->ambient_color);
	cam_to_light = normalize(sub(&scene->lights->pos, orig));
	//printf("%f %d %x %x\n", dot(&cam_to_light, &cam_hit.normal), 240 - abs((int)(dot(&cam_to_light, &cam_hit.normal) * 120.0)), rgbmult(cam_hit.obj->color, 240 - abs((int)(dot(&cam_to_light, &cam_hit.normal) * 120.0))), cam_hit.obj->color);
	return (rgbmult(cam_hit.obj->color, abs((int)(dot(&cam_to_light, &cam_hit.normal) * 120.0)) - 240));
	//return (cam_hit.obj->color);
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
	t_sphere	sphere[] = {
		{ (t_vec){ 5, 0, 0 }, 5.0 },
		{ (t_vec){ 0, 1, 0 }, 1.0 },
		{ (t_vec){ 0, 0, 2 }, 2.0 }
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

	render(&scene, mlx.buf);
	mlx_put_image_to_window(mlx.ptr, mlx.win, mlx.img, 0, 0);

	mlx_loop(mlx.ptr);
	return (0);
}
