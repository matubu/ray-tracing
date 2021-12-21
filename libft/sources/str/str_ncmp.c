/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_ncmp.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/07 12:19:31 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/06 18:11:14 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	str_ncmp(const char *s1, const char *s2, size_t n)
{
	unsigned char	*s1_bis;
	unsigned char	*s2_bis;

	if (n == 0)
		return (0);
	s1_bis = (unsigned char *) s1;
	s2_bis = (unsigned char *) s2;
	while (n > 1 && *s1_bis && *s2_bis && *s1_bis == *s2_bis)
	{
		s1_bis++;
		s2_bis++;
		n--;
	}
	return (*s1_bis - *s2_bis);
}
