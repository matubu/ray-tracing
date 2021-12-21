/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 16:33:45 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/21 16:33:46 by mberger-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VECTOR_H
# define VECTOR_H

# include "vector_add.h"
# include "struct.h"

static inline t_vec	sub(const t_vec *a, const t_vec *b)
{
	return ((t_vec){a->x - b->x, a->y - b->y, a->z - b->z});
}

static inline t_vec	mult(const t_vec *a, const float fac)
{
	return ((t_vec){a->x * fac, a->y * fac, a->z * fac});
}

static inline t_vec	normalize(const t_vec vec)
{
	return (mult(&vec, q_rsqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z)));
}

static inline float	dot(const t_vec *a, const t_vec *b)
{
	return (a->x * b->x + a->y * b->y + a->z * b->z);
}

static inline t_vec	cross(const t_vec *a, const t_vec *b)
{
	return ((t_vec){
		a->y * b->z - a->z * b->y,
		a->z * b->x - a->x * b->z,
		a->x * b->y - a->y * b->x
	});
}

#endif