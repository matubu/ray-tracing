/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_join.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/10 12:32:28 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/03 18:09:16 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * Alloue et retourne une nouvelle chaine,
 * résultat de la concaténation de s1 et s2.
 *
 * \param	s1	La chaine de caractères préfixe.
 * \param	s2	La chaine de caractères suffixe.
 * \return 		La nouvelle chaine de caractères.
 */
char	*str_join(const char *s1, const char *s2)
{
	size_t	s1_len;
	size_t	s2_len;
	char	*sptr;

	if (!s1 || !s2)
		return (NULL);
	s1_len = str_len(s1);
	s2_len = str_len(s2);
	sptr = (char *) ft_calloc(s1_len + s2_len + 1, 1);
	if (!sptr)
		return (NULL);
	str_cpy(sptr, s1);
	str_cat(sptr, s2);
	return (sptr);
}
