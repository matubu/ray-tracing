typedef struct	s_pos
{
	double	x;
	double	y;
	double	z;
}	t_pos;

typedef struct	s_color
{
	unsigned short	red;
	unsigned short green;
	unsigned short blue;
} t_color;

typedef struct	s_material
{
	t_color color;
} t_material;

typedef struct	s_tris
{
	t_pos				(*points)[3];
	t_pos 			normal;
	t_material	*material;
}	t_tris;

typedef struct	s_sphere
{
	t_pos				pos;
	float				radius;
	t_material	*material;
} t_sphere;