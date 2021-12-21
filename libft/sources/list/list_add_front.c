/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list_add_front.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 15:31:13 by acoezard          #+#    #+#             */
/*   Updated: 2021/11/14 20:56:53 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/libft.h"

void	list_add_front(t_list *list, void *content)
{
	t_node	*head;
	t_node	*last;
	t_node	*node;

	head = list->first;
	node = list_create_node(content);
	if (head == NULL)
		list->first = node;
	else
	{
		last = list->last;
		if (last == NULL)
			last = head;
		node->next = head;
		head->prev = node;
		list->first = node;
		list->last = last;
	}
	list->size++;
}
