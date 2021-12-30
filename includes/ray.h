/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 16:31:31 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/30 15:14:22 by mberger-         ###   ########.fr       */
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
}

static inline void	ray_cone(const t_vec *orig, const t_vec *ray,
		const t_obj *obj, t_hit *hit)
{
	const float rad = 2;
	const float height = 5;

	const float A = orig->x - obj->cone.pos.x;
	const float B = orig->z - obj->cone.pos.z;
	const float D = height - orig->y + obj->cone.pos.y;
	
	const float tan = (rad / height) * (rad / height);
	
	const float a = (ray->x * ray->x) + (ray->z * ray->z) - (tan*(ray->y * ray->y));
	const float b = (2 * A * ray->x) + (2 * B * ray->z) + (2 * tan * D * ray->y);
	const float c = (A * A) + (B * B) - (D * D * tan);
	
	const float delta = b * b - 4 * a * c;
	if (delta < 0.001) 
		return ((void)(hit->dist = -1));
	
	const float t2 = (-b + sqrt(delta))/(2*a);
	const float t = (-b - sqrt(delta))/(2*a);
	
	if (t > t2)
		*((float *)(&t)) = t2;
	
	hit->dist = t;
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
