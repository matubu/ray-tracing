/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/16 16:15:51 by mberger-          #+#    #+#             */
/*   Updated: 2022/01/18 09:46:44 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

static inline int	ray_reflect(const t_light *light, const t_vec *ray,
		const t_hit *hit, const t_vec *l)
{
	float	i_diff;
	float	i_spec;
	float	r_dot;
	t_vec	r;
	t_vec	v;

	i_diff = light->intensity * hit->obj->kd * dot(*l, hit->normal);
	i_spec = light->intensity * hit->obj->ks;
	r = reflect(l, &hit->normal);
	v = mult(*ray, -1.0f);
	r_dot = dot(r, v);
	return (rgbmult(light->color,
			255.0 * (i_diff + i_spec * powf(r_dot, hit->obj->shinyness))));
}

static inline void	apply_bump_map(t_hit *hit)
{
	hit->pos = add(hit->pos,
			mult(hit->normal, (float)hit->obj->bump_map.buf[
				abs((int)fmod(hit->pos.y * 10, hit->obj->bump_map.height))
				* hit->obj->bump_map.width
				+ abs((int)fmod(hit->pos.x * 10, hit->obj->bump_map.width))
			] / 50000.0 + .5)
			);
}

static inline unsigned int	ray_color(const t_vec *orig,
		const t_vec *ray, const t_scene *scene, int rec)
{
	t_hit	hits[2];
	t_vec	l;
	int		i;
	int		col;

	if (!ray_scene(orig, ray, scene, hits))
		return (scene->ambient.color);
	if (hits->obj->bump_map.buf)
		apply_bump_map(hits);
	col = rgbmult(scene->ambient.color,
			255.0 * scene->ambient.intensity * hits->obj->ka);
	i = scene->lights_count;
	while (i--)
	{
		l = normalize(sub(scene->lights[i].pos, hits->pos));
		if (!ray_scene(&hits->pos, &l, scene, hits + 1)
			|| hits[1].dist > dist(&scene->lights[i].pos, &hits->pos))
			col = rgbadd(col, ray_reflect(scene->lights + i, ray, hits, &l));
	}
	if (--rec < 0)
		return (rgbmatrix(hits->obj->color, col));
	l = normalize(sub(scene->cam.pos, hits->pos));
	l = reflect(&l, &hits->normal);
	return (rgbmatrix(hits->obj->color, rgbadd(col,
				rgbmult(ray_color(&hits->pos, &l, scene, rec), 128))));
}

typedef struct s_vars_render
{
	t_vec	dir;
	t_vec	cam_right;
	t_vec	cam_up;
	float	half_x;
	float	half_y;
	t_vec	ray;
	t_vec	yr;
	t_vec	xr;
}	t_vars_render;

void	render(const t_scene *scene, const t_window *win,
		const t_camera *cam, int *buf)
{
	t_vars_render	t;
	register int	y;
	register int	x;

	t.dir = radian_to_vector(&cam->rot);
	t.cam_right = normalize(cross(t.dir, (t_vec){0, 0, 1}));
	t.cam_up = normalize(cross(t.cam_right, t.dir));
	t.half_x = cam->width / 2.0;
	t.half_y = cam->height / 2.0;
	y = cam->height;
	while (y--)
	{
		t.yr = mult(t.cam_up, (y - t.half_y) * cam->fov_pixel);
		x = cam->width;
		while (x--)
		{
			t.xr = mult(t.cam_right, (t.half_x - x) * cam->fov_pixel);
			t.ray = normalize(add3(t.dir, t.xr, t.yr));
			*buf++ = ray_color(&cam->pos, &t.ray, scene, 2);
		}
	}
	mlx_put_image_to_window(win->ptr, win->win, win->img, 0, 0);
}

int	main(int argc, char **argv)
{
	t_scene	scene;

	scene = parse(argc, argv);
	controls_init(&scene);
	render(&scene, &scene.win, &scene.cam, scene.win.buf);
	mlx_hook(scene.win.win, 17, 0, hook_close, (void *)&scene);
	mlx_hook(scene.win.win, 6, 64, hook_mouse_move, (void *)&scene);
	mlx_hook(scene.win.win, 2, 1L << 0, hook_key_down, (void *)&scene);
	mlx_hook(scene.win.win, 3, 1L << 1, hook_key_up, (void *)&scene);
	mlx_loop_hook(scene.win.ptr, controls_listen, &scene);
	mlx_loop(scene.win.ptr);
	return (0);
}
