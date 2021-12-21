/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minirt.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 13:12:22 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/21 16:15:40 by mberger-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINIRT_H
# define MINIRT_H

# include <unistd.h>
# include <fcntl.h>
# include <math.h>
# include <stdlib.h>
# include <mlx.h>

// DEBUG LIBS
# include <time.h>
# include <stdio.h>

// DEV MODES
//# define DEV_NO_SHADOW
//# define DEV_SHOW_NORMAL
//# define DEV_SHOW_DISTANCE

// MATH CONSTANT
# define PI					M_PI
# define EPSILON			0.0001

// CAMERA
# define WIDTH				1000
# define HEIGHT				1000
# define CAMERA_CLIP_START	.01

// COLORS
# define RED				0xf598af
# define GREEN				0xa2fac0
# define BLUE				0xaec9f0
# define WHITE				0xeeeeee
# define BLACK				0x191a1f
# define GREY				0x504d5e

typedef struct s_vec {
	float	x;
	float	y;
	float	z;
}	t_vec;

typedef struct s_obj {
	void		(*func)();
	int			color;
	void		*data;
}	t_obj;

typedef struct s_camera {
	t_vec	pos;
	t_vec	rot_euler;
	int		width;
	int		height;
	float	fov_pixel;
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

typedef struct s_window {
	void	*mlx;
	void	*window;
	void	*image;
	int		*buffer;
	int		width;
	int		height;
}	t_window;

typedef struct s_scene {
	t_camera	*camera;
	t_obj		*obj;
	t_light		*lights;
	int			ambient_color;
	int			button;
	t_window	*window;
}	t_scene;

typedef struct s_bump_map
{
	void	*img;
	int		*buf;
	int		width;
	int		height;
}	t_bump_map;

typedef struct s_sphere {
	t_vec	pos;
	float	rad;
	float	srad;
}	t_sphere;

typedef struct s_plane {
	t_vec		pos;
	t_vec		normal;
	t_bump_map	*bump_map;
}	t_plane;

typedef struct s_cylinder
{
	t_vec	pos;
	t_vec	normal;
	float	rad;
	float	srad;
	float	height;
}	t_cylinder;

typedef struct s_cone
{
	t_vec	pos;
	t_vec	dir;
}	t_cone;

t_window	window_open(char *title, int width, int height);
void		window_loop(t_scene *game, int (*hook)());
void		window_update(t_window *window);
void		window_clear(t_window *window, int color);
void		window_close(t_window *window);

static inline int	max(int a, int b)
{
	if (a > b)
		return (a);
	return (b);
}

static inline float	q_rsqrt(float y)
{
	int			i;
	float		x2;
	const float	threehalfs = 1.5F;

	x2 = y * 0.5F;
	i = *(int *)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float *)&i;
	return (y * (threehalfs - (x2 * y * y)));
}

static inline unsigned int	rgbmult(unsigned int color, int fac)
{
	return (
		((((color & (255 << 16)) * fac) & (255 << 24))
			| (((color & (255 << 8)) * fac) & (255 << 16))
			| (((color & (255 << 0)) * fac) & (255 << 8))) >> 8
	);
}

static inline float	dot(const t_vec *a, const t_vec *b)
{
	return (a->x * b->x + a->y * b->y + a->z * b->z);
}

static inline t_vec	sub(const t_vec *a, const t_vec *b)
{
	return ((t_vec){a->x - b->x, a->y - b->y, a->z - b->z});
}

static inline t_vec	mult(const t_vec *a, const float fac)
{
	return ((t_vec){a->x * fac, a->y * fac, a->z * fac});
}

static inline t_vec	add3(const t_vec *a, const t_vec *b, const t_vec *c)
{
	return ((t_vec){
		a->x + b->x + c->x,
		a->y + b->y + c->y,
		a->z + b->z + c->z
	});
}

static inline t_vec	add(const t_vec *a, const t_vec *b)
{
	return ((t_vec){a->x + b->x, a->y + b->y, a->z + b->z});
}

static inline t_vec	cross(const t_vec *a, const t_vec *b)
{
	return ((t_vec){
		a->y * b->z - a->z * b->y,
		a->z * b->x - a->x * b->z,
		a->x * b->y - a->y * b->x
	});
}

static inline t_vec	reflect(const t_vec *ray, const t_vec *normal)
{
	t_vec	tmp;

	tmp = mult(normal, 2 * dot(ray, normal));
	return (sub(ray, &tmp));
}

static inline t_vec	normalize(const t_vec vec)
{
	return (mult(&vec, q_rsqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z)));
}

static inline t_vec	radian_to_vector(const t_vec *rot)
{
	float	sin_z;
	float	cos_z;
	float	sin_y;
	float	sin_x;
	float	cos_x;

	sin_z = sinf(rot->z);
	cos_z = cosf(rot->z);
	sin_y = sinf(rot->y);
	sin_x = sinf(rot->x);
	cos_x = cosf(rot->x);
	return ((t_vec){
		-cos_z * sin_y * sin_x - sin_z * cos_x,
		-sin_z * sin_y * sin_x + cos_z * cos_x,
		cosf(rot->y) * sin_x
	});
}

static inline void	ray_sphere(t_vec *orig, t_vec *ray,
		t_sphere *sphere, t_hit *hit)
{
	const t_vec	oc = sub(orig, &sphere->pos);
	const float	a = dot(ray, ray);
	const float	b = dot(&oc, ray);
	const float	c = dot(&oc, &oc) - sphere->srad;
	const float	d = b * b - a * c;

	if (d <= EPSILON)
		return ((void)(hit->dist = -1));
	hit->dist = (-b - sqrt(b * b - a * c)) / a;
	hit->pos = mult(ray, hit->dist);
	hit->pos = add(orig, &hit->pos);
	hit->normal = normalize(sub(&hit->pos, &sphere->pos));
}

static inline void	ray_plane(t_vec *orig, t_vec *ray,
		t_plane *plane, t_hit *hit)
{
	const float	d = dot(&plane->normal, ray);

	if (d >= EPSILON && d <= EPSILON)
		return ((void)(hit->dist = -1));
	hit->pos = sub(&plane->pos, orig);
	hit->dist = dot(&hit->pos, &plane->normal) / d;
	hit->pos = mult(ray, hit->dist);
	hit->pos = add(orig, &hit->pos);
	hit->normal = plane->normal;
}

static inline void	ray_cylinder(t_vec *orig, t_vec *ray,
		t_cylinder *cylinder, t_hit *hit)
{
	const float	a = (ray->x * ray->x) + (ray->z * ray->z);
	const float	b = 2 * (ray->x * (orig->x - cylinder->pos.x)
			+ ray->z * (orig->z - cylinder->pos.z));
	const float	c = (orig->x - cylinder->pos.x) * (orig->x - cylinder->pos.x)
		+ (orig->z - cylinder->pos.z) * (orig->z - cylinder->pos.z)
		- cylinder->srad;
	const float	delta = b * b - 4 * (a * c);
	float		r;

	if (fabs(delta) < 0.001 || delta < 0.0)
		return ((void)(hit->dist = -1));
	hit->dist = fmin((-b - sqrt(delta)) / (2 * a),
			(-b + sqrt(delta)) / (2 * a));
	r = orig->y + hit->dist * ray->y;
	if (!(r >= cylinder->pos.y) && (r <= cylinder->pos.y + cylinder->height))
		return ((void)(hit->dist = -1));
}

// radius/apex https://mrl.cs.nyu.edu/~dzorin/rend05/lecture2.pdf
static inline void	ray_cone(t_vec *orig, t_vec *ray, t_cone *cone, t_hit *hit)
{
	(void)orig;
	(void)ray;
	(void)cone;
	hit->dist = 0;
}

static inline int	ray_scene(t_vec *orig, t_vec *ray,
		t_scene *scene, t_hit *closest)
{
	t_hit			hit;
	register t_obj	*obj;

	obj = scene->obj;
	closest->dist = -1;
	while (obj->func)
	{
		hit.dist = -1;
		obj->func(orig, ray, obj->data, &hit);
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

#endif
