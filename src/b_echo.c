/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   b_echo.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 07:07:20 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/08 04:11:01 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	is_space_filled(char *arg)
{
	int	idx;

	idx = 0;
	while (arg[idx] != '\0')
	{
		if (arg[idx] == '"' || arg[idx] == '\'' || arg[idx] == ' ')
		{
			idx++;
		}
		else
		{
			return (FALSE);
		}
	}
	return (TRUE);
}

void	safe_write(int fd, const void *buf, size_t count)
{
	if (write(fd, buf, count) == -1)
	{
		printf("Write error\n");
		exit(EXIT_FAILURE);
	}
}

void	handle_escaped_chars(char *str, int *index)
{
	int	count;
	int	i;
	int	to_write;

	count = 0;
	i = *index;
	while (str[i] == '\\')
	{
		count++;
		i++;
	}
	*index = i;
	to_write = (count / 2);
	if (count % 2 != 0)
	{
		to_write++;
	}
	while (to_write > 0)
	{
		safe_write(1, "\\", 1);
		to_write--;
	}
}

void	print_non_quote_chars(char *str, int quote_detected, int *index)
{
	if (str[*index] != DQ && str[*index] != SQ)
	{
		safe_write(1, &str[*index], 1);
	}
	else if (str[*index] == SQ && quote_detected)
	{
		safe_write(1, &str[*index], 1);
	}
	(*index)++;
}

int	analyze_quotes(char *str, int *quote_found)
{
	int	j;

	j = 0;
	while (str[j] != '\0')
	{
		if (str[j] == '\'' || str[j] == '"')
		{
			*quote_found = 1;
			break ;
		}
		j++;
	}
	return (0);
}

int	print_without_quotes(char *input)
{
	int	idx;
	int	detected_dq;
	int	found_quote;

	idx = 0;
	detected_dq = 0;
	found_quote = 0;
	analyze_quotes(input, &found_quote);
	while (input[idx] != '\0')
	{
		if (input[idx] == '"')
			detected_dq = 1;
		while ((input[idx] == ' ' && input[idx + 1] == ' ' && !found_quote)
			|| input[idx] == '\\')
			idx++;
		if (input[idx] == '\\')
			handle_escaped_chars(input, &idx);
		else
			print_non_quote_chars(input, detected_dq, &idx);
	}
	return (0);
}

void	process_single_arg(char *arg, int *current_is_space_filled)
{
	print_without_quotes(arg);
	*current_is_space_filled = is_space_filled(arg);
}

// Вспомогательная функция для печати аргументов
void	process_args(char **arguments, int startIndex)
{
	int	*p_current_space;
	int	*p_next_space;

	p_current_space = (int *)smalloc(sizeof(int));
	p_next_space = (int *)smalloc(sizeof(int));
	while (arguments[startIndex] != NULL)
	{
		process_single_arg(arguments[startIndex], p_current_space);
		if (arguments[startIndex + 1] != NULL)
		{
			*p_next_space = is_space_filled(arguments[startIndex + 1]);
		}
		if (arguments[startIndex + 1] != NULL && (!(*p_current_space)
				|| (*p_next_space)))
		{
			write(1, " ", 1);
		}
		startIndex++;
	}
	free(p_current_space);
	free(p_next_space);
}

int	handle_flag_n(char **args, int start)
{
	if (ft_strncmp(args[start], "-n", 2) == 0)
	{
		if (is_flag_valid(args[start]) == 1)
		{
			write(1, "", 1);
			return (1);
		}
	}
	return (0);
}

// Вспомогательная функция для обработки аргументов
void	handle_args(char **args)
{
	int	i;

	i = 1;
	while (is_flag_valid(args[i]) == 1)
	{
		i++;
	}
	if (!handle_flag_n(args, i))
	{
		process_args(args, i);
		if (!is_flag_valid(args[1]))
		{
			write(1, "\n", 1);
		}
	}
}

// Вспомогательная функция для проверки флага -n
int	check_n_flag(char *arg)
{
	int	i;

	i = 1;
	while (arg[i] != '\0')
	{
		if (arg[i] != 'n')
		{
			return (0);
		}
		i++;
	}
	return (1);
}

// Проверка флага
int	is_flag_valid(char *arg)
{
	if (ft_strcmp(arg, "-n") == 1)
	{
		return (1);
	}
	else if (ft_strncmp(arg, "-n", 2) == 0)
	{
		return (check_n_flag(arg));
	}
	return (0);
}

// Основная функция echo
int	echo(t_tools *shell, char *cmd, char **args)
{
	if (shell->flag_log == 1)
	{
		shell->last_status = 0;
		if (ft_strcmp(cmd, "echo") == 1 && args[1] == NULL)
		{
			write(1, "\n", 1);
		}
		else
		{
			handle_args(args);
		}
	}
	return (0);
}

int	call_echo(t_tools *shell, char *cmd, char **args)
{
	echo(shell, cmd, args);
	if (shell->last_status != 127)
		shell->last_status = 0;
	return (0);
}
