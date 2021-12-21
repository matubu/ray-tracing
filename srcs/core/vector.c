/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 14:06:49 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/21 14:16:36 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

inline float	dot(t_vec *a, t_vec *b)
{
	return (a->x * b->x + a->y * b->y + a->z * b->z);
}

inline t_vec	sub(t_vec *a, t_vec *b)
{
	return ((t_vec){a->x - b->x, a->y - b->y, a->z - b->z});
}

inline t_vec	mult(t_vec *a, float fac)
{
	return ((t_vec){a->x * fac, a->y * fac, a->z * fac});
}

inline t_vec	add3(t_vec *a, t_vec *b, t_vec *c)
{
	return ((t_vec){a->x + b->x + c->x, a->y + b->y + c->y, a->z + b->z + c->z});
}

inline t_vec	add(t_vec *a, t_vec *b)
{
	return ((t_vec){a->x + b->x, a->y + b->y, a->z + b->z});
}

inline t_vec	cross(t_vec *a, t_vec *b)
{
	return ((t_vec){a->y * b->z - a->z * b->y, a->z * b->x - a->x * b->z, a->x * b->y - a->y * b->x});
}

inline t_vec	reflect(t_vec *ray, t_vec *normal)
{
	t_vec	tmp;

	tmp = mult(normal, 2 * dot(ray, normal));
	return (sub(ray, &tmp));
}

inline t_vec	normalize(t_vec vec)
{
	return (mult(&vec, q_rsqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z)));
}

inline t_vec	radian_to_vector(t_vec *rot)
{
	float	sin_z = sinf(rot->z),
		cos_z = cosf(rot->z),
		sin_y = sinf(rot->y),
		sin_x = sinf(rot->x),
		cos_x = cosf(rot->x);

	return ((t_vec){
		-cos_z * sin_y * sin_x - sin_z * cos_x,
		-sin_z * sin_y * sin_x + cos_z * cos_x,
		cosf(rot->y) * sin_x
	});
}

