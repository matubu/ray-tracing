/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list_clear.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 15:54:21 by acoezard          #+#    #+#             */
/*   Updated: 2021/11/13 20:23:36 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/libft.h"

void	list_clear(t_list *list, void (*del)(void*))
{
	t_node	*current;
	t_node	*tmp;

	if (list != NULL && del != NULL)
	{
		current = list->first;
		while (current != NULL)
		{
			tmp = current->next;
			list_remove(list, current, del);
			current = tmp;
		}
		free(list);
	}
}
