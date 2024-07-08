/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   b_unset.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 07:10:56 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/08 18:20:33 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

char	*search_char(const char *str, int ch)
{
	int	index;

	index = 0;
	while (str[index])
	{
		if (str[index] == (char)ch)
			return ((char *)(str + index));
		index++;
	}
	if (str[index] == (char)ch)
		return ((char *)(str + index));
	return (NULL);
}

int	check_invalid_chars(char *string)
{
	int	idx;

	idx = 1;
	while (string[idx])
	{
		if (string[idx] == '^')
			return (1);
		if (string[idx] == '+' || string[idx] == '=' || string[idx] == '_'
			|| (string[idx] >= 'A' && 'z' >= string[idx]) || (string[idx] >= '0'
				&& '9' >= string[idx]))
		{
			if (string[idx] == '+')
			{
				if (string[idx + 1] == '=')
					return (0);
				return (1);
			}
			else if (string[idx] == '=')
				return (0);
			idx++;
		}
		else
			return (1);
	}
	return (0);
}

int	validate_argument(char *arg)
{
	return (strlen(arg) == 0 || arg[0] == '?' || arg[0] == '$'
		|| search_char(arg, '=') || search_char(arg, '\\')
		|| check_invalid_chars(arg));
}

int	process_argument(t_tools *tools, char *arg)
{
	t_environment	*env;

	env = ft_find(tools->envair, arg);
	if (env != NULL)
	{
		tools->last_status = 0;
		remove_env_var(tools->envair, env);
		return (1);
	}
	return (0);
}

int	handle_argument(t_tools *tools, char **args, int index)
{
	if (validate_argument(args[index]))
	{
		tools->last_status = 1;
		if (tools->flag_log)
		{
			printf("%s%s: `%s': %s\n", MINI, args[0], args[index], VAL);
		}
		return (1);
	}
	else if (args[index][0] == '-')
	{
		tools->last_status = 2;
		return (0);
	}
	process_argument(tools, args[index]);
	return (0);
}

int	unset(t_tools *tools, char *command, char **args)
{
	int	index;

	index = 1;
	if (args[1] == NULL)
	{
		return (0);
	}
	while (args[index] != NULL)
	{
		if (handle_argument(tools, args, index))
		{
			return (1);
		}
		if (args[index][0] == '-')
		{
			index++;
			continue ;
		}
		index++;
	}
	return (0);
}

t_environment	*ft_find(t_environment *env, const char *name)
{
	while (env != NULL)
	{
		if (strncmp(env->title, name, strlen(name)) == 0)
			return (env);
		env = env->next;
	}
	return (NULL);
}

int	remove_env_var(t_environment *start, t_environment *target)
{
	t_environment	*temporary_node;
	t_environment	*current_node;

	current_node = start;
	while (current_node != NULL)
	{
		if (current_node->next == target)
		{
			temporary_node = target->next;
			current_node->next = temporary_node;
			free(target->title);
			free(target->data);
			free(target);
			return (0);
		}
		current_node = current_node->next;
	}
	return (0);
}
