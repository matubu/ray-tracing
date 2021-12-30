/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/28 17:04:28 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/30 12:33:25 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

void	push_obj(t_scene *scene, t_obj obj)
{
	if (scene->obj_count == 1022)
		err("too many objects");
	obj.ka = 0.7f;
	obj.kd = 0.5f;
	obj.ks = 0.8f;
	obj.shinyness = 2.0f;
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
