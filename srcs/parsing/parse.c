/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 13:42:38 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/28 14:04:10 by acoezard         ###   ########.fr       */
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

/*
t_bump_map	load_bump_map(t_window *window, char *filename)
{
	t_bump_map	img;
	int			null;

	img.img = mlx_xpm_file_to_image(window->mlx, filename, &img.width, &img.height);
	if (img.img == NULL)
		err("could            not load image");
	img.buf = (int *)mlx_get_data_addr(img.img, &null, &null, &null);
	return (img);
}
*/

//TODO initialize scene at first
//TODO remove hard codded scene initialization
//		and replace by check of number of camera, lights, ambient light ...
t_scene	parse(int argc, char **argv)
{
	t_scene		scene;
	const int	fd = open_file(argc, argv);
	char		**splits;

	scene.obj_count = 0;
	scene.lights_count = 0;
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
	scene.win = window_open("miniRT", scene.cam.width, scene.cam.height);
	scene.button = 0;
	scene.obj[scene.obj_count].func = NULL;
	return (scene);
}
