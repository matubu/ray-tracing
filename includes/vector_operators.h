/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector_add.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 16:35:37 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/21 16:35:38 by mberger-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VECTOR_OPERATORS_H
# define VECTOR_OPERATORS_H

# include "struct.h"

static inline t_vec	sub(const t_vec a, const t_vec b)
{
	return ((t_vec){a.x - b.x, a.y - b.y, a.z - b.z});
}

static inline t_vec	add3(const t_vec a, const t_vec b, const t_vec c)
{
	return ((t_vec){
		a.x + b.x + c.x,
		a.y + b.y + c.y,
		a.z + b.z + c.z
	});
}

static inline t_vec	add(const t_vec a, const t_vec b)
{
	return ((t_vec){a.x + b.x, a.y + b.y, a.z + b.z});
}

static inline t_vec	mult(const t_vec a, const float fac)
{
	return ((t_vec){a.x * fac, a.y * fac, a.z * fac});
}

#endif
