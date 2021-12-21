/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 16:47:32 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/21 17:03:38 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

t_camera	camera_init(int width, int height, t_vec pos, t_vec rot)
{
	t_camera	camera;

	camera.pos = pos;
	camera.rot_euler = rot;
	camera.width = width;
	camera.height = height;
	camera.fov_pixel = M_PI_2 / width;
	return (camera);
}
