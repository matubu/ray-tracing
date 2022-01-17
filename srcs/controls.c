/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   controls.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/29 12:40:05 by acoezard          #+#    #+#             */
/*   Updated: 2022/01/17 13:45:58 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

void	controls_init(t_scene *scene)
{
	int	i;

	i = -1;
	while (++i < 256)
		scene->keys[i] = 0;
}

void	make_move(t_scene *scene, int axis, int dist)
{
	t_vec	dir;
	t_vec	move;

	dir = radian_to_vector(&scene->cam.rot);
	move = dir;
	if (axis >= 1)
		move = cross(move, (t_vec){0, 0, 1});
	if (axis == 2)
		move = cross(move, dir);
	move = mult(move, dist);
	scene->cam.pos = add(scene->cam.pos, move);
	render(scene, &scene->win, &scene->cam, scene->win.buf);
}

int	controls_listen(t_scene *scene)
{
	if (scene->keys[KEY_ESCAPE])
		hook_close(scene);
	if (scene->keys[KEY_W] || scene->keys[KEY_UP])
		make_move(scene, 0, 10);
	if (scene->keys[KEY_S] || scene->keys[KEY_DOWN])
		make_move(scene, 0, -10);
	if (scene->keys[KEY_A] || scene->keys[KEY_LEFT])
		make_move(scene, 1, -10);
	if (scene->keys[KEY_D] || scene->keys[KEY_RIGHT])
		make_move(scene, 1, 10);
	if (scene->keys[KEY_SPACE])
		make_move(scene, 2, 5);
	if (scene->keys[KEY_LSHIFT])
		make_move(scene, 2, -5);
	return (0);
}
