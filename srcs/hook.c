#include "minirt.h"
/*
int	on_mouse_move(int x, int y, t_scene *scene)
{
	static int	first = 1;
	static int	last[2];

	if (first)
		first = 0;
	else if (scene->button == 1)
	{
		scene->cam.rot_euler.z += (float)(last[0] - x) / 50.0;
		scene->cam.rot_euler.y += (float)(last[1] - y) / 50.0;
		render(scene, scene->win.buf);
	}
	else if (scene->button == 3)
	{
		t_vec	mx = radian_to_vector(&scene->cam.rot_euler);
		t_vec	mz = mx;
		t_vec	up = {0, 0, 1};
		t_vec	left = {1, 0, 0};
		mx = cross(&mx, &up);
		mz = cross(&mz, &left);
		mx = mult(&mx, (float)(last[0] - x) / 10.0);
		mz = mult(&mz, (float)(last[1] - y) / 10.0);
		scene->cam.pos = add3(&scene->cam.pos, &mx, &mz);
		render(scene, scene->win.buf);
	}
	else
		first = 1;
	last[0] = x;
	last[1] = y;
	return (1);
}

int	on_button_down(int button, int x, int y, t_scene *scene)
{
	t_vec	rad;

	(void)x;
	(void)y;
	scene->button = button;
	rad = radian_to_vector(&scene->cam.rot_euler);
	if (button == 5)
		rad = mult(&rad, -1);
	if (button == 4 || button == 5)
	{
		scene->cam.pos = sub(&scene->cam.pos, &rad);
		render(scene, scene->win.buf);
	}
	return (1);
}

int	on_button_up(int button, int x, int y, t_scene *scene)
{
	(void)x;
	(void)y;
	(void)button;
	scene->button = 0;
	return (1);
}

int	on_key_up(int key, t_scene *scene)
{
	if (key == 12 || key == 53 || key == 65307)
	{
		mlx_destroy_window(scene->win.mlx, scene->win.win);
		exit(EXIT_SUCCESS);
	}
	return (0);
}
*/