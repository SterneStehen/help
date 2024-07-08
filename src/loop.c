/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 16:43:01 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/08 15:24:13 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	get_input(t_tools *tools)
{
	char	*line;

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

void	loop_minishell(t_tools *tools)
{
	while (true)
	{
		configure_signals(&tools->terminal_setting);
		tools->flag_pipe = false;
		tools->flag_ready_to_execute = false;
		get_input(tools);
		if (tools->message == NULL)
		{
			printf("exit\n");
			break ;
		}
		tools->shell_string = del_string(tools->message, " ");
		refresh_environment(tools);
		if (!audit_intput(tools) && run_lexer(tools) == 0
			&& parse_shell_input(tools))
		{
			run_exec_steps(tools, tools->commands);
		}
		add_history(tools->message);
	}
}
