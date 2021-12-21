/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 12:57:46 by acoezard          #+#    #+#             */
/*   Updated: 2021/10/23 17:18:37 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	unsigned char	*pstr1;
	unsigned char	*pstr2;

	if (n == 0)
		return (0);
	pstr1 = (unsigned char *) s1;
	pstr2 = (unsigned char *) s2;
	while (*pstr1 == *pstr2 && n > 1)
	{
		pstr1++;
		pstr2++;
		n--;
	}
	return (*pstr1 - *pstr2);
}
