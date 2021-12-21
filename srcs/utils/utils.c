/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 13:51:25 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/21 17:28:44 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

int	print_err(char *message, int code)
{
	ft_putstr_fd("Error: ", 1);
	ft_putstr_fd(message, 1);
	ft_putstr_fd("\n", 1);
	return (code);
}

size_t	rgb_to_hex(int r, int g, int b)
{
	return ((r & 255) << 16 | (g & 255) << 8 | (b & 255));
}
