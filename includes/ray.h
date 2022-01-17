/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 16:31:31 by mberger-          #+#    #+#             */
/*   Updated: 2022/01/17 12:34:03 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RAY_H
# define RAY_H

# include "struct.h"
# include "vector.h"

static inline void	ray_sphere(const t_vec *orig, const t_vec *ray,
		const t_obj *obj, t_hit *hit)
{
	const t_vec	oc = sub(orig, &obj->sphere.pos);
	const float	a = dot(ray, ray);
	const float	b = dot(&oc, ray);
	const float	c = dot(&oc, &oc) - obj->sphere.rad * obj->sphere.rad;
	const float	d = b * b - a * c;

	if (d <= EPSILON)
		return ((void)(hit->dist = -1));
	hit->dist = (-b - sqrt(b * b - a * c)) / a;
	hit->pos = mult(ray, hit->dist);
	hit->pos = add(orig, &hit->pos);
	hit->normal = normalize(sub(&hit->pos, &obj->sphere.pos));
}

static inline void	ray_plane(const t_vec *orig, const t_vec *ray,
		const t_obj *obj, t_hit *hit)
{
	const float	d = dot(&obj->plane.normal, ray);

	if (d >= EPSILON && d <= EPSILON)
		return ((void)(hit->dist = -1));
	hit->pos = sub(&obj->plane.pos, orig);
	hit->dist = dot(&hit->pos, &obj->plane.normal) / d;
	hit->pos = mult(ray, hit->dist);
	hit->pos = add(orig, &hit->pos);
	hit->normal = obj->plane.normal;
}

static inline void	ray_cylinder(const t_vec *orig, const t_vec *ray,
		const t_obj *obj, t_hit *hit)
{
	const float	a = (ray->x * ray->x) + (ray->z * ray->z);
	const float	b = 2 * (ray->x * (orig->x - obj->cylinder.pos.x)
			+ ray->z * (orig->z - obj->cylinder.pos.z));
	const float	c = (orig->x - obj->cylinder.pos.x)
		* (orig->x - obj->cylinder.pos.x)
		+ (orig->z - obj->cylinder.pos.z) * (orig->z - obj->cylinder.pos.z)
		- obj->cylinder.rad * obj->cylinder.rad;
	const float	delta = b * b - 4 * (a * c);

	if (delta < 0.001)
		return ((void)(hit->dist = -1));
	hit->dist = fmin((-b - sqrt(delta)) / (2 * a),
			(-b + sqrt(delta)) / (2 * a));
	hit->pos = mult(ray, hit->dist);
	hit->pos = add(orig, &hit->pos);
	hit->normal = normalize((t_vec){hit->pos.x - obj->cylinder.pos.x, 0, hit->pos.z - obj->cylinder.pos.z});
/*	const float	d = dot(obj->sphere.dir, P) + t*(N.V);
	const float	t = (d - dot(obj->sphere.dir, P)) / dot(obj->sphere.dir, ray);

	return (t);
	const t_vec	pdp = sub(&obj->cylinder.dir, &obj->cylinder.pos);
	const t_vec	tmp = sub(orig, &obj->sphere.pos);
	const t_vec	eyexpdp = cross(&tmp, &pdp);
	const t_vec	rdxpdp = cross(ray, &pdp);
	const float	a = dot(&rdxpdp, &rdxpdp);
	const float	b = 2 * dot(&rdxpdp, &eyexpdp);
	const float	c = dot(&eyexpdp, &eyexpdp) - (obj->cylinder.rad * obj->cylinder.rad * dot(&pdp, &pdp));
	const float	delta = sqrt((b * b) - (4.0 * a * c));
	float		t[2];

	if (isnan(delta))
		return ((void)(hit->dist = -1));
	t[0] = (-b - (delta)) / (2.0 * a);
	t[1] = (-b + (delta)) / (2.0 * a);
	if (t[0] > t[1])
		*((float *)t) = t[1];
	hit->dist = *t;
	
	const float	a = (ray->x * ray->x) + (ray->z * ray->z);
	const float	b = 2 * (ray->x * (orig->x - obj->cylinder.pos.x)
			+ ray->z * (orig->z - obj->cylinder.pos.z));
	const float	c = (orig->x - obj->cylinder.pos.x)
		* (orig->x - obj->cylinder.pos.x)
		+ (orig->z - obj->cylinder.pos.z) * (orig->z - obj->cylinder.pos.z)
		- obj->cylinder.rad * obj->cylinder.rad;
	const float	delta = b * b - 4 * (a * c);

	if (delta < 0.001)
		return ((void)(hit->dist = -1));
	hit->dist = fmin((-b - sqrt(delta)) / (2 * a),
			(-b + sqrt(delta)) / (2 * a));
	hit->pos = mult(ray, hit->dist);
	hit->pos = add(orig, &hit->pos);
	hit->normal = normalize((t_vec){hit->pos.x - obj->cylinder.pos.x, 0, hit->pos.z - obj->cylinder.pos.z});*/
}

static inline void	ray_cone(const t_vec *orig, const t_vec *ray,
		t_obj *obj, t_hit *hit)
{
	const float	cosa = powf(obj->cone.rad, 2.0f);
	const t_vec	co = sub(orig, &obj->cone.pos);
	const float a = powf(dot(ray, &obj->cone.dir), 2.0f) - cosa;
	const float b = 2.0f * (dot(ray, &obj->cone.dir) * dot(&co, &obj->cone.dir) - dot(ray, &co) * cosa);
	const float c = powf(dot(&co, &obj->cone.dir), 2.0f) - dot(&co, &co) * cosa;

	const float rdet = b * b - (4.0f * a * c);
	if (rdet < 0.0f)
		return ((void)(hit->dist = -1));
	
	const float det = sqrtf(rdet);
	const float t1 = (float)(-b - det) / (float)(2.0f * a);
	const float t2 = (float)(-b + det) / (float)(2.0f * a);

	float t = t1;
	if (t < EPSILON || (t2 > EPSILON && t2 < t))
		t = t2;
	if (t < EPSILON)
		return ((void)(hit->dist = -1));
	t_vec cp = mult(ray, t);
	cp = add(orig, &cp);
	cp = sub(&cp, &obj->cone.pos);
	const float h = dot(&cp, &obj->cone.dir);
	if (h < EPSILON || h > obj->cone.height)
		return ((void)(hit->dist = -1));
	hit->dist = t;
	hit->pos = mult(ray, hit->dist);
	hit->pos = add(orig, &hit->pos);
	hit->normal = mult(&cp, dot(&obj->cone.dir, &cp));
	hit->normal = vec_div(&hit->normal, dot(&cp, &cp));
	hit->normal = normalize(sub(&hit->normal, &obj->cone.dir));
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
