/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 16:31:31 by mberger-          #+#    #+#             */
/*   Updated: 2022/01/17 12:21:22 by mberger-         ###   ########.fr       */
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

static inline float sign(float v)
{
	if (v >= 0)
		return (1);
	return (-1);
}

static inline void	ray_cylinder(const t_vec *orig, const t_vec *ray,
		const t_obj *obj, t_hit *hit)
{
	t_vec pb = mult(&obj->cylinder.dir, obj->cylinder.height);
	pb = add(&obj->cylinder.pos, &pb);
	t_vec ca = sub(&pb, &obj->cylinder.pos);
	t_vec oc = sub(orig, &obj->cylinder.pos);
	float caca = dot(&ca, &ca);
	float card = dot(&ca, ray);
	float caoc = dot(&ca, &oc);
	float a = caca - card * card;
	float b = caca * dot(&oc, ray) - caoc * card;
	float c = caca * dot(&oc, &oc) - caoc * caoc - obj->cylinder.rad * obj->cylinder.rad * caca;
	float h = b * b - a * c;
	if(h < 0.0) //no intersection
		return ((void)(hit->dist = -1));
	h = sqrt(h);
	float t = (-b - h) / a;
	// body
	float y = caoc + t * card;
	if(y > 0.0 && y < caca)
	{
		hit->dist = t;
		hit->pos = mult(ray, hit->dist);
		hit->pos = add(orig, &hit->pos);
		//hit->normal = (oc + t * ray - ca * y / caca) / ra;
		t_vec tmp = mult(&ca, y);
		t_vec tmp1 = mult(ray, t);
		t_vec tmp2 = mult(&tmp, 1 / caca);
		t_vec tmp3 = sub(&tmp1, &tmp2);
		tmp = add(&oc, &tmp3);
		hit->normal = mult(&tmp, 1 / obj->cylinder.rad);
		return ;
	}
	// caps
	t = (((y < 0.0) ? 0.0 : caca) - caoc) / card;
	if (fabs(b + a * t) < h)
	{
		hit->dist = t;
		hit->pos = mult(ray, hit->dist);
		hit->pos = add(orig, &hit->pos);
		//hit->normal = ca * sign(y) / caca;
		t_vec tmp = mult(&ca, sign(y));
		hit->normal = mult(&tmp, 1 / caca);
		return ;
	}
	hit->dist = -1;
}

static inline void	ray_cone(const t_vec *orig, const t_vec *ray,
		t_obj *obj, t_hit *hit)
{
	const float	cosa = .95f * .95f;
	obj->cone.height = 100.0f;

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
	if (t < .0f || (t2 > .0f && t2 < t))
		t = t2;
	if (t < .0f)
		return ((void)(hit->dist = -1));
	t_vec cp = mult(ray, t);
	cp = add(orig, &cp);
	cp = sub(&cp, &obj->cone.pos);
	const float h = dot(&cp, &obj->cone.dir);
	if (h < .0f || h > obj->cone.height)
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
