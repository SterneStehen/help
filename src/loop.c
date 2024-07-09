/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 16:43:01 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/09 18:07:31 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	read_and_process_input(t_tools *tools)
{
	char	*line;

	configure_signals(&tools->terminal_setting);
	tools->flag_pipe = false;
	tools->flag_ready_to_execute = false;
	if (isatty(fileno(stdin)))
	{
		tools->message = readline("minishell$ ");
	}
	else
	{
		line = get_next_line(fileno(stdin));
		tools->message = del_string(line, "\n");
		free(line);
	}
}

bool	is_only_spaces(const char *str)
{
	while (*str)
	{
		if (*str != ' ')
		{
			return (false);
		}
		str++;
	}
	return (true);
}

bool	check_for_exit_and_spaces(t_tools *tools)
{
	if (is_only_spaces(tools->message))
	{
		if (tools->message != NULL)
		{
			free(tools->message);
			tools->message = NULL;
		}
		return (false);
	}
	if (tools->message == NULL)
	{
		printf("exit\n");
		return (true);
	}
	return (false);
}

void	process_and_execute_commands(t_tools *tools)
{
	tools->shell_string = del_string(tools->message, " ");
	if (tools->shell_string == NULL)
	{
		free(tools->message);
		tools->message = NULL;
		return ;
	}
	refresh_environment(tools);
	if (!audit_intput(tools) && run_lexer(tools) == 0
		&& parse_shell_input(tools))
		run_exec_steps(tools, tools->commands);
	add_history(tools->message);
	free(tools->message);
	free(tools->shell_string);
	tools->message = NULL;
	tools->shell_string = NULL;
}

void	loop_minishell(t_tools *tools)
{
	while (true)
	{
		read_and_process_input(tools);
		if (tools->message == NULL)
		{
			printf("exit\n");
			break ;
		}
		if (check_for_exit_and_spaces(tools))
		{
			break ;
		}
		process_and_execute_commands(tools);
	}
}
