/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 13:42:38 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/21 17:29:59 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

void err(char *s)
{
	printf("\033[31mError\033[0m: %s\n", s);
	exit(1);
}
/*
void	check_args(int argc, char **argv)
{
	(void) argv;
	if (argc != 2)
		err("usage: ./minirt [minirtfile]");
}

void	check_file(char *filename)
{
	int	fd;

	fd = open(filename, O_DIRECTORY);
	if (fd != -1)
		err("file is a directory.");
	close(fd);
	fd = open(filename, O_RDONLY);
	if (fd == -1)
		err("file is invalid.");
	close(fd);
}

t_scene	parse(int ac, char **av)
{
	t_scene	scene;
	int		fd;

	check_args(ac, av);
	check_file(av[1]);
	fd = open(av[1], O_RDONLY);
	scene = scene_init("MINIRT", WIDTH, HEIGHT);
	close(fd);
	return (scene);
}
*/