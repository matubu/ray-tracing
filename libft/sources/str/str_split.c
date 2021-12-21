/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_split.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/06 17:22:47 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/03 17:55:44 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	str_split_sep(char c, char sep)
{
	return (c == sep || c == '\0');
}

static int	str_split_search(const char *s, char c)
{
	int		j;

	j = 0;
	while (!str_split_sep(s[j], c))
		j++;
	return (j);
}

static int	str_split_size(const char *s, char c)
{
	int		size;
	int		i;

	size = 0;
	i = 0;
	while (s[i] != '\0')
	{
		if (!str_split_sep(s[i], c) && str_split_sep(s[i + 1], c))
		{
			size++;
		}
		i++;
	}
	return (size);
}

static char	*str_split_copy(const char *s, int start, int size)
{
	char	*word;
	int		i;

	word = (char *) ft_calloc(size + 1, 1);
	i = 0;
	while (i < size)
	{
		word[i] = s[start + i];
		i++;
	}
	return (word);
}

/**
 * Alloue et retourne un tableau de chaines de caracteres
 * obtenu en séparant ’s’ à l’aide du caractère ’c’,
 * utilisé comme délimiteur. Le tableau doit être terminé par NULL.
 *
 * \param	s	La chaine de caractères à découper.
 * \param	c	Le caractère délimitant.
 * \return		Le tableau des chaines de caractères résultant du découpage.
 */
char	**str_split(const char *s, char c)
{
	char	**words;
	int		size;
	int		i;
	int		j;
	int		k;

	size = str_split_size(s, c);
	words = (char **) malloc(sizeof(char *) * (size + 1));
	if (!words)
		return (NULL);
	i = 0;
	k = 0;
	while (s[i] != '\0')
	{
		if (s[i] == c)
			i++;
		else
		{
			j = str_split_search(s + i, c);
			words[k++] = str_split_copy(s, i, j);
			i += j;
		}
	}
	words[size] = NULL;
	return (words);
}
