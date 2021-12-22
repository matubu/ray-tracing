/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gnl.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberger- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/22 12:41:40 by mberger-          #+#    #+#             */
/*   Updated: 2021/12/22 12:41:56 by mberger-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>

int	ft_strlen(const char *s)
{
	int	i;

	if (s == NULL)
		return (0);
	i = 0;
	while (s[i])
		i++;
	return (i);
}

char	*ft_strjoin(char *s, char c)
{
	char	*out;
	int		n;

	n = 2;
	if (s)
		while (*s && s++)
			n++;
	out = malloc(n * sizeof(char));
	out[--n] = '\0';
	out[--n] = c;
	if (s)
	{
		while (n--)
			out[n] = *--s;
		free(s);
	}
	return (out);
}

//gnl with auto free
char	*gnl(int fd)
{
	char		c;
	static char	*line = NULL;

	if (line)
		free(line);
	line = NULL;
	while (read(fd, &c, 1) && (c != '\n' || line == NULL))
	{
		if (c == '\n')
			continue ;
		line = ft_strjoin(line, c);
	}
	return (line);
}
