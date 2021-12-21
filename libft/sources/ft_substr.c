/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/07 14:30:42 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/03 17:44:17 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

/**
 * Alloue et retourne une chaine de caractères issue de
 * la chaine ’s’. Cette nouvelle chaine commence à l’index
 * ’start’ et a pour taille maximale ’len’.
 *
 * \param	s		La chaine depuis laquelle extraire la nouvelle.
 * \param	start	L’index de début  dans la chaine ’s’.
 * \param	len		La taille maximale de la nouvelle chaine.
 * \return			La nouvelle chaine de caractères.
 */
char	*ft_substr(const char *s, unsigned int start, size_t len)
{
	char	*sptr;

	if (!s)
		return (NULL);
	if (str_len(s) < start)
		return (str_dup(""));
	if (str_len(s + start) < len)
		len = str_len(s + start);
	sptr = (char *) ft_calloc(len + 1, 1);
	if (!sptr)
		return (NULL);
	str_lcpy(sptr, s + start, len + 1);
	return (sptr);
}
