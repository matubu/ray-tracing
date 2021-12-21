/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 13:42:38 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/21 17:29:21 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

t_scene	*parse(int ac, char **av)
{
	t_scene	*scene;
	int		fd;

	if (!check_args(ac, av) || !check_file(av[1]))
		return (NULL);
	fd = open(av[1], O_RDONLY);
	scene = scene_init("MINRT", 1080, 720);
	close(fd);
	return (scene);
}
