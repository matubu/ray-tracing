/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_rchr.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/06 17:02:29 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/03 17:55:35 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*str_rchr(const char *s, int c)
{
	char	*str;
	int		i;

	str = (char *) s;
	i = str_len(s);
	while (str[i] != (unsigned char) c)
	{
		if (i == 0)
			return (NULL);
		i--;
	}
	return (str + i);
}
