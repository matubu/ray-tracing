/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scene.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 16:36:43 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/22 10:33:25 by mberger-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

t_camera	camera_init(int width, int height, t_vec pos, t_vec rot)
{
	return ((t_camera){
		.pos = pos,
		.rot_euler = rot,
		.width = width,
		.height = height,
		.fov_pixel = M_PI_2 / width
	});
}

t_window	window_open(char *name, int width, int height)
{
	t_window	win;
	int			null;

	win.width = width;
	win.height = height;
	win.mlx = mlx_init();
	win.win = mlx_new_window(win.mlx, width, height, name);
	win.img = mlx_new_image(win.mlx, width, height);
	win.buf = (int *)mlx_get_data_addr(win.img, &null, &null, &null);
	return (win);
}

t_scene	scene_init(char *title, int width, int height)
{
	return ((t_scene){
		.win = window_open(title, width, height),
		.cam = camera_init(WIDTH, HEIGHT,
			(t_vec){8, -4, 5.5},
			(t_vec){-PI / 4, 0, PI / 4}
		),
		.ambient_color = 0xebebeb,
		.button = 0
	});
}