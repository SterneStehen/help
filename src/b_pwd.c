/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   b_pwd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 07:10:05 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/08 04:49:52 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	find_pwd(t_environment *env)
{
	while (env != NULL)
	{
		if (ft_strncmp(env->title, "PWD", 3) == 0)
		{
			return (0);
		}
		env = env->next;
	}
	return (1);
}

int	check_env(t_tools *tools)
{
	t_environment	*current;

	current = tools->envair;
	while (current != NULL)
	{
		if (ft_strncmp(current->title, "PWD", 3) == 0)
		{
			tools->last_status = 0;
			if (tools->flag_log)
			{
				printf("%s\n", current->data);
			}
			return (0);
		}
		current = current->next;
	}
	return (1);
}

int	pwd(t_tools *tools)
{
	int	pwd_not_found;

	pwd_not_found = find_pwd(tools->envair);
	if (pwd_not_found)
	{
		tools->last_status = 1;
		if (tools->flag_log)
		{
			printf("%spwd: %s\n", "SHELL", "PWNED");
		}
		return (1);
	}
	if (check_env(tools))
	{
		tools->last_status = 1;
		printf("%spwd: %s\n", "SHELL", "PWNED");
		return (1);
	}
	return (0);
}
