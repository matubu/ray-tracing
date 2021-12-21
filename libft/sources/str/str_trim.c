/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_trim.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/07 15:09:41 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/03 17:56:06 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * Alloue et retourne une copie de la chaine ’s1’,
 * sans les caractères spécifiés dans ’set’ au début
 * et à la fin de la chaine de caractères.
 *
 * \param	s1	La chaine de caractères à trimmer.
 * \param	set	Le set de reference de caractères à trimmer.
 * \return		La chaine de caractères trimmée.
 */
char	*str_trim(const char *s1, const char *set)
{
	int		size;

	if (!s1 || !set)
		return (NULL);
	while (*s1 && str_chr(set, *s1))
		s1++;
	size = str_len(s1);
	while (size && str_chr(set, s1[size]))
		size--;
	return (ft_substr(s1, 0, size + 1));
}
