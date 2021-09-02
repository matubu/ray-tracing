#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#include <stdio.h>

#define	PI                 3.1415926535
#define EPSILON            0.0000001

#define	POINTS_COUNT       300
#define	TRIS_COUNT         100

#define	WIDTH              100
#define	HEIGHT             100
#define	CAMERA_POSITION    { -3, 0, 0 }
#define	CAMERA_ROTATION   { 0, 0, PI / 2 }
#define	FIELD_OF_VIEW      PI / 2

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

typedef struct s_camera {
	unsigned int width;
	unsigned int height;
	t_pos pos;
	t_pos rot;// euler radian
	float fv;// field of view
	float angle_of_pixel;// angle of a pixel
}	t_camera;

float	dot(t_pos a, t_pos b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
t_pos	sub(t_pos a, t_pos b) 
{
	t_pos	r = { a.x - b.x, a.y - b.y, a.z - b.z };
	return r;
}
t_pos	cross(t_pos a, t_pos b)
{
	t_pos r = { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
	return r;
}
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
t_pos	etov(t_pos *rot)
{
	t_pos	vec;
	float	yaw = rot->z,
			pitch = rot->y,
			roll = rot->x;
	float	yaw_sin = sinf(yaw),
			yaw_cos = cosf(yaw),
			pitch_sin = sinf(pitch),
			pitch_cos = cosf(pitch),
			roll_sin = sinf(roll),
			roll_cos = cosf(roll);

	vec.x = -yaw_cos * pitch_sin * roll_sin - yaw_sin * roll_cos;
	vec.y = -yaw_sin * pitch_sin * roll_sin + yaw_cos * roll_cos;
	vec.z =  pitch_cos * roll_sin;
	return vec;
}	

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
	unsigned int	i;
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
	i = 0;
	while (i < count)
		write(fd, data + i++, 3);
	close(fd);
}

int	ray_tris(t_pos orig, t_pos ray, t_tris *tris, t_pos *intersect)
{
	t_pos	ea = sub(*tris->b, *tris->a);// vector from b to a
	t_pos	eb = sub(*tris->c, *tris->a);// vector from c to a
	t_pos	pvec, dist, qvec;
	float	det, idet;

	pvec = cross(ray, eb);
	det = dot(ea, pvec);
	if (det > -EPSILON && det < EPSILON)
		return 0;// ray and tris are parallel
	idet = 1.0 / det;
	dist = sub(orig, *tris->a); // dist between origin and point a
//U
	float	u = dot(dist, pvec) * idet;
	if (u < 0.0 || u > 1.0)
		return 0;
//V
	qvec = cross(dist, ea);

	float	v = dot(ray, qvec) * idet;
	if (v < 0.0 || det + v > 1.0)
		return 0;
//T	
	(void)intersect;
//	float	t = dot(eb, qvec) * idet;
//	intersect->x = orig.x + ray.x * t;
//	intersect->y = orig.y + ray.y * t;
//	intersect->z = orig.z + ray.z * t;

	return 1;
}

void	render(t_camera *camera, t_tris *tris, unsigned int tris_count, t_color *data)
{
	unsigned int		j = 0;
	t_pos	intersect;

	t_pos	vec;
	t_pos	rot = camera->rot;
	rot.y -= camera->height / 2 * camera->angle_of_pixel;

	for (unsigned int y = 0; y < camera->height; y++)
	{
		rot.z = camera->rot.z - camera->width / 2 * camera->angle_of_pixel;
		for (unsigned int x = 0; x < camera->width; x++)
		{
			data[j].r = 0;
			data[j].g = 0;
			data[j].b = 0;
			vec = etov(&rot);
			printf("%f %f %f -> %f %f %f\n", rot.x, rot.y, rot.z, vec.x, vec.y, vec.z);
			for (unsigned int i = 0; i < tris_count; i++)
				if (ray_tris(camera->pos, vec, tris + i, &intersect))
				{
					data[j].r = 255;
					break;
				}
			rot.z += camera->angle_of_pixel;
			j++;
		}
		rot.y += camera->angle_of_pixel;
	}
}

t_camera	create_camera(unsigned int width, unsigned int height, 
		t_pos pos, t_pos rot, float fv)
{
	t_camera	camera;

	camera.width = width;
	camera.height = height;
	camera.pos = pos;
	camera.rot = rot;
	camera.fv = fv;
	camera.angle_of_pixel = fv / width;
	return camera;
}

int	main()
{
	t_camera camera = create_camera(WIDTH, HEIGHT,
			(t_pos)CAMERA_POSITION, (t_pos)CAMERA_ROTATION, FIELD_OF_VIEW);
	//int	points_count = 8;
	t_pos	points[POINTS_COUNT] = {
		{ 1, 1, 1 },
		{ 1, 1, -1 },
		{ 1, -1, -1 },
		{ 1, -1, 1 },
		{ -1, 1, 1 },
		{ -1, 1, -1 },
		{ -1, -1, -1 },
		{ -1, -1, 1 }
	};
	unsigned int	tris_count = 12;
	t_tris	tris[TRIS_COUNT] = {
		{ points + 0, points + 1, points + 2, { 1, 0, 0 } },//front x
		{ points + 0, points + 3, points + 2, { 1, 0, 0 } },
		{ points + 4, points + 5, points + 6, { -1, 0, 0 } },//back x
		{ points + 4, points + 8, points + 6, { -1, 0, 0 } },
		{ points + 4, points + 5, points + 1, { 0, 1, 0 } },//front y
		{ points + 4, points + 0, points + 1, { 0, 1, 0 } },
		{ points + 7, points + 6, points + 2, { 0, -1, 0 } },//back y
		{ points + 7, points + 3, points + 2, { 0, -1, 0 } },
		{ points + 7, points + 4, points + 0, { 0, 0, 1 } },//front z
		{ points + 7, points + 3, points + 0, { 0, 0, 1 } },
		{ points + 6, points + 5, points + 1, { 0, 0, -1 } },//back z
		{ points + 6, points + 2, points + 1, { 0, 0, -1 } }
	};
	t_color	data[WIDTH * HEIGHT];

	render(&camera, tris, tris_count, data);
	save_image(&camera, "image.ppm", data);
}
