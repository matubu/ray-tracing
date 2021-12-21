/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_cmp.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/03 17:49:31 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/03 17:54:09 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	str_cmp(const char *s1, const char *s2)
{
	unsigned char	*s1_bis;
	unsigned char	*s2_bis;

	s1_bis = (unsigned char *) s1;
	s2_bis = (unsigned char *) s2;
	while (*s1_bis && *s2_bis && *s1_bis == *s2_bis)
	{
		s1_bis++;
		s2_bis++;
	}
	return (*s1_bis - *s2_bis);
}
