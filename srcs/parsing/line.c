/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   line.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matubu <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/26 21:05:43 by matubu            #+#    #+#             */
/*   Updated: 2022/01/17 12:32:28 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

//parse a number from a string
static float	num(char ***args)
{
	char	*s;
	int		point;
	double	dec;
	double	exp;
	int		neg;

	s = *(*args)++;
	if (s == NULL)
		err("missing float");
	dec = 0;
	point = 0;
	exp = 1;
	neg = (*s == '-' && s++) * 2 - 1;
	while (*s)
	{
		if (point)
			exp *= 10;
		if (*s >= '0' && *s <= '9')
			dec = dec * 10.0 - *s++ + '0';
		else if (*s != '.' || point)
			err("invalid number");
		else
			point = (1 && s++);
	}
	return (dec * neg / exp);
}

//parse a 3d vector from a string
static t_vec	vec(char ***args)
{
	char	**splits;
	t_vec	vec;

	if (**args == NULL)
		err("missing vector");
	splits = split(**args, ',');
	if (!splits || !splits[0] || !splits[1] || !splits[2] || splits[3])
		err("invalid vector");
	vec = (t_vec){num(&splits), num(&splits), num(&splits)};
	free_splits(splits - 3, -1);
	(*args)++;
	return (vec);
}

//parse a color from a string
static int	col(char ***args)
{
	char	**splits;
	int		color;

	if (**args == NULL)
		err("missing color");
	splits = split(**args, ',');
	if (!splits || !splits[0] || !splits[1] || !splits[2] || splits[3])
		err("invalid color");
	color = ((int)num(&splits) & 255) << 16
		| ((int)num(&splits) & 255) << 8
		| ((int)num(&splits) & 255) << 0;
	free_splits(splits - 3, -1);
	(*args)++;
	return (color);
}

void	parse_cylinder(char ***arg, t_scene *scene)
{
	t_cylinder	cylinder;

	cylinder.pos = vec(arg);
	cylinder.dir = vec(arg);
	cylinder.rad = num(arg);
	cylinder.height = num(arg);
	cylinder.ca = sub(add(cylinder.pos, \
		mult(cylinder.dir, cylinder.height)), cylinder.pos);
	cylinder.caca = dot2(cylinder.ca);
	push_obj(arg, scene, (t_obj){.func = ray_cylinder,
		.cylinder = cylinder, .color = col(arg)});
}

void	parse_line(char *type, char **arg, t_scene *scene)
{
	if (type[0] == 'A' && type[1] == '\0')
		push_ambient(scene, (t_ambient){num(&arg), col(&arg)});
	else if (type[0] == 'C' && type[1] == '\0')
		push_camera(scene, (t_camera){vec(&arg), vec(&arg),
			WIDTH, HEIGHT, num(&arg) / 180.0 * PI / WIDTH});
	else if (type[0] == 'L' && type[1] == '\0')
		push_light(scene, (t_light){vec(&arg), num(&arg), col(&arg)});
	else if (type[0] == 's' && type[1] == 'p' && type[2] == '\0')
		push_obj(&arg, scene, (t_obj){.func = ray_sphere, .sphere
			= (t_sphere){vec(&arg), num(&arg)}, .color = col(&arg)});
	else if (type[0] == 'p' && type[1] == 'l' && type[2] == '\0')
		push_obj(&arg, scene, (t_obj){.func = ray_plane,
			.plane = (t_plane){vec(&arg), vec(&arg)}, .color = col(&arg)});
	else if (type[0] == 'c' && type[1] == 'y' && type[2] == '\0')
		parse_cylinder(&arg, scene);
	else if (type[0] == 'c' && type[1] == 'o' && type[2] == '\0')
		push_obj(&arg, scene, (t_obj){.func = ray_cone,
			.cone = (t_cone){vec(&arg), vec(&arg), powf(num(&arg), 2.0),
			num(&arg)}, .color = col(&arg)});
	else
		err("invalid object type");
	if (*arg)
		err("too many field");
}
