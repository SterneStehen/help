/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_control.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <7353718@gmail.com>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 03:37:03 by ggeorgie          #+#    #+#             */
/*   Updated: 2024/04/19 13:08:00 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

/**
 * @brief	Free a pointer and prevent double-freeing.
 * @param	char	*variable : a pointer to heap memory address.
 * @return	NULL.
 */
void fn_free(char **variable) 
{
    if (*variable) 
	{
        free(*variable);
        *variable = NULL;
    }
}


/**
 * @brief	Free a double pointer and the pointers it points to.
 * @param	char	**ptr : a pointer to pointers.
 * @return	NULL.
 */
char	*fn_free_ptr(char **ptr)
//void	fn_free_ptr(char **ptr)
{
	int	i;

	if (ptr)
	{
		i = 0;
//		printf("fn_free_ptr: ptr[%d] = %s\n", i, ptr[i]);
		while (ptr[i])
		{
			fn_free(&ptr[i]);
			i++;
		}
		fn_free(ptr);
	}
	return (NULL);
//	write(2, "Error\n", 6);
//	exit (EXIT_FAILURE);
}

void	*ft_calloc(size_t count, size_t size)
{
	size_t	total_size;
	char	*string;
	size_t	i;

	i = 0;
	total_size = count * size;
	string = malloc(total_size);
	if (string)
	{
		while (i < total_size)
		{
			string[i] = '\0';
			i++;
		}
	}
	return (string);
}
