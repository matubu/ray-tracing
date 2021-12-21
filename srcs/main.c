/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/16 16:15:51 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/21 16:14:59 by mberger-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

void	err(char *s)
{
	printf("\033[31mError: %s\033[0m\n", s);
	exit(1);
}

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
static unsigned int	ray_scene_color(t_vec *orig, t_vec *ray, t_scene *scene)
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

// TODO binary tree bounding box
void	render(t_scene *scene, int *buf)
{
	clock_t	start = clock();

	t_vec	up = {0, 0, 1};
	t_vec	dir = radian_to_vector(&scene->camera->rot_euler);
	t_vec	cv_right = normalize(cross(&dir, &up));
	t_vec	cv_up = normalize(cross(&cv_right, &dir));
	float	half_x = scene->camera->width / 2.0;
	float	half_y = scene->camera->height / 2.0;
	t_vec	ray;
	t_vec	yr, xr;

	register int	y = scene->camera->height;
	register int	x;
	while (y--)
	{
		yr = mult(&cv_up, (y - half_y) * scene->camera->fov_pixel);
		x = scene->camera->width;
		while (x--)
		{
			xr = mult(&cv_right, (half_x - x) * scene->camera->fov_pixel);
			ray = normalize(add3(&dir, &xr, &yr));
			*buf++ = ray_scene_color(&scene->camera->pos, &ray, scene);
		}
	}
	mlx_put_image_to_window(scene->window->mlx, scene->window->window, \
			scene->window->image, 0, 0);
	printf("rendering took %.2fms\n", (double)(clock() - start) / CLOCKS_PER_SEC * 1000);
}

t_camera	create_camera(int width, int height,
		t_vec pos, t_vec rot, float fov)
{
	return ((t_camera){ pos, rot, width, height, fov / width });
}

t_scene	create_scene(t_camera *camera, int ambient_color, t_obj *obj, t_light *lights)
{
	return ((t_scene){ camera, obj, lights, ambient_color, 0, NULL });
}

int	on_mouse_move(int x, int y, t_scene *scene)
{
	static int	first = 1;
	static int	last[2];

	if (first)
		first = 0;
	else if (scene->button == 1)
	{
		scene->camera->rot_euler.z += (float)(last[0] - x) / 50.0;
		scene->camera->rot_euler.y += (float)(last[1] - y) / 50.0;
		render(scene, scene->window->buffer);
	}
	else if (scene->button == 3)
	{
		t_vec	mx = radian_to_vector(&scene->camera->rot_euler);
		t_vec	mz = mx;
		t_vec	up = {0, 0, 1};
		t_vec	left = {1, 0, 0};
		mx = cross(&mx, &up);
		mz = cross(&mz, &left);
		mx = mult(&mx, (float)(last[0] - x) / 10.0);
		mz = mult(&mz, (float)(last[1] - y) / 10.0);
		scene->camera->pos = add3(&scene->camera->pos, &mx, &mz);
		render(scene, scene->window->buffer);
	}
	else
		first = 1;
	last[0] = x;
	last[1] = y;
	return (1);
}

int	on_button_down(int button, int x, int y, t_scene *scene)
{
	t_vec	rad;

	(void)x;
	(void)y;
	scene->button = button;
	rad = radian_to_vector(&scene->camera->rot_euler);
	if (button == 5)
		rad = mult(&rad, -1);
	if (button == 4 || button == 5)
	{
		scene->camera->pos = sub(&scene->camera->pos, &rad);
		render(scene, scene->window->buffer);
	}
	return (1);
}

int	on_button_up(int button, int x, int y, t_scene *scene)
{
	(void)x;
	(void)y;
	(void)button;
	scene->button = 0;
	return (1);
}

int	on_key_up(int key, t_scene *scene)
{
	if (key == 12 || key == 53 || key == 65307)
		window_close(scene->window);
	return (0);
}

int	main(void)
{
	t_window	window;

	window = window_open("MINIRT", WIDTH, HEIGHT);
	t_camera	camera = create_camera(WIDTH, HEIGHT,
			(t_vec){8, -4, 5.5}, (t_vec){-PI / 4, 0, PI / 4}, M_PI_2);
	
	t_bump_map	bump_maps[] = {
		load_bump_map(&window, "assets/test.xpm")
	};

	t_sphere	spheres[] = {
		{(t_vec){0, 0, 0}, 2, 4},
		{(t_vec){0, 5, 0}, 1, 1},
		{(t_vec){4, -4, 3.5}, .5, .25}
	};

	t_plane	planes[] = {
		{(t_vec){0, 0, -4}, (t_vec){0, 0, 1}, NULL},
		{(t_vec){0, 10, 0}, (t_vec){0, 1, 0}, bump_maps + 0}
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
		{ray_plane, RED, (void *)(planes + 1)},
		{ray_cylinder, BLUE, (void *)(cylinders + 0)},
		{ray_cone, RED, (void *)(cones + 0)},
		{NULL, BLACK, NULL}
	};

	t_light	lights[] = {
		{{5, -5, 5}, RED, 2}
	};

	t_scene	scene = create_scene(&camera, GREY, objects, lights);
	scene.window = &window;
	
	mlx_hook(window.window, 4, 1 << 2, on_button_down, &scene);
	mlx_hook(window.window, 5, 1 << 3, on_button_up, &scene);
	mlx_hook(window.window, 6, 64, on_mouse_move, &scene);
	mlx_hook(window.window, 3, 2, on_key_up, &scene);
	
	render(&scene, scene.window->buffer);
	mlx_loop(window.mlx);
	return (0);
}
