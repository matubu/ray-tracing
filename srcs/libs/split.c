/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/22 12:40:55 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/28 18:30:56 by mberger-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

static int	ft_malloc_splits(char ***splits, const char *s, char c)
{
	int	size;
	int	i;

	size = 1;
	i = 0;
	while (s[i])
		if (s[i++] != c && size++)
			while (s[i] && s[i] != c)
				i++;
	*splits = malloc(size * sizeof(char *));
	return (*splits == NULL);
}

//put -1 to free every thing
int	free_splits(char **splits, int n)
{
	if (n == -1)
		while (splits[++n])
			free(splits[n]);
	else
		while (n--)
			free(splits[n]);
	free(splits);
	return (1);
}

static void	cpy(char *dst, const char *src, size_t size)
{
	while (--size > 0 && *src)
		*dst++ = *src++;
	*dst = '\0';
}

char	**split(const char *s, char c)
{
	char			**out;
	unsigned int	size;
	unsigned int	n;

	if (s == NULL || ft_malloc_splits(&out, s, c))
		return (NULL);
	n = 0;
	while (*s)
	{
		if (*s++ != c)
		{
			size = 1;
			while (*s && *s != c && size++)
				s++;
			out[n] = malloc(sizeof(char) * (size + 1));
			if (out[n] == NULL && free_splits(out, n))
				return (NULL);
			cpy(out[n++], s - size, size + 1);
		}
	}
	out[n] = NULL;
	return (out);
}
