/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 16:33:45 by mberger-          #+#    #+#             */
/*   Updated: 2022/01/17 13:55:54 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VECTOR_H
# define VECTOR_H

# include "vector_operators.h"
# include "struct.h"

static inline t_vec	normalize(const t_vec vec)
{
	return (mult(vec, 1.0f
			/ sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z)));
}

static inline float	dot(const t_vec a, const t_vec b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

static inline float	dot2(const t_vec v)
{
	return (dot(v, v));
}

static inline t_vec	cross(const t_vec a, const t_vec b)
{
	return ((t_vec){
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	});
}

#endif
