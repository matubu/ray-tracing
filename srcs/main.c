/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/16 16:15:51 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/27 00:33:01 by matubu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

#ifdef DEV_SHOW_NORMAL
static inline unsigned int	ray_scene_color(const t_vec *orig,
		const t_vec *ray, const t_scene *scene)
{
	t_hit	hit;

	hit.normal = (t_vec){0, 0, 0};
	if (!ray_scene(orig, ray, scene, &hit))
		return (0x0);
	return ((int)(hit.normal.x * 100 + 100)
			| (int)(hit.normal.y * 100 + 100) << 8
			| (int)(hit.normal.z * 100 + 100) << 16);
}
#elif defined(DEV_SHOW_DISTANCE)
static int	range(int a, int b, float fac)
{
	return (
		((int)((float)(b & (255 << 0)) * fac
			+ (float)(a & (255 << 0)) * (1.0 - fac)) & (255 << 0))
		| ((int)((float)(b & (255 << 8)) * fac
				+ (float)(a & (255 << 8)) * (1.0 - fac)) & (255 << 8))
		| ((int)((float)(b & (255 << 16)) * fac
				+ (float)(a & (255 << 16)) * (1.0 - fac)) & (255 << 16))
			);
}

static int	dist(float fac)
{
	if (fac < .5)
		return (range(0x000000, 0x512372, fac * 2.0));
	else
		return (range(0x512372, 0xe1536f, fac * 2.0 - 1.0));
}

static inline unsigned int	ray_scene_color(const t_vec *orig,
		const t_vec *ray, const t_scene *scene)
{
	t_hit	hit;

	if (!ray_scene(orig, ray, scene, &hit))
		return (0x0);
	return (dist(1.0 - fmin(hit.dist / 80.0, 1.0)));
}
#else
static inline unsigned int	ray_scene_color(const t_vec *orig,
		const t_vec *ray, const t_scene *scene)
{
	t_hit	hit;
	t_hit	null;
	t_vec	to_light;
	//t_vec	reflected;

	if (!ray_scene(orig, ray, scene, &hit))
		return (scene->ambient.color);
	//reflected = reflect(ray, &hit.normal);
	//if (!ray_scene(&hit.pos, &reflected, scene, &null))
	//	return (0xffffff);
	to_light = normalize(sub(&scene->lights->pos, &hit.pos));
	if (ray_scene(&hit.pos, &to_light, scene, &null))
		return (rgbmult(hit.obj->color, 95));
	return (rgbmult(hit.obj->color,
			max((int)(dot(&to_light, &hit.normal) * 160.0), 0) + 95));
}
#endif

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

	const t_vec		up = {0, 0, 1};
	t_trash			t;
	register int	y;
	register int	x;

	t.dir = radian_to_vector(&cam->rot_euler);
	t.cam_right = normalize(cross(&t.dir, &up));
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
			*buf++ = ray_scene_color(&cam->pos, &t.ray, scene);
		}
	}
	mlx_put_image_to_window(win->ptr, win->win, win->img, 0, 0);

	// ----- DEBUG -----
	printf("rendering took %.3fms\n",
		(double)(clock() - start) / CLOCKS_PER_SEC * 1000);
}

int	main(int argc, char **argv)
{
	const t_scene	scene = parse(argc, argv);

	render(&scene, &scene.win, &scene.cam, scene.win.buf);
	mlx_hook(scene.win.win, 17, 0, hook_close, (void *)&scene);
	mlx_hook(scene.win.win, 3, 2, hook_key_up, (void *)&scene);
	mlx_hook(scene.win.win, 4, 1 << 2, hook_button_down, (void *)&scene);
	mlx_hook(scene.win.win, 5, 1 << 3, hook_button_up, (void *)&scene);
	mlx_hook(scene.win.win, 6, 64, hook_mouse_move, (void *)&scene);
	mlx_hook(scene.win.win, 3, 2, hook_key_up, (void *)&scene);
	mlx_loop(scene.win.ptr);
	return (0);
}
