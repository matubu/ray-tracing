/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 16:31:31 by mberger-          #+#    #+#             */
/*   Updated: 2022/01/18 11:56:33 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RAY_H
# define RAY_H

# include "struct.h"
# include "vector.h"

static inline void	ray_sphere(const t_vec *orig, const t_vec *ray,
		const t_sphere *sphere, t_hit *hit)
{
	const t_vec	oc = sub(*orig, sphere->pos);
	const float	a = dot(*ray, *ray);
	const float	b = dot(oc, *ray);
	const float	c = dot(oc, oc) - sphere->rad * sphere->rad;
	const float	d = b * b - a * c;

	if (d <= EPSILON)
		return ((void)(hit->dist = -1));
	hit->dist = (-b - sqrt(b * b - a * c)) / a;
	hit->pos = add(*orig, mult(*ray, hit->dist));
	hit->normal = normalize(sub(hit->pos, sphere->pos));
}

static inline void	ray_plane(const t_vec *orig, const t_vec *ray,
		const t_plane *plane, t_hit *hit)
{
	const float	d = dot(plane->normal, *ray);

	if (d >= EPSILON && d <= EPSILON)
		return ((void)(hit->dist = -1));
	hit->dist = dot(sub(plane->pos, *orig), plane->normal) / d;
	hit->pos = add(*orig, mult(*ray, hit->dist));
	hit->normal = plane->normal;
}

static inline void	ray_cylinder(const t_vec *orig, const t_vec *ray,
		const t_cylinder *cylinder, t_hit *hit)
{
	const t_vec	oc = sub(*orig, cylinder->pos);
	const float	card = dot(cylinder->ca, *ray) \
	, caoc = dot(cylinder->ca, oc), a = cylinder->caca - card * card;
	const float	b = cylinder->caca * dot(oc, *ray) - caoc \
	* card, h = b * b - a * (cylinder->caca * dot2(oc) - caoc * caoc \
		- cylinder->rad * cylinder->rad * \
		cylinder->caca), y;

	if (h < 0.0)
		return ((void)(hit->dist = -1));
	*(float *)&h = sqrtf(h);
	hit->dist = (-b - h) / a;
	*(float *)&y = caoc + hit->dist * card;
	if (y > 0.0 && y < cylinder->caca)
		hit->normal = normalize(mult(add(oc, sub(mult(*ray, hit->dist), mult(\
mult(cylinder->ca, y), 1.0 / cylinder->caca))), 1.0 / cylinder->rad));
	else
	{
		hit->dist = (cylinder->caca * !(y < 0.0) - caoc) / card;
		if (fabs(b + a * hit->dist) >= h)
			return ((void)(hit->dist = -1));
		hit->normal = normalize(mult(mult(cylinder->ca, sign(y)), \
			1.0 / cylinder->caca));
	}
	hit->pos = add(*orig, mult(*ray, hit->dist));
}

static inline void	ray_cone(const t_vec *orig, const t_vec *ray,
		t_cone *cone, t_hit *hit)
{
	const t_vec	co = sub(*orig, cone->pos);
	const float	a = powf(dot(*ray, cone->dir), 2.0f) - cone->rad2;
	const float	b = 2.0f * (dot(*ray, cone->dir) * dot(co, cone->dir)
			- dot(*ray, co) * cone->rad2);
	const float	det = b * b - (4.0f * a * (powf(dot(co, cone->dir), 2.0f) \
		- dot2(co) * cone->rad2));
	float		t;

	if (det < 0.0f)
		return ((void)(hit->dist = -1));
	*(float *)&det = sqrtf(det);
	hit->dist = (float)(-b - det) / (float)(2.0f * a);
	t = (float)(-b + det) / (float)(2.0f * a);
	if (hit->dist < 0.0f || (t > 0.0f && t < hit->dist))
		hit->dist = t;
	if (hit->dist < 0.0f)
		return ((void)(hit->dist = -1));
	*(t_vec *)&co = sub(add(*orig, mult(*ray, hit->dist)), cone->pos);
	t = dot(co, cone->dir);
	if (t < 0.0 || t > cone->height)
		return ((void)(hit->dist = -1));
	hit->pos = add(*orig, mult(*ray, hit->dist));
	hit->normal = normalize(sub(mult(mult(co, t), 1.0 / dot2(co)),
				cone->dir));
}

static inline int	ray_scene(const t_vec *orig, const t_vec *ray,
		const t_scene *scene, t_hit *closest)
{
	t_hit			hit;
	register t_obj	*obj;

	obj = (t_obj *)scene->obj;
	closest->dist = -1;
	while (obj->func)
	{
		hit.dist = -1;
		obj->func(orig, ray, (void *)&obj->ptr, &hit);
		if (hit.dist > CAMERA_CLIP_START
			&& (closest->dist == -1 || hit.dist < closest->dist))
		{
			closest->dist = hit.dist;
			closest->pos = hit.pos;
			closest->normal = hit.normal;
			closest->obj = obj;
		}
		obj++;
	}
	return (closest->dist != -1);
}

#endif
