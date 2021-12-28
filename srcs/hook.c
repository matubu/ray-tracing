/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hook.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/22 12:42:08 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/28 15:38:28 by mberger-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

int	hook_mouse_move(int x, int y, t_scene *scene)
{
	static int	first = 1;
	static int	last[2];
	t_vec		mx;
	t_vec		mz;

	if (first)
		first = 0;
	else if (scene->button == 1)
	{
		scene->cam.rot_euler.z += (float)(last[0] - x) / 50.0;
		scene->cam.rot_euler.y += (float)(last[1] - y) / 50.0;
		render(scene, &scene->win, &scene->cam, scene->win.buf);
	}
	else if (scene->button == 3 || scene->button == 2)
	{
		mx = radian_to_vector(&scene->cam.rot_euler);
		mz = cross(&mx, &((t_vec){1, 0, 0}));
		mx = cross(&mx, &((t_vec){0, 0, 1}));
		mx = mult(&mx, (float)(last[0] - x));
		mz = mult(&mz, (float)(last[1] - y));
		scene->cam.pos = add3(&scene->cam.pos, &mx, &mz);
		render(scene, &scene->win, &scene->cam, scene->win.buf);
	}
	else
		first = 1;
	last[0] = x;
	last[1] = y;
	return (1);
}

int	hook_button_down(int button, int x, int y, t_scene *scene)
{
	t_vec	rad;

	(void)x;
	(void)y;
	scene->button = button;
	rad = radian_to_vector(&scene->cam.rot_euler);
	rad = mult(&rad, 10);
	if (button == 5)
		rad = mult(&rad, -1);
	if (button == 4 || button == 5)
	{
		scene->cam.pos = sub(&scene->cam.pos, &rad);
		render(scene, &scene->win, &scene->cam, scene->win.buf);
	}
	return (1);
}

int	hook_button_up(int button, int x, int y, t_scene *scene)
{
	(void)x;
	(void)y;
	(void)button;
	scene->button = 0;
	return (1);
}

int	hook_close(t_scene *scene)
{
	mlx_destroy_window(scene->win.ptr, scene->win.win);
	exit(0);
	return (0);
}

int	hook_key_up(int key, t_scene *scene)
{
	if (key == 12 || key == 53 || key == 65307)
		hook_close(scene);
	return (0);
}
