/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hook.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/22 12:42:08 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/30 16:47:18 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

int	hook_mouse_move(int x, int y, t_scene *scene)
{
	static int		first = 1;
	static t_vec	last;
	const float		yy = y - last.y;

	if (!first)
	{
		scene->cam.rot.z += (float)(last.x - x) / 50.0f;
		scene->cam.rot.x += yy / 50.0f * cos(scene->cam.rot.z);
		scene->cam.rot.y += yy / 50.0f * sin(scene->cam.rot.z);
		render(scene, &scene->win, &scene->cam, scene->win.buf);
	}
	first = 0;
	last = (t_vec){x, y, 0};
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
	return (scene->keys[key] = 1);
}

int	hook_key_up(int key, t_scene *scene)
{
	return (scene->keys[key] = 0);
}
