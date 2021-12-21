/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_dup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/07 13:39:21 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/03 17:54:30 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*str_dup(const char *s)
{
	char	*dest;
	int		dest_len;

	dest_len = str_len(s) + 1;
	dest = (char *) ft_calloc(dest_len, 1);
	if (!dest)
		return (NULL);
	return (str_cpy(dest, s));
}
