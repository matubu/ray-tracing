/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 16:31:22 by mberger-          #+#    #+#             */
/*   Updated: 2022/01/17 11:29:54 by mberger-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCT_H
# define STRUCT_H

# include <math.h>

# define PI					3.14159265358979323846264338327950288
# define TWO_PI				6.28318530717958647692528676655900577
# define PI_TWO				1.57079632679489661923132169163975144
# define EPSILON			0.0001

# define WIDTH				480
# define HEIGHT				480
# define CAMERA_CLIP_START	.01

# define MOUSE_LEFT			1
# define MOUSE_RIGHT		2
# define MOUSE_WHEEL		3
# define MOUSE_SCROLL_UP	4
# define MOUSE_SCROLL_DOWN	5
# define MOUSE_SCROLL_LEFT	6
# define MOUSE_SCROLL_RIGHT	7

# define KEY_A				0
# define KEY_S				1
# define KEY_D				2
# define KEY_W				13

# define KEY_ESCAPE			53
# define KEY_SPACE			49
# define KEY_LSHIFT			257
# define KEY_LEFT			123
# define KEY_UP				126
# define KEY_RIGHT			124
# define KEY_DOWN			125

typedef struct s_vec {
	float	x;
	float	y;
	float	z;
}	t_vec;

typedef struct s_camera {
	t_vec	pos;
	t_vec	rot;
	int		width;
	int		height;
	float	fov_pixel;
}	t_camera;

typedef struct s_light {
	t_vec	pos;
	float	intensity;
	int		color;
}	t_light;

typedef struct s_ambient {
	float	intensity;
	int		color;
}	t_ambient;

typedef struct s_window {
	void	*ptr;
	void	*win;
	void	*img;
	int		*buf;
	int		width;
	int		height;
}	t_window;

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
}	t_sphere;

typedef struct s_plane {
	t_vec		pos;
	t_vec		normal;
}	t_plane;

typedef struct s_cylinder
{
	t_vec	pos;
	t_vec	dir;
	float	rad;
	float	height;
	t_vec	ca;
	float	caca;
}	t_cylinder;

typedef struct s_cone
{
	t_vec	pos;
	t_vec	dir;
	float	rad2;
	float	height;
}	t_cone;

typedef struct s_obj {
	void		(*func)();
	union {
		int			ptr;
		t_sphere	sphere;
		t_plane		plane;
		t_cylinder	cylinder;
		t_cone		cone;
	};
	float		ka;
	float		kd;
	float		ks;
	float		shinyness;
	int			color;
	t_bump_map	bump_map;
}	t_obj;

typedef struct s_hit {
	t_vec	pos;
	t_vec	normal;
	float	dist;
	t_obj	*obj;
}	t_hit;

typedef struct s_scene {
	t_window	win;
	t_camera	cam;
	int			cam_count;
	t_obj		obj[1024];
	int			obj_count;
	t_light		lights[64];
	int			lights_count;
	t_ambient	ambient;
	int			ambient_count;
	int			keys[1024];
}	t_scene;

#endif
