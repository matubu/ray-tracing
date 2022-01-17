/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 16:31:31 by mberger-          #+#    #+#             */
/*   Updated: 2022/01/17 15:48:40 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RAY_H
# define RAY_H

# include "struct.h"
# include "vector.h"

static inline void	ray_sphere(const t_vec *orig, const t_vec *ray,
		const t_obj *obj, t_hit *hit)
{
	const t_vec	oc = sub(*orig, obj->sphere.pos);
	const float	a = dot(*ray, *ray);
	const float	b = dot(oc, *ray);
	const float	c = dot(oc, oc) - obj->sphere.rad * obj->sphere.rad;
	const float	d = b * b - a * c;

	if (d <= EPSILON)
		return ((void)(hit->dist = -1));
	hit->dist = (-b - sqrt(b * b - a * c)) / a;
	hit->pos = add(*orig, mult(*ray, hit->dist));
	hit->normal = normalize(sub(hit->pos, obj->sphere.pos));
}

static inline void	ray_plane(const t_vec *orig, const t_vec *ray,
		const t_obj *obj, t_hit *hit)
{
	const float	d = dot(obj->plane.normal, *ray);

	if (d >= EPSILON && d <= EPSILON)
		return ((void)(hit->dist = -1));
	hit->pos = sub(obj->plane.pos, *orig);
	hit->dist = dot(hit->pos, obj->plane.normal) / d;
	hit->pos = mult(*ray, hit->dist);
	hit->pos = add(*orig, hit->pos);
	hit->normal = obj->plane.normal;
}

static inline void	ray_cylinder(const t_vec *orig, const t_vec *ray,
		const t_obj *obj, t_hit *hit)
{
	const t_vec	ca = sub(add(obj->cylinder.pos, \
		mult(obj->cylinder.dir, obj->cylinder.height)), obj->cylinder.pos);
	const t_vec	oc = sub(*orig, obj->cylinder.pos);
	const float	caca = dot2(ca);
	const float	card = dot(ca, *ray);
	const float	caoc = dot(ca, oc);
	const float	a = caca - card * card;
	const float	b = caca * dot(oc, *ray) - caoc * card;
	const float	h = b * b - a * (caca * dot2(oc) - caoc * caoc \
		- obj->cylinder.rad * obj->cylinder.rad * caca);

	if (h < 0.0)
		return ((void)(hit->dist = -1));
	*(float *)&h = sqrtf(h);
	hit->dist = (-b - h) / a;
	const float	y = caoc + hit->dist * card;
	if (y > 0.0 && y < caca)
		hit->normal = normalize(mult(add(oc, sub(mult(*ray, hit->dist), \
			vec_div(vec_div(ca, y), caca))), obj->cylinder.rad));
	else
	{
		hit->dist = (caca * !(y < 0.0) - caoc) / card;
		if (fabs(b + a * hit->dist) < h)
			hit->normal = normalize(vec_div(mult(ca, sign(y)), caca));
		else
			return ((void)(hit->dist = -1));
	}
	hit->pos = add(*orig, mult(*ray, hit->dist));
}

static inline void	ray_cone(const t_vec *orig, const t_vec *ray,
		t_obj *obj, t_hit *hit)
{
	const t_vec	co = sub(*orig, obj->cone.pos);
	const float	a = powf(dot(*ray, obj->cone.dir), 2.0f) - obj->cone.rad2;
	const float	b = 2.0f * (dot(*ray, obj->cone.dir) * dot(co, obj->cone.dir)
			- dot(*ray, co) * obj->cone.rad2);
	const float	c = powf(dot(co, obj->cone.dir), 2.0f)
		- dot(co, co) * obj->cone.rad2;
	const float	det = b * b - (4.0f * a * c);

	if (det < 0.0f)
		return ((void)(hit->dist = -1));
	*(float *)&det = sqrtf(det);
	const float	t = (float)(-b - det) / (float)(2.0f * a);
	const float	t2 = (float)(-b + det) / (float)(2.0f * a);
	if (t < EPSILON || (t2 > EPSILON && t2 < t))
		*(float *)&t = t2;
	if (t < 0.0)
		return ((void)(hit->dist = -1));
	const t_vec	cp = sub(add(*orig, mult(*ray, t)), obj->cone.pos);
	const float	h = dot(cp, obj->cone.dir);
	if (h < 0.0 || h > obj->cone.height)
		return ((void)(hit->dist = -1));
	hit->dist = t;
	hit->pos = add(*orig, mult(*ray, hit->dist));
	hit->normal = normalize(sub(vec_div(mult(cp, \
			dot(obj->cone.dir, cp)), dot2(cp)), obj->cone.dir));
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
		obj->func(orig, ray, obj, &hit);
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
