/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 16:31:22 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/28 16:48:29 by mberger-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCT_H
# define STRUCT_H

# include <math.h>

// MATH CONSTANT
# define PI					3.14159265358979323846264338327950288
# define TWO_PI				6.28318530717958647692528676655900577
# define PI_TWO				1.57079632679489661923132169163975144
# define EPSILON			0.0001

// CAMERA
# define WIDTH				1080
# define HEIGHT				720
# define CAMERA_CLIP_START	.01

typedef struct s_vec {
	float	x;
	float	y;
	float	z;
}	t_vec;

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

//t_bump_map	*bump_map;
typedef struct s_plane {
	t_vec		pos;
	t_vec		normal;
}	t_plane;

typedef struct s_cylinder
{
	t_vec	pos;
	t_vec	normal;
	float	rad;
	float	height;
}	t_cylinder;

typedef struct s_cone
{
	t_vec	pos;
	t_vec	dir;
}	t_cone;

typedef struct s_obj {
	void		(*func)();
	union {
		t_sphere	sphere;
		t_plane		plane;
		t_cylinder	cylinder;
		t_cone		cone;
	};
	int			color;
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
	int			button;
}	t_scene;

#endif
