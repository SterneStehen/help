/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 20:10:20 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/04 10:03:13 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	run_builtin(t_tools *tools, char *command, char **args)
{
	if (ft_strcmp(command, "/usr/bin/cd") == TRUE && tools->flag_pipe == FALSE)
		return (0);
	// return (cd(shell, cmd, args), TRUE);
	else if (ft_strcmp(command, "export") == TRUE
		&& tools->flag_ready_to_execute == TRUE)
		return (0);
	// return (export(shell, cmd, args), TRUE);
	else if (ft_strcmp(command, "cd") == TRUE && tools->flag_pipe == FALSE)
		return (0);
	// return (cd(shell, cmd, args), TRUE);
	else if (ft_strcmp(command, "pwd") == TRUE
		&& tools->flag_ready_to_execute == TRUE)
		return (0);
	// return (pwd(shell, args), TRUE);
	else if (ft_strcmp(command, "env") == TRUE
		&& tools->flag_ready_to_execute == TRUE)
		return (0);
	// return (display_env(shell, args), TRUE);
	else if (ft_strcmp(command, "exit") == TRUE)
		return (0);
	// return (exit_shell(shell, cmd, args), TRUE);
	else if (ft_strcmp(command, "unset") == TRUE
		&& tools->flag_ready_to_execute == TRUE)
		return (0);
	// return (unset(shell, cmd, args), TRUE);
	else if (ft_strcmp(command, "echo") == TRUE
		&& tools->flag_ready_to_execute == TRUE)
		return (0);
		// return (call_echo(shell, cmd, args), TRUE);
		if (tools->flag_envair == TRUE)
			tools->last_status = 127;
	return (0);
}
