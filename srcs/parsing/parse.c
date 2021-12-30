/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 13:42:38 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/30 12:53:59 by mberger-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

int	open_file(int argc, char **argv)
{
	int	fd;

	if (argc != 2)
		err("usage: ./minirt [minirtfile]");
	fd = open(argv[1], O_DIRECTORY);
	if (fd != -1)
		err("file is a directory");
	close(fd);
	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		err("file can't be open");
	return (fd);
}

t_window	window_open(char *name, int width, int height)
{
	t_window	win;
	int			null;

	win.width = width;
	win.height = height;
	win.ptr = mlx_init();
	win.win = mlx_new_window(win.ptr, width, height, name);
	win.img = mlx_new_image(win.ptr, width, height);
	win.buf = (int *)mlx_get_data_addr(win.img, &null, &null, &null);
	return (win);
}

//TODO initialize scene at first
//TODO remove hard codded scene initialization
//		and replace by check of number of camera, lights, ambient light ...
t_scene	parse(int argc, char **argv)
{
	t_scene		scene;
	const int	fd = open_file(argc, argv);
	char		**splits;

	scene = (t_scene){.obj_count = 0, .lights_count = 0, .cam_count = 0,
		.ambient_count = 0, .win = window_open("miniRT", WIDTH, HEIGHT)};
	while (1)
	{
		splits = split(gnl(fd), ' ');
		if (splits == NULL)
			break ;
		if (splits && *splits)
			parse_line(*splits, splits + 1, &scene);
		free_splits(splits, -1);
	}
	close(fd);
	if (scene.lights_count < 1)
		err("not enough lights");
	if (scene.cam_count < 1)
		err("a camera is missing");
	if (scene.ambient_count < 1)
		err("not enough ambient lights");
	scene.obj[scene.obj_count].func = NULL;
	return (scene);
}
