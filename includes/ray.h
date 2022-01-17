/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 16:31:31 by mberger-          #+#    #+#             */
/*   Updated: 2022/01/17 14:01:11 by acoezard         ###   ########.fr       */
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
	const t_vec	ca = sub(add(obj->cylinder.pos, mult(obj->cylinder.dir, obj->cylinder.height)), obj->cylinder.pos);
	const t_vec	oc = sub(*orig, obj->cylinder.pos);
	const float	caca = dot(ca, ca);
	const float	card = dot(ca, *ray);
	const float	caoc = dot(ca, oc);
	const float	a = caca - card * card;
	const float	b = caca * dot(oc, *ray) - caoc * card;
	const float	c = caca * dot(oc, oc) - caoc * caoc - obj->cylinder.rad * obj->cylinder.rad * caca;
	float	h = b * b - a * c;
	if(h < 0.0)
		return ((void)(hit->dist = -1));
	h = sqrt(h);
	float t = (-b - h) / a;
	const float y = caoc + t * card;
	if(y > 0.0 && y < caca)
		hit->normal = mult(add(oc, sub(mult(*ray, t), mult(mult(ca, y), 1 / caca))), 1 / obj->cylinder.rad);
	else {
		t = ((caca * !(y < 0.0)) - caoc) / card;
		if (fabs(b + a * t) < h)
			hit->normal = mult(mult(ca, sign(y)), 1 / caca);
		else 
			return ((void)(hit->dist = -1));
	}
	hit->dist = t;
	hit->pos = add(*orig, mult(*ray, hit->dist));
}

static inline void	ray_cone(const t_vec *orig, const t_vec *ray,
		t_obj *obj, t_hit *hit)
{
	const float	cosa = powf(obj->cone.rad, 2.0f);
	const t_vec	co = sub(*orig, obj->cone.pos);
	const float	a = powf(dot(*ray, obj->cone.dir), 2.0f) - cosa;
	const float	b = 2.0f * (dot(*ray, obj->cone.dir) * dot(co, obj->cone.dir) - dot(*ray, co) * cosa);
	const float	c = powf(dot(co, obj->cone.dir), 2.0f) - dot(co, co) * cosa;

	const float	rdet = b * b - (4.0f * a * c);
	if (rdet < 0.0f)
		return ((void)(hit->dist = -1));
	
	const float	det = sqrtf(rdet);
	const float	t1 = (float)(-b - det) / (float)(2.0f * a);
	const float	t2 = (float)(-b + det) / (float)(2.0f * a);

	float t = t1;
	if (t < EPSILON || (t2 > EPSILON && t2 < t))
		t = t2;
	if (t < EPSILON)
		return ((void)(hit->dist = -1));
	const t_vec	cp = sub(add(*orig, mult(*ray, t)), obj->cone.pos);
	const float	h = dot(cp, obj->cone.dir);
	if (h < EPSILON || h > obj->cone.height)
		return ((void)(hit->dist = -1));
	hit->dist = t;
	hit->pos = mult(*ray, hit->dist);
	hit->pos = add(*orig, hit->pos);
	hit->normal = mult(cp, dot(obj->cone.dir, cp));
	hit->normal = vec_div(hit->normal, dot(cp, cp));
	hit->normal = normalize(sub(hit->normal, obj->cone.dir));
}
/*
vec4 coneIntersect( in vec3  ro, in vec3  rd, in vec3  pa, in vec3  pb, in float ra, in float rb )
{
	vec3  ba = pb - pa;
	vec3  oa = ro - pa;
	vec3  ob = ro - pb;
	float m0 = dot(ba,ba);
	float m1 = dot(oa,ba);
	float m2 = dot(rd,ba);
	float m3 = dot(rd,oa);
	float m5 = dot(oa,oa);
	float m9 = dot(ob,ba); 

	// caps
	if( m1<0.0 )
	{
		if( dot2(oa*m2-rd*m1)<(ra*ra*m2*m2) ) // delayed division
			return vec4(-m1/m2,-ba*inversesqrt(m0));
	}
	else if( m9>0.0 )
	{
		float t = -m9/m2;                     // NOT delayed division
		if( dot2(ob+rd*t)<(rb*rb) )
			return vec4(t,ba*inversesqrt(m0));
	}

	// body
	float rr = ra - rb;
	float hy = m0 + rr*rr;
	float k2 = m0*m0    - m2*m2*hy;
	float k1 = m0*m0*m3 - m1*m2*hy + m0*ra*(rr*m2*1.0        );
	float k0 = m0*m0*m5 - m1*m1*hy + m0*ra*(rr*m1*2.0 - m0*ra);
	float h = k1*k1 - k2*k0;
	if( h<0.0 ) return vec4(-1.0); //no intersection
	float t = (-k1-sqrt(h))/k2;
	float y = m1 + t*m2;
	if( y<0.0 || y>m0 ) return vec4(-1.0); //no intersection
	return vec4(t, normalize(m0*(m0*(oa+t*rd)+rr*ba*ra)-ba*hy*y));
}
*/
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
