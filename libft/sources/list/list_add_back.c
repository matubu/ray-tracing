/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list_add_back.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 15:43:24 by acoezard          #+#    #+#             */
/*   Updated: 2021/11/13 20:41:01 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/libft.h"

void	list_add_back(t_list *list, void *content)
{
	t_node	*head;
	t_node	*node;

	node = list_create_node(content);
	head = list->first;
	if (head == NULL)
		list->first = node;
	else
	{
		if (list->last == NULL)
			list->last = head;
		list->last->next = node;
		list->last = node;
		node->prev = list->last;
	}
	list->size++;
}
