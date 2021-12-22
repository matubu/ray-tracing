/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/16 16:15:51 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/22 14:18:33 by mberger-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

/*
	// DEV MODES
	#ifdef DEV_SHOW_NORMAL

	return ((int)(cam_hit.normal.x * 100 + 100)
			| (int)(cam_hit.normal.y * 100 + 100) << 8
			| (int)(cam_hit.normal.z * 100 + 100) << 16);
	#endif
	#ifdef DEV_SHOW_DISTANCE

	return (rgbmult(WHITE, 1.0 / cam_hit.dist * 255));
	#endif
	#ifdef DEV_NO_SHADOW

	return (cam_hit.obj->color);
	#endif
*/
static inline unsigned int	ray_scene_color(const t_vec *orig,
		const t_vec *ray, const t_scene *scene)
{
	t_hit	cam_hit;
	t_hit	light_hit;
	t_vec	hit_to_light;

	if (!ray_scene(orig, ray, scene, &cam_hit))
		return (scene->ambient_color);
	hit_to_light = normalize(sub(&scene->lights->pos, &cam_hit.pos));
	if (ray_scene(&cam_hit.pos, &hit_to_light, scene, &light_hit))
		return (rgbmult(cam_hit.obj->color, 95));
	return (rgbmult(cam_hit.obj->color,
			max((int)(dot(&hit_to_light, &cam_hit.normal) * 160.0), 0) + 95));
}

// TODO binary tree bounding box
void	render(const t_scene *scene, const t_window *win,
		const t_camera *cam, int *buf)
{
	const t_vec		up = {0, 0, 1};
	const t_vec		dir = radian_to_vector(&cam->rot_euler);
	const t_vec		cv_right = normalize(cross(&dir, &up));
	const t_vec		cv_up = normalize(cross(&cv_right, &dir));
	const float		half_x = cam->width / 2.0;
	const float		half_y = cam->height / 2.0;
	t_vec			ray;
	t_vec			yr;
	t_vec			xr;
	register int	y;
	register int	x;

	y = cam->height;
	while (y--)
	{
		yr = mult(&cv_up, (y - half_y) * cam->fov_pixel);
		x = cam->width;
		while (x--)
		{
			xr = mult(&cv_right, (half_x - x) * cam->fov_pixel);
			ray = normalize(add3(&dir, &xr, &yr));
			*buf++ = ray_scene_color(&cam->pos, &ray, scene);
		}
	}
	mlx_put_image_to_window(win->ptr, win->win, win->img, 0, 0);
}

//HOOKS
//mlx_hook(scene->window->mlx, 4, 1 << 2, on_button_down, &scene);
//mlx_hook(scene->window->mlx, 5, 1 << 3, on_button_up, &scene);
//mlx_hook(scene->window->mlx, 6, 64, on_mouse_move, &scene);
//mlx_hook(scene->window->mlx, 3, 2, on_key_up, &scene);
int	main(int argc, char **argv)
{
	const t_scene	scene = parse(argc, argv);
	const clock_t	start = clock();

	render(&scene, &scene.win, &scene.cam, scene.win.buf);
	printf("rendering took %.3fms\n",
		(double)(clock() - start) / CLOCKS_PER_SEC * 1000);
	mlx_hook(scene.win.win, 17, 0, hook_close, (void *)&scene);
	mlx_hook(scene.win.win, 3, 2, hook_key_up, (void *)&scene);
	mlx_loop(scene.win.ptr);
	return (0);
}
