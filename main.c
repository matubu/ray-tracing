#include <unistd.h>
#include <fcntl.h>
#include <math.h>
//TODO pthread
//TODO pass more things by reference

#include <stdio.h>

//MATH CONSTANT
#define	PI                   3.1415926535
#define EPSILON              0.0000001

//COUNT
#define	POINTS_COUNT         300
#define TRIS_COUNT           100
#define	OBJ_COUNT            100
#define LIGHT_COUNT          10

//CAMERA
#define	WIDTH                10
#define	HEIGHT               10
#define	CAMERA_FOCAL_LENGTH  1.0
#define	CAMERA_POSITION      { 0, -5, 0 }
#define	CAMERA_ROTATION      { 0, -PI / 2, 0 }
#define	FIELD_OF_VIEW        PI / 2

//PERFORMANCE / SYSTEM
#define	THREAD_COUNT         16

typedef struct s_pos {
	float	x;
	float	y;
	float	z;
}	t_pos;

typedef struct s_tris {
	t_pos	*a;
	t_pos	*b;
	t_pos	*c;
	t_pos	normal;
}	t_tris;

typedef struct s_color {
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
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
	void		*data;
}	t_obj;

typedef struct s_camera {
	unsigned int	width;
	unsigned int	height;
	float			aspect_ratio;
	t_pos			pos;
	t_pos			rot;// euler radian
	float			fovx;// field of view
	float			fovy;
	float			fov_pixel;// field of view of a pixel
}	t_camera;

typedef struct s_light {
	t_pos	pos;
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

int		abs(int v)
{
	return v < 0 ? -v : v;
}

float	dot(t_pos a, t_pos b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

t_pos	sub(t_pos a, t_pos b) 
{
	t_pos	r = { a.x - b.x, a.y - b.y, a.z - b.z };
	return r;
}

t_pos	mult(t_pos a, float fac)
{
	t_pos	r = { a.x * fac, a.y * fac, a.z * fac };
	return r;
}

t_pos	add3(t_pos a, t_pos b, t_pos c)
{
	t_pos r = { a.x + b.x + c.x, a.y + b.y + c.y, a.z + b.z + c.z };
	return r;
}

t_pos	cross(t_pos a, t_pos b)
{
	t_pos r = { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
	return r;
}

t_color	mult_color(t_color *a, float fac)
{
	t_color	color;

	color.r = a->r * fac;
	color.g = a->g * fac;
	return color;
	color.b = a->b * fac;
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

// https://en.wikipedia.org/wiki/Fast_inverse_square_root
float	Q_rsqrt(float number)
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

t_pos	normalize(t_pos vec)
{
	float	fac = Q_rsqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);

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
t_pos	etov(t_pos *rot)
{
	t_pos	vec;
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
/*
t_pos	apply_rot(t_pos vec, t_pos *rot)
{
	t_pos	vec_a;
	float	sin_z = sinf(rot->z),
			cos_z = cosf(rot->z),
			sin_y = sinf(rot->y),
			cos_y = cosf(rot->y),
			sin_x = sinf(rot->x),
			cos_x = cosf(rot->x);

	printf("\ns: x %f, y %f, z %f, c: x %f, y %f, z %f\n", sin_x, sin_y, sin_z, cos_x, cos_y, cos_z);

	vec_a.       x =
		vec.      x  * ( cos_x * cos_y                         )
		+ vec.    y  * ( cos_x * sin_y * sin_z - sin_x * cos_z )
		+ vec.    z  * ( cos_x * sin_y * cos_z + sin_x * sin_z );

	vec_a.        y =
		vec.       x  * ( sin_x * cos_y                         )
		+ vec.     y  * ( sin_x * sin_y * sin_z + cos_x * cos_z )
		+ vec.     z  * ( sin_x * sin_y * cos_z - cos_x * sin_z );

	vec_a.        z =
		vec.       x  * ( -sin_z        )
		+ vec.     y  * ( cos_y * sin_z )
		+ vec.     z  * ( cos_y * cos_z );

	return vec_a;
}
*/
void	print_uint(int fd, unsigned int n)
{
	int i;
	char res[11];

	i = 11;
	res[--i] = n % 10 + '0';
	while (n /= 10)
		res[--i] = n % 10 + '0';
	write(fd, res + i, 11 - i);
}

void	save_image(t_camera *camera, char *path, t_color *data)
{
	int				fd;
	unsigned int	count;

	fd = open(path, O_CREAT|O_WRONLY|O_TRUNC, S_IWUSR);
	if (fd == -1)
		return ;
	write(fd, "P6\n", 3);
	print_uint(fd, camera->width);
	write(fd, " ", 1);
	print_uint(fd, camera->height);
	write(fd, "\n255\n", 5);
	count = camera->width * camera->height;
	write(fd, data, count * 3);
	close(fd);
}

int	ray_tris(t_pos *orig, t_pos *ray, t_tris *tris, t_pos *intersect, float *t)
{
	t_pos	ea = sub(*tris->b, *tris->a);// vector from b to a
	t_pos	eb = sub(*tris->c, *tris->a);// vector from c to a
	t_pos	pvec, dist, qvec;
	float	det, idet;

	pvec = cross(*ray, eb);
	det = dot(ea, pvec);
	if (det > -EPSILON && det < EPSILON)
		return 0;// ray and tris are parallel
	idet = 1.0 / det;
	dist = sub(*orig, *tris->a); // dist between origin and point a
	//U
	float	u = dot(dist, pvec) * idet;
	if (u < 0.0 || u > 1.0)
		return 0;
	//V
	qvec = cross(dist, ea);

	float	v = dot(*ray, qvec) * idet;
	if (v < 0.0 || u + v > 1.0)
		return 0;
	//T	
	*t = dot(eb, qvec) * idet;
	intersect->x = orig->x + ray->x * *t;
	intersect->y = orig->y + ray->y * *t;
	intersect->z = orig->z + ray->z * *t;

	return 1;
}

void	ray_scene(t_pos *orig, t_pos ray, t_scene *scene, t_color *color)
{
	printf(" = appl: %f %f %f\n", ray.x, ray.y, ray.z);
	*(int *)color = *(int *)(&scene->ambient_color);
	float	closest_hit = -1;
	t_pos	closest_hit_pos;
	t_obj	*closest_hit_obj;
	float	hit;
	t_pos	hit_pos;

	for (t_obj *obj = scene->obj; obj->type != END_OBJ; obj++)
	{
		switch(obj->type)
		{
			case TRIS_OBJ :
				if (!ray_tris(orig, &ray, (t_tris *)obj->data, &hit_pos, &hit))
					hit = -1;
				break;
			default :
				hit = -1;
				break;
		}
		if (hit != -1 && (closest_hit == -1 || hit < closest_hit))
		{
			closest_hit = hit;
			closest_hit_pos = hit_pos;
			closest_hit_obj = obj;
		}
	}
	(void)closest_hit_pos;
	(void)closest_hit_obj;
	if (closest_hit != -1)
	{
		color->r = color->g = color->b = closest_hit / 15 * 255;
	}
}

//TODO change to rotation based or relative position based
void	render(t_scene *scene, t_color *data)
{
	unsigned int		j = 0;

	t_pos	rot = scene->camera->rot;
	t_pos	dir = etov(&rot);
	t_pos	cv_right = normalize(cross(dir, (t_pos){0, 0, 1}));
	t_pos	cv_up = normalize(cross(cv_right, dir));
	float	half_x = scene->camera->width / 2;
	float	half_y = scene->camera->height / 2;

	//t_pos	dir = {0, 0, 0};
	//dir.z = (scene->camera->height / 2.0 - .5) * scene->camera->fov_pixel;

	for (unsigned int y = 0; y < scene->camera->height; y++)
	{
		t_pos	yr = mult(cv_up, (y - half_y) * scene->camera->fov_pixel);
		//dir.y = -scene->camera->fov / 2.0 - .5 * scene->camera->fov_pixel;
		for (unsigned int x = 0; x < scene->camera->width; x++)
		{
			t_pos	xr = mult(cv_right, (x - half_x) * scene->camera->fov_pixel);
		/*	printf("pos: %f %f %f, dir: %f %f %f -> %f %f %f  + rot: %f %f %f", 
					scene->camera->pos.x, scene->camera->pos.y, scene->camera->pos.z,
					dir.x, dir.y, dir.z,
					etov(dir).x, etov(dir).y, etov(dir).z,
					rot.x, rot.y, rot.z);*/
			//ray_scene(&scene->camera->pos, /*normalize(*/apply_rot(etov(dir), &scene->camera->rot)/*)*/, scene, data + j++);
			ray_scene(&scene->camera->pos, normalize(add3(dir, xr, yr)), scene, data + j++);
			//dir.y += scene->camera->fov_pixel;
		}
		//dir.z -= scene->camera->fov_pixel;
	}
}

t_camera	create_camera(unsigned int width, unsigned int height, 
		t_pos pos, t_pos rot, float fov)
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
	while (obj++->type != END_OBJ)
		scene.obj_count++;
	scene.lights = lights;
	return scene;
}

int	main()
{
	t_camera camera = create_camera(WIDTH, HEIGHT,
			(t_pos)CAMERA_POSITION, (t_pos)CAMERA_ROTATION, FIELD_OF_VIEW);
	t_pos	points[POINTS_COUNT] = {
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
		{ TRIS_OBJ, (void *)(tris + 0)  },
		{ TRIS_OBJ, (void *)(tris + 1)  },
		{ TRIS_OBJ, (void *)(tris + 2)  },
		{ TRIS_OBJ, (void *)(tris + 3)  },
		{ TRIS_OBJ, (void *)(tris + 4)  },
		{ TRIS_OBJ, (void *)(tris + 5)  },
		{ TRIS_OBJ, (void *)(tris + 6)  },
		{ TRIS_OBJ, (void *)(tris + 7)  },
		{ TRIS_OBJ, (void *)(tris + 8)  },
		{ TRIS_OBJ, (void *)(tris + 9)  },
		{ TRIS_OBJ, (void *)(tris + 10) },
		{ TRIS_OBJ, (void *)(tris + 11) },
		{ END_OBJ }
	};
	t_light	lights[LIGHT_COUNT] = {
		{ { 2, 2, 2 }, { 255, 200, 200 }, 2 }
	};
	t_color	data[WIDTH * HEIGHT];
	t_scene	scene = create_scene(&camera, (t_color){ 23, 20, 33 }, objects, lights);

	render(&scene, data);
	save_image(&camera, "image.ppm", data);

	printf("%f\n", camera.aspect_ratio);
}
