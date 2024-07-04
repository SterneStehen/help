/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   audit.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 19:15:37 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/03 21:56:09 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	is_triple_symbol(const char *str, char symbol)
{
	return (str[0] == symbol && str[1] == symbol && str[2] == symbol);
}

int	audit_intput(t_tools *tols)
{
	if (tols->shell_string[0] == '"' && tols->shell_string[1] == '"'
		&& ft_strlen(tols->shell_string) == 2)
	{
		tols->flag_execution_completed = 0;
		tols->last_status = -1;
		return (-1);
	}
	if (is_triple_symbol(tols->shell_string, '>')
		|| is_triple_symbol(tols->shell_string, '<'))
	{
		tols->last_status = -1;
		printf("Syntax error near unexpected token '%c'\n",
			tols->shell_string[0]);
		return (-1);
	}
	if (tols->shell_string[0] == '~' && ft_strlen(tols->shell_string))
	{
		tols->last_status = -1;
		return (-1);
	}
	return (0);
}
