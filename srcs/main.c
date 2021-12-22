/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/16 16:15:51 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/22 10:46:56 by mberger-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
/*
t_bump_map	load_bump_map(t_window *window, char *filename)
{
	t_bump_map	img;
	int			null;

	img.img = mlx_xpm_file_to_image(window->mlx, filename, &img.width, &img.height);
	if (img.img == NULL)
		err("could not load image");
	img.buf = (int *)mlx_get_data_addr(img.img, &null, &null, &null);
	return (img);
}
*/
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
static inline unsigned int	ray_scene_color(t_vec *orig, t_vec *ray, t_scene *scene)
{
	t_hit	cam_hit, light_hit;
	t_vec	hit_to_light;

	if (!ray_scene(orig, ray, scene, &cam_hit))
		return (scene->ambient_color);
	hit_to_light = normalize(sub(&scene->lights->pos, &cam_hit.pos));
	if (ray_scene(&cam_hit.pos, &hit_to_light, scene, &light_hit))
		return (rgbmult(cam_hit.obj->color, 95));
	return (rgbmult(cam_hit.obj->color, max((int)(dot(&hit_to_light, &cam_hit.normal) * 160.0), 0) + 95));
}

void	render(t_scene *scene, t_camera *cam, int *buf)
{
	clock_t	start = clock();

	t_vec	up = {0, 0, 1};
	t_vec	dir = radian_to_vector(&cam->rot_euler);
	t_vec	cv_right = normalize(cross(&dir, &up));
	t_vec	cv_up = normalize(cross(&cv_right, &dir));
	float	half_x = cam->width / 2.0;
	float	half_y = cam->height / 2.0;
	t_vec	ray;
	t_vec	yr, xr;

	register int	y = cam->height;
	register int	x;
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
	mlx_put_image_to_window(scene->win.mlx, scene->win.win, scene->win.img, 0, 0);

	printf("rendering took %.2fms\n", (double)(clock() - start) / CLOCKS_PER_SEC * 1000);
}

int	main(void)
{
	t_scene	scene;

	scene = scene_init("MINIRT", WIDTH, HEIGHT);

	//t_bump_map	bump_maps[] = {
	//	load_bump_map(scene.window, "assets/test.xpm")
	//};

	t_sphere	spheres[] = {
		{(t_vec){0, 0, 0}, 2, 4},
		{(t_vec){0, 5, 0}, 1, 1},
		{(t_vec){4, -4, 3.5}, .5, .25}
	};

	t_plane	planes[] = {
		{(t_vec){0, 0, -4}, (t_vec){0, 0, 1}, NULL},
		//{(t_vec){0, 10, 0}, (t_vec){0, 1, 0}, bump_maps + 0}
	};

	t_cylinder	cylinders[] = {
		{(t_vec){5, 5, 0}, (t_vec){0, 0, 1}, 2, 4, 1}
	};

	t_cone	cones[] = {
		{(t_vec){1, 1, 0}, (t_vec){0, 0, 1}}
	};
	
	t_obj	objects[] = {
		{ray_sphere, RED, (void *)(spheres + 0)},
		{ray_sphere, GREEN, (void *)(spheres + 1)},
		{ray_sphere, BLUE, (void *)(spheres + 2)},
		{ray_plane, GREEN, (void *)(planes + 0)},
		//{ray_plane, RED, (void *)(planes + 1)},
		{ray_cylinder, BLUE, (void *)(cylinders + 0)},
		{ray_cone, RED, (void *)(cones + 0)},
		{NULL, BLACK, NULL}
	};
	scene.obj = objects;

	t_light	lights[] = {
		{{5, -5, 5}, RED, 2}
	};
	scene.lights = lights;
	
	//mlx_hook(scene->window->mlx, 4, 1 << 2, on_button_down, &scene);
	//mlx_hook(scene->window->mlx, 5, 1 << 3, on_button_up, &scene);
	//mlx_hook(scene->window->mlx, 6, 64, on_mouse_move, &scene);
	//mlx_hook(scene->window->mlx, 3, 2, on_key_up, &scene);
	
	render(&scene, &scene.cam, scene.win.buf);
	mlx_loop(scene.win.mlx);
	return (0);
}
