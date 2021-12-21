/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acoezard <acoezard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/07 13:15:46 by acoezard          #+#    #+#             */
/*   Updated: 2021/10/23 17:18:37 by acoezard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

/**
 * La fonction ft_calloc() alloue la mémoire nécessaire pour
 * un tableau de nmemb éléments de size octets, et renvoie un pointeur
 * vers la mémoire allouée qui a été remplie avec des zéros.
 *
 * \param	nmemb	Le nombre d'élements du tableau.
 * \param	size	La taille des élements du tableau.
 * \return			L'adresse de l'espace mémoire alloué.
 */
void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*array;

	array = malloc(nmemb * size);
	if (!array)
		return (NULL);
	ft_bzero(array, nmemb * size);
	return (array);
}
