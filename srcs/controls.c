/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   controls.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/29 12:40:05 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/29 13:53:40 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

void	controls_init(t_scene *scene)
{
	int	i;

	i = -1;
	while (++i < 1024)
		scene->keys[i] = 0;
}

int	controls_listen(t_scene *scene)
{
	if (scene->keys[KEY_ESCAPE])
		hook_close(scene);
	if (scene->keys[KEY_W] || scene->keys[KEY_UP])
		make_straight_move(scene, 1);
	if (scene->keys[KEY_S] || scene->keys[KEY_DOWN])
		make_straight_move(scene, -1);
	if (scene->keys[KEY_A] || scene->keys[KEY_LEFT])
		make_side_move(scene, -1);
	if (scene->keys[KEY_D] || scene->keys[KEY_RIGHT])
		make_side_move(scene, 1);
	if (scene->keys[KEY_SPACE])
		make_vertical_move(scene, -1);
	if (scene->keys[KEY_LSHIFT])
		make_vertical_move(scene, 1);
	return (0);
}

void	make_straight_move(t_scene *scene, int direction)
{
	t_vec	rad;

	rad = radian_to_vector(&scene->cam.rot_euler);
	rad = mult(&rad, 100);
	if (direction == 1)
		rad = mult(&rad, -1);
	scene->cam.pos = sub(&scene->cam.pos, &rad);
	render(scene, &scene->win, &scene->cam, scene->win.buf);
}

void	make_side_move(t_scene *scene, int direction)
{
	t_vec	m[2];
	float	velocity;
	float	acceleration;

	velocity = 25.0f;
	acceleration = velocity * direction;
	m[0] = radian_to_vector(&scene->cam.rot_euler);
	m[0] = cross(&m[0], &((t_vec){0, 0, 1}));
	m[0] = mult(&m[0], acceleration);
	m[1] = (t_vec){0, 0, 0};
	scene->cam.pos = add3(&scene->cam.pos, &m[0], &m[1]);
	render(scene, &scene->win, &scene->cam, scene->win.buf);
}

void	make_vertical_move(t_scene *scene, int direction)
{
	t_vec	m[2];
	float	velocity;
	float	acceleration;

	velocity = 25.0f;
	acceleration = velocity * direction;
	m[0] = radian_to_vector(&scene->cam.rot_euler);
	m[1] = cross(&m[0], &((t_vec){1, 0, 0}));
	m[1] = mult(&m[1], acceleration);
	m[0] = (t_vec){0, 0, 0};
	scene->cam.pos = add3(&scene->cam.pos, &m[0], &m[1]);
	render(scene, &scene->win, &scene->cam, scene->win.buf);
}
