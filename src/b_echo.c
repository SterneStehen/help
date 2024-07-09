/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   b_echo.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 07:07:20 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/09 07:13:33 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	is_space_filled(char *arg)
{
	int	idx;

	idx = 0;
	while (arg[idx] != '\0')
	{
		if (arg[idx] == 34 || arg[idx] == 39 || arg[idx] == 32)
		{
			idx++;
		}
		else
		{
			return (0);
		}
	}
	return (1);
}

void	safe_write(int fd, const void *buf, size_t count)
{
	if (write(fd, buf, count) == -1)
	{
		printf("Write error\n");
		exit(EXIT_FAILURE);
	}
}

// Функция для обработки экранированных символов
void	handle_escaped_chars(char *str, int *index)
{
	int	count;
	int	i;
	int	to_write;

	count = 0;
	i = *index;
	while (str[i] == 92)
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

// Функция для печати символов, не являющихся кавычками
void	print_non_quote_chars2(char *str, int single_quote_detected,
		int double_quote_detected, int *index)
{
	if (str[*index] != 34 && str[*index] != 39)
	{
		safe_write(1, &str[*index], 1);
	}
	else if (str[*index] == 34 && single_quote_detected)
	{
		safe_write(1, &str[*index], 1);
	}
	else if (str[*index] == 39 && double_quote_detected)
	{
		safe_write(1, &str[*index], 1);
	}
	(*index)++;
}

// Функция для анализа наличия кавычек в строке
int	analyze_quotes(char *str, int *quote_found)
{
	int	j;

	j = 0;
	while (str[j] != '\0')
	{
		if (str[j] == 34 || str[j] == 39)
		{
			*quote_found = 1;
			break ;
		}
		j++;
	}
	return (0);
}

void	analyze_and_handle_quotes(char *input, int *ctx)
{
	if (input[ctx[0]] == 34)
	{
		if (!(ctx[2]))
			ctx[1] = !(ctx[1]);
		else
			safe_write(1, &input[ctx[0]], 1);
		ctx[0]++;
	}
	else if (input[ctx[0]] == 39)
	{
		if (!(ctx[1]))
			ctx[2] = !(ctx[2]);
		else
			safe_write(1, &input[ctx[0]], 1);
		ctx[0]++;
	}
}

int	print_without_quotes(char *input)
{
	int	ctx[4];

	ctx[0] = 0;
	ctx[1] = 0;
	ctx[2] = 0;
	ctx[3] = 0;
	analyze_quotes(input, &ctx[3]);
	while (input[ctx[0]] != '\0')
	{
		analyze_and_handle_quotes(input, ctx);
		if (input[ctx[0]] == '\0')
			break ;
		while ((input[ctx[0]] == 32 && input[ctx[0] + 1] == 32 && !ctx[3])
			|| input[ctx[0]] == 92)
			ctx[0]++;
		if (input[ctx[0]] == 92)
			handle_escaped_chars(input, &ctx[0]);
		else
			print_non_quote_chars2(input, ctx[2], ctx[1], &ctx[0]);
	}
	return (0);
}

// // Функция для печати строки без кавычек
// int	print_without_quotes(char *input)
// {
// 	int	idx;
// 	int	detected_dq;
// 	int	detected_sq;
// 	int	found_quote;

// 	idx = 0;
// 	detected_dq = 0;
// 	detected_sq = 0;
// 	found_quote = 0;
// 	analyze_quotes(input, &found_quote);
// 	while (input[idx] != '\0')
// 	{
// 		if (input[idx] == 34)
// 		{
// 			if (!detected_sq)
// 				detected_dq = !detected_dq;
// 			else
// 				safe_write(1, &input[idx], 1);
// 			idx++;
// 			continue ;
// 		}
// 		if (input[idx] == 39)
// 		{
// 			if (!detected_dq)
// 				detected_sq = !detected_sq;
// 			else
// 				safe_write(1, &input[idx], 1);
// 			idx++;
// 			continue ;
// 		}
// 		while ((input[idx] == 32 && input[idx + 1] == 32 && !found_quote)
// 			|| input[idx] == 92)
// 			idx++;
// 		if (input[idx] == 92)
// 			handle_escaped_chars(input, &idx);
// 		else
// 			print_non_quote_chars2(input, detected_sq, detected_dq, &idx);
// 	}
// 	return (0);
// }

// Функция для обработки одного аргумента
void	process_single_arg(char *arg, int *current_is_space_filled)
{
	print_without_quotes(arg);
	*current_is_space_filled = is_space_filled(arg);
}

// Функция для обработки аргументов
void	process_args(char **arguments, int startIndex)
{
	int	*p_current_space;
	int	*p_next_space;

	p_current_space = (int *)malloc(sizeof(int));
	p_next_space = (int *)malloc(sizeof(int));
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

// Функция для обработки флага -n
int	handle_flag_n(char **args, int start)
{
	if (strncmp(args[start], "-n", 2) == 0)
	{
		if (is_flag_valid(args[start]) == 1)
		{
			write(1, "", 1);
			return (1);
		}
	}
	return (0);
}

// Функция для обработки аргументов
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

// Функция для проверки флага -n
int	check_n_flag(char *arg)
{
	int	i;

	i = 1;
	while (arg[i] != '\0')
	{
		if (arg[i] != 110)
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
	if (strcmp(arg, "-n") == 1)
	{
		return (1);
	}
	else if (strncmp(arg, "-n", 2) == 0)
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
		if (strcmp(cmd, "echo") == 1 && args[1] == NULL)
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

// Функция вызова echo
int	call_echo(t_tools *shell, char *cmd, char **args)
{
	echo(shell, cmd, args);
	if (shell->last_status != 127)
		shell->last_status = 0;
	return (0);
}
