/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 16:31:22 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/21 17:10:26 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCT_H
# define STRUCT_H

# include <math.h>

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
//	float	srad;
}	t_sphere;

typedef struct s_plane {
	t_vec		pos;
	t_vec		normal;
	//t_bump_map	*bump_map;
}	t_plane;

typedef struct s_cylinder
{
	t_vec	pos;
	t_vec	normal;
	float	rad;
//	float	srad;
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
/*
typedef struct s_obj {
	void		(*func)();
	int			color;
	void		*data;
}	t_obj;*/

typedef struct s_hit {
	t_vec	pos;
	t_vec	normal;
	float	dist;
	t_obj	*obj;
}	t_hit;

typedef struct s_scene {
	t_window	win;
	t_camera	cam;
	t_obj		obj[1024];
	int			obj_count;
	t_light		lights[64];
	t_ambient	ambient;
	int			button;
}	t_scene;

#endif