/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minirt.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 13:12:22 by acoezard          #+#    #+#             */
/*   Updated: 2022/01/18 10:15:02 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINIRT_H
# define MINIRT_H

# include "maths.h"
# include "ray.h"
# include "vector.h"
# include "struct.h"

# include <unistd.h>
# include <fcntl.h>
# include <stdlib.h>
# include <mlx.h>

# include <stdio.h>

char	*gnl(int fd);
int		free_splits(char **splits, int n);
char	**split(const char *s, char c);

void	push_obj(char ***args, t_scene *scene, t_obj obj);
void	push_light(t_scene *scene, t_light light);
void	push_camera(t_scene *scene, t_camera cam);
void	push_ambient(t_scene *scene, t_ambient ambient);

t_scene	parse(int argc, char **argv);
void	parse_line(char *type, char **arg, t_scene *scene);

void	err(char *s);

int		hook_close(t_scene *scene);
int		hook_key_down(int key, t_scene *scene);
int		hook_key_up(int key, t_scene *scene);
int		hook_mouse_move(int x, int y, t_scene *scene);

void	controls_init(t_scene *scene);
int		controls_listen(t_scene *scene);

void	render(const t_scene *scene, const t_window *win,
			const t_camera *cam, int *buf);

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

static inline float	dist(const t_vec *a, const t_vec *b)
{
	return (sqrt((a->x - b->x) * (a->x - b->x)
			+ (a->y - b->y) * (a->y - b->y)
			+ (a->z - b->z) * (a->z - b->z)));
}

static inline float	size(const t_vec *a)
{
	return (sqrt(a->x * a->x + a->y * a->y + a->z * a->z));
}

static inline t_vec	reflect(const t_vec *l, const t_vec *normal)
{
	const float	cos_theta = dot(*l, *normal) / (size(l) * size(normal));

	return (sub(mult(*normal, cos_theta * 2), *l));
}

#endif
