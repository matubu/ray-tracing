/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minirt.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 13:12:22 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/21 17:16:01 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINIRT_H
# define MINIRT_H

# include "maths.h"
# include "ray.h"
# include "vector.h"
# include "struct.h"
# include "libft.h"

# include <unistd.h>
# include <fcntl.h>
# include <stdlib.h>
# include <mlx.h>

// DEBUG LIBS
# include <time.h>
# include <stdio.h>

// DEV MODES
//# define DEV_NO_SHADOW
//# define DEV_SHOW_NORMAL
//# define DEV_SHOW_DISTANCE

int	print_err(char *message, int code);

t_window	window_open(char *title, int width, int height);
void		window_loop(t_scene *game, int (*hook)());
void		window_update(t_window *window);
void		window_clear(t_window *window, int color);
void		window_close(t_window *window);

t_scene		*scene_init(char *title, int width, int height);
t_camera	camera_init(int width, int height, t_vec pos, t_vec rot);

int			check_args(int ac, char **av);
int			check_file(char *filename);
t_scene		*parse(int ac, char **av);


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

static inline t_vec	reflect(const t_vec *ray, const t_vec *normal)
{
	t_vec	tmp;

	tmp = mult(normal, 2 * dot(ray, normal));
	return (sub(ray, &tmp));
}

#endif
