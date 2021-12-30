/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/16 16:15:51 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/30 16:24:58 by mberger-         ###   ########.fr       */
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

	i_diff = light->intensity * hit->obj->kd * dot(l, &hit->normal);
	i_spec = light->intensity * hit->obj->ks;
	r = reflect(l, &hit->normal);
	v = mult(ray, -1.0f);
	r_dot = dot(&r, &v);
	return (rgbmult(light->color,
			255.0 * (i_diff + i_spec * powf(r_dot, hit->obj->shinyness))));
}

static inline void	apply_bump_map(t_hit *hit)
{
	const t_vec	displace = mult(&hit->normal, (float)hit->obj->bump_map.buf[
			abs((int)fmod(hit->pos.y * 10, hit->obj->bump_map.height))
			* hit->obj->bump_map.width
			+ abs((int)fmod(hit->pos.x * 10, hit->obj->bump_map.width))
		] / 50000.0 + .5);

	hit->pos = add(&hit->pos, &displace);
}

static inline unsigned int	ray_color(const t_vec *orig,
		const t_vec *ray, const t_scene *scene, int rebounds)
{
	t_hit	hit;
	t_hit	hit_light;
	t_vec	l;
	t_vec	r;
	int		cnt;
	int		color;

	if (!ray_scene(orig, ray, scene, &hit))
		return (scene->ambient.color);
	if (hit.obj->bump_map.buf)
		apply_bump_map(&hit);
	cnt = scene->lights_count;
	color = rgbmult(scene->ambient.color,
			255.0 * scene->ambient.intensity * hit.obj->ka);
	while (cnt--)
	{
		l = normalize(sub(&scene->lights[cnt].pos, &hit.pos));
		if (!ray_scene(&hit.pos, &l, scene, &hit_light))
			color = rgbadd(color, ray_reflect(scene->lights + cnt, ray, &hit, &l));
	}
	cnt = scene->lights_count;
	if (rebounds-- > 0)
	{
		l = normalize(sub(&scene->cam.pos, &hit.pos));
		r = reflect(&l, &hit.normal);
		color = rgbadd(color, rgbmult(ray_color(&hit.pos, &r, scene, rebounds), 128));
	}
	return (rgbmatrix(hit.obj->color, color));
}

typedef struct s_trash
{
	t_vec	dir;
	t_vec	cam_right;
	t_vec	cam_up;
	float	half_x;
	float	half_y;
	t_vec	ray;
	t_vec	yr;
	t_vec	xr;
}	t_trash;

void	render(const t_scene *scene, const t_window *win,
		const t_camera *cam, int *buf)
{
	const clock_t	start = clock();
	// ----- DEBUG -----
	t_trash			t;
	register int	y;
	register int	x;

	t.dir = radian_to_vector(&cam->rot);
	t.cam_right = normalize(cross(&t.dir, &((t_vec){0, 0, 1})));
	t.cam_up = normalize(cross(&t.cam_right, &t.dir));
	t.half_x = cam->width / 2.0;
	t.half_y = cam->height / 2.0;
	y = cam->height;
	while (y--)
	{
		t.yr = mult(&t.cam_up, (y - t.half_y) * cam->fov_pixel);
		x = cam->width;
		while (x--)
		{
			t.xr = mult(&t.cam_right, (t.half_x - x) * cam->fov_pixel);
			t.ray = normalize(add3(&t.dir, &t.xr, &t.yr));
			*buf++ = ray_color(&cam->pos, &t.ray, scene, 1);
		}
	}
	mlx_put_image_to_window(win->ptr, win->win, win->img, 0, 0);
	// ----- DEBUG -----
	printf("rendering took %.3fms\n", (double)(clock() - start) / CLOCKS_PER_SEC * 1000);
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
