/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hook.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/22 12:42:08 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/28 16:48:09 by mberger-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

int	hook_mouse_move(int x, int y, t_scene *scene)
{
	static int		first = 1;
	static t_vec	last;
	t_vec			m[2];

	if (!first && scene->button == 1)
	{
		scene->cam.rot_euler.z += (float)(last.x - x) / 50.0;
		scene->cam.rot_euler.y += (float)(last.y - y) / 50.0;
		render(scene, &scene->win, &scene->cam, scene->win.buf);
	}
	else if (!first && (scene->button == 3 || scene->button == 2))
	{
		m[0] = radian_to_vector(&scene->cam.rot_euler);
		m[1] = cross(&m[0], &((t_vec){1, 0, 0}));
		m[0] = cross(&m[0], &((t_vec){0, 0, 1}));
		m[1] = mult(&m[1], (float)(last.y - y));
		m[0] = mult(&m[0], (float)(last.x - x));
		scene->cam.pos = add3(&scene->cam.pos, &m[0], &m[1]);
		render(scene, &scene->win, &scene->cam, scene->win.buf);
	}
	first = 0;
	if (scene->button == 0)
		first = 1;
	last = (t_vec){x, y, 0};
	return (1);
}

int	hook_button_down(int button, int x, int y, t_scene *scene)
{
	t_vec	rad;

	(void)x;
	(void)y;
	scene->button = button;
	rad = radian_to_vector(&scene->cam.rot_euler);
	rad = mult(&rad, 100);
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
