/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   maths.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 16:34:02 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/28 14:47:19 by mberger-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MATHS_H
# define MATHS_H
/*
static inline int	max(int a, int b)
{
	if (a > b)
		return (a);
	return (b);
}

static inline int	min(int a, int b)
{
	if (a < b)
		return (a);
	return (b);
}
*/
static inline float	q_rsqrt(float y)
{
	int			i;
	float		x2;
	const float	threehalfs = 1.5F;

	x2 = y * 0.5F;
	i = *(int *)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float *)&i;
	return (y * (threehalfs - (x2 * y * y)));
}

static inline unsigned int	rgbmult(unsigned int color, int fac)
{
	return (
		((((color & (255 << 16)) * fac) & (255 << 24))
			| (((color & (255 << 8)) * fac) & (255 << 16))
			| (((color & (255 << 0)) * fac) & (255 << 8))) >> 8
	);
}

#endif
