/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putunbr_base_fd.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 14:32:47 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/03 17:50:48 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

static int	ft_check_base(char	*base)
{
	int		i;
	int		j;

	i = 0;
	while (base[i] != '\0')
	{
		if (base[i] == '-' || base[i] == '+')
			return (0);
		if (ft_isspace(base[i]))
			return (0);
		j = i + 1;
		while (base[j] != '\0')
			if (base[i] == base[j++])
				return (0);
		i++;
	}
	if (i <= 1)
		return (0);
	return (i);
}

static void	ft_putunbr_recursive(unsigned long nbr, char *base, int fd)
{
	int		base_size;

	base_size = str_len(base);
	if (nbr > 0)
	{
		ft_putunbr_recursive(nbr / base_size, base, fd);
		ft_putchar_fd(base[nbr % base_size], fd);
	}
}

void	ft_putunbr_base_fd(unsigned long nbr, char *base, int fd)
{
	int		base_size;

	base_size = ft_check_base(base);
	if (base_size)
	{
		if (nbr == 0)
			ft_putchar_fd(base[0], fd);
		else
			ft_putunbr_recursive(nbr, base, fd);
	}
}
