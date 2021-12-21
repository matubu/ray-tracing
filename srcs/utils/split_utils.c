/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42nice.f>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 13:51:00 by acoezard          #+#    #+#             */
/*   Updated: 2021/12/21 13:51:16 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

void	str_split_free(char **array)
{
	int	i;

	i = -1;
	while (array && array[++i] != NULL)
		free(array[i]);
	free(array);
}

int	str_split_len(char **array)
{
	int	i;

	i = 0;
	while (array && array[i] != NULL)
		i++;
	return (i);
}
