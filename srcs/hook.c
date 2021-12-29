/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hook.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/22 12:42:08 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/29 13:46:13 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

int	hook_mouse_move(int x, int y, t_scene *scene)
{
	static int		first = 1;
	static t_vec	last;

	if (!first && scene->button == MOUSE_LEFT)
	{
		scene->cam.rot_euler.z += (float)(last.x - x) / 50.0;
		scene->cam.rot_euler.y += (float)(last.y - y) / 50.0;
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
	(void)x;
	(void)y;
	scene->button = button;
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

int	hook_key_down(int key, t_scene *scene)
{
	scene->keys[key] = 1;
	return (0);
}

int	hook_key_up(int key, t_scene *scene)
{
	(void) scene;
	scene->keys[key] = 0;
	return (0);
}
