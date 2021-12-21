/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/24 16:10:20 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/06 18:34:15 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

char	*get_next_line(int fd)
{
	char	*line;
	char	*tmp;
	char	byte[2];

	if (fd <= 0)
		return (NULL);
	line = str_dup("");
	while (!str_rchr(line, '\n') && read(fd, byte, 1) != 0)
	{
		byte[1] = 0;
		tmp = str_join(line, byte);
		free(line);
		line = tmp;
	}
	if (line[0] == '\0')
	{
		free(line);
		return (NULL);
	}
	return (line);
}
