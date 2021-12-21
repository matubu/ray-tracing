/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:44:06 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/21 17:26:35 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

t_window	window_open(char *title, int width, int height)
{
	t_window	window;
	int			n;

	window.width = width;
	window.height = height;
	window.mlx = mlx_init();
	window.window = mlx_new_window(window.mlx, width, height, title);
	window.image = mlx_new_image(window.mlx, width, height);
	window.buffer = (int *) mlx_get_data_addr(window.image, &n, &n, &n);
	return (window);
}

void	window_loop(t_scene *scene, int (*hook)())
{
	if (scene == NULL || scene->window == NULL)
		return ;
	mlx_loop_hook(scene->window->mlx, hook, scene);
	mlx_loop(scene->window->mlx);
}

void	window_update(t_window *window)
{
	if (window == NULL)
		return ;
	mlx_put_image_to_window(\
		window->mlx, \
		window->window, \
		window->image, \
		0, 0 \
	);
}

void	window_close(t_window *window)
{
	if (window != NULL)
	{
		mlx_destroy_window(window->mlx, window->window);
		exit(EXIT_SUCCESS);
	}
}
