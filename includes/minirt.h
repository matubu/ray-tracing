/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minirt.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 13:12:22 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/21 13:36:00 by acoezard         ###   ########.fr       */
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

typedef struct s_mlx_data {
	void	*ptr;
	void	*win;
	void	*img;
	int		*buf;
}	t_mlx_data;

typedef struct s_scene {
	t_camera	*camera;
	t_obj		*obj;
	t_light		*lights;
	int			ambient_color;
	int			button;
	t_mlx_data	*mlx;
}	t_scene;

typedef struct s_bump_map
{
	void	*img;
	int		*buf;
	int		width;
	int		height;
}	t_bump_map;

#endif
