/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/28 17:04:28 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/30 16:50:32 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

t_bump_map	load_bump_map(t_scene *scene, char ***args)
{
	t_bump_map	img;
	int			null;

	if (**args == NULL)
		return ((t_bump_map){NULL, NULL, 0, 0});
	img.img = mlx_xpm_file_to_image(scene->win.ptr, **args,
			&img.width, &img.height);
	if (img.img == NULL)
		err("could not load image");
	img.buf = (int *)mlx_get_data_addr(img.img, &null, &null, &null);
	(*args)++;
	return (img);
}

void	push_obj(char ***args, t_scene *scene, t_obj obj)
{
	if (scene->obj_count == 1022)
		err("too many objects");
	obj.ka = 0.7f;
	obj.kd = 0.5f;
	obj.ks = 0.8f;
	obj.shinyness = 16.0f;
	obj.bump_map = load_bump_map(scene, args);
	scene->obj[scene->obj_count++] = obj;
}

void	push_light(t_scene *scene, t_light light)
{
	if (scene->lights_count == 62)
		err("too many lights");
	scene->lights[scene->lights_count++] = light;
}

void	push_camera(t_scene *scene, t_camera cam)
{
	if (scene->cam_count == 1)
		err("too many camera");
	scene->cam = cam;
	scene->cam_count++;
}

void	push_ambient(t_scene *scene, t_ambient ambient)
{
	if (scene->ambient_count == 1)
		err("too many ambient lights");
	scene->ambient = ambient;
	scene->ambient_count++;
}
