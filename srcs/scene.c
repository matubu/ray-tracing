/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scene.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 16:36:43 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/21 17:06:45 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

t_scene	*scene_init(char *title, int width, int height)
{
	t_scene		*scene;
	t_window	window;
	t_camera	camera;

	scene = malloc(sizeof(t_scene));
	window = window_open(title, width, height);
	camera = camera_init(WIDTH, HEIGHT, \
		(t_vec){8, -4, 5.5}, \
		(t_vec){-PI / 4, 0, PI / 4} \
	);
	scene->window = &window;
	scene->camera = &camera;
	scene->ambient_color = 0xebebeb;
	scene->button = 0;
	return (scene);
}

int	scene_update(t_scene *scene)
{
	window_update(scene->window);
	return (0);
}
