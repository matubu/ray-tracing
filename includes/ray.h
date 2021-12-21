/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 16:31:31 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/21 16:31:32 by mberger-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RAY_H
# define RAY_H

# include "struct.h"
# include "vector.h"

static inline void	ray_sphere(t_vec *orig, t_vec *ray,
		t_sphere *sphere, t_hit *hit)
{
	const t_vec	oc = sub(orig, &sphere->pos);
	const float	a = dot(ray, ray);
	const float	b = dot(&oc, ray);
	const float	c = dot(&oc, &oc) - sphere->srad;
	const float	d = b * b - a * c;

	if (d <= EPSILON)
		return ((void)(hit->dist = -1));
	hit->dist = (-b - sqrt(b * b - a * c)) / a;
	hit->pos = mult(ray, hit->dist);
	hit->pos = add(orig, &hit->pos);
	hit->normal = normalize(sub(&hit->pos, &sphere->pos));
}

static inline void	ray_plane(t_vec *orig, t_vec *ray,
		t_plane *plane, t_hit *hit)
{
	const float	d = dot(&plane->normal, ray);

	if (d >= EPSILON && d <= EPSILON)
		return ((void)(hit->dist = -1));
	hit->pos = sub(&plane->pos, orig);
	hit->dist = dot(&hit->pos, &plane->normal) / d;
	hit->pos = mult(ray, hit->dist);
	hit->pos = add(orig, &hit->pos);
	hit->normal = plane->normal;
}

static inline void	ray_cylinder(t_vec *orig, t_vec *ray,
		t_cylinder *cylinder, t_hit *hit)
{
	const float	a = (ray->x * ray->x) + (ray->z * ray->z);
	const float	b = 2 * (ray->x * (orig->x - cylinder->pos.x)
			+ ray->z * (orig->z - cylinder->pos.z));
	const float	c = (orig->x - cylinder->pos.x) * (orig->x - cylinder->pos.x)
		+ (orig->z - cylinder->pos.z) * (orig->z - cylinder->pos.z)
		- cylinder->srad;
	const float	delta = b * b - 4 * (a * c);
	float		r;

	if (fabs(delta) < 0.001 || delta < 0.0)
		return ((void)(hit->dist = -1));
	hit->dist = fmin((-b - sqrt(delta)) / (2 * a),
			(-b + sqrt(delta)) / (2 * a));
	r = orig->y + hit->dist * ray->y;
	if (!(r >= cylinder->pos.y) && (r <= cylinder->pos.y + cylinder->height))
		return ((void)(hit->dist = -1));
}

// radius/apex https://mrl.cs.nyu.edu/~dzorin/rend05/lecture2.pdf
static inline void	ray_cone(t_vec *orig, t_vec *ray, t_cone *cone, t_hit *hit)
{
	(void)orig;
	(void)ray;
	(void)cone;
	hit->dist = 0;
}

static inline int	ray_scene(t_vec *orig, t_vec *ray,
		t_scene *scene, t_hit *closest)
{
	t_hit			hit;
	register t_obj	*obj;

	obj = scene->obj;
	closest->dist = -1;
	while (obj->func)
	{
		hit.dist = -1;
		obj->func(orig, ray, obj->data, &hit);
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
