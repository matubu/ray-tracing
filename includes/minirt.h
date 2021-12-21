/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minirt.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 13:12:22 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/21 15:53:40 by acoezard         ###   ########.fr       */
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

// radius/apex https://mrl.cs.nyu.edu/~dzorin/rend05/lecture2.pdf
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

void		ray_sphere(t_vec *orig, t_vec *ray, t_sphere *sphere, t_hit *hit);
void		ray_plane(t_vec *orig, t_vec *ray, t_plane *plane, t_hit *hit);
void		ray_cylinder(t_vec *orig, t_vec *ray, \
		t_cylinder *cylinder, t_hit *hit);
void		ray_cone(t_vec *orig, t_vec *ray, t_cone *cone, t_hit *hit);
int			ray_scene(t_vec *orig, t_vec *ray, t_scene *scene, t_hit *closest);

float		dot(t_vec *a, t_vec *b);
t_vec		sub(t_vec *a, t_vec *b);
t_vec		mult(t_vec *a, float fac);
t_vec		add3(t_vec *a, t_vec *b, t_vec *c);
t_vec		add(t_vec *a, t_vec *b);
t_vec		cross(t_vec *a, t_vec *b);
t_vec		normalize(t_vec vec);
t_vec		radian_to_vector(t_vec *rot);
t_vec		reflect(t_vec *ray, t_vec *normal);

float		q_rsqrt(float y);

#endif
