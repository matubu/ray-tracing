/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 13:42:15 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/21 13:42:35 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

int	check_args(int ac, char **av)
{
	(void) av;

	if (ac < 2)
		return (print_err("too few arguments.", 0));
	if (ac > 2)
		return (print_err("too much arguments.", 0));
	return (1);
}

int	check_file(char *filename)
{
	int	filedesc;

	filedesc = open(filename, O_DIRECTORY);
	if (filedesc != -1)
		return (print_err("file is a directory.", 0));
	close(filedesc);
	filedesc = open(filename, O_RDONLY);
	if (filedesc == -1)
		return (print_err("file is invalid.", 0));
	close(filedesc);
	return (1);
}
