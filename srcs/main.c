/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/16 16:15:51 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/29 14:09:23 by mberger-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

static inline float	dist(const t_vec *a, const t_vec *b)
{
	return (sqrt((a->x - b->x) * (a->x - b->x)
			+ (a->y - b->y) * (a->y - b->y)
			+ (a->z - b->z) * (a->z - b->z)));
}

static inline unsigned int	ray_color(const t_vec *orig,
		const t_vec *ray, const t_scene *scene)
{
	t_hit	hit;
	t_hit	hit_light;
	t_vec	to_light;
	int		count;
	float	fac;

	if (!ray_scene(orig, ray, scene, &hit))
		return (scene->ambient.color);
	fac = 0;
	count = scene->lights_count;
	while (count--)
	{
		to_light = normalize(sub(&scene->lights[count].pos, &hit.pos));
		if (ray_scene(&hit.pos, &to_light, scene, &hit_light)
			&& hit_light.dist < dist(&scene->lights[count].pos, &hit.pos))
			fac += scene->ambient.intensity;
		else
			fac += fmin(fmax(dot(&to_light, &hit.normal)
						* scene->lights[count].intensity, 0.0)
					+ scene->ambient.intensity, 1.0);
	}
	return (rgbmult(hit.obj->color, 255.0 * fac / scene->lights_count));
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

	t.dir = radian_to_vector(&cam->rot_euler);
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
			*buf++ = ray_color(&cam->pos, &t.ray, scene);
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
