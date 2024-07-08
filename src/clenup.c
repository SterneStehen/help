/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clenup.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 14:55:21 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/08 17:51:56 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/* Free memory allocated for an array of strings. */
void	free_string_array(char **array)
{
	int	i;

	if (!array)
		return ;
	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

/* Free memory allocated for the environment structure. */
void	release_env_list(t_environment *env_list)
{
	t_environment	*temp;

	while (env_list)
	{
		temp = env_list->next;
		free(env_list->title);
		free(env_list->data);
		free(env_list);
		env_list = temp;
	}
}

/* Frees the memory allocated for all tokens in a linked list. */
void	clear_token_list(t_node *tkn)
{
	t_node	*temp;

	temp = NULL;
	while (tkn)
	{
		temp = tkn->next;
		free_single_token(tkn);
		tkn = temp;
	}
}

/* Free memory allocated for the command tables. */
void	cleanup_command_tables(t_simple_cmds *cmd_tables)
{
	t_simple_cmds	*next_table;
	t_node			*temp;

	while (cmd_tables)
	{
		while (cmd_tables->token_arg)
		{
			temp = cmd_tables->token_arg->next;
			free_single_token(cmd_tables->token_arg);
			cmd_tables->token_arg = temp;
		}
		while (cmd_tables->redirections)
		{
			temp = cmd_tables->redirections->next;
			free_single_token(cmd_tables->redirections);
			cmd_tables->redirections = temp;
		}
		free_string_array(cmd_tables->arguments);
		free(cmd_tables->command);
		free(cmd_tables->hd_file_name);
		next_table = cmd_tables->next;
		free(cmd_tables);
		cmd_tables = next_table;
	}
}

/* General purpose memory release function. */
void	free_resources(t_tools *shell)
{
	if (shell->shell_string)
		cleanup_command_tables(shell->commands);
	free_string_array(shell->paths);
	free(shell->shell_string);
	free_string_array(shell->envp);
	release_env_list(shell->envair);
	free(shell->content_hd);
	free(shell->message);
	rl_clear_history();
}

/* Free memory on exit. */
void	cleanup_before_exit(t_tools *shell)
{
	free_resources(shell);
}

/* Free memory in the child process. */
void	cleanup_child_proc(t_tools *shell)
{
	free_resources(shell);
}

/* Builtin release function to free memory and exit. */
void	release_builtin_resources(t_tools *shell)
{
	int	result;

	result = shell->last_status;
	if (shell->shell_string)
		cleanup_command_tables(shell->commands);
	free_string_array(shell->paths);
	free(shell->shell_string);
	free_string_array(shell->envp);
	release_env_list(shell->envair);
	free(shell->content_hd);
	free(shell->message);
	rl_clear_history();
	exit(result);
}

/* Exit the child process and clean up resources. */
void	child_process_finish(t_tools *shell)
{
	int	code;

	code = shell->last_status;
	if (shell->shell_string)
		cleanup_command_tables(shell->commands);
	free_string_array(shell->paths);
	free_string_array(shell->envp);
	release_env_list(shell->envair);
	free(shell->content_hd);
	free(shell->message);
	rl_clear_history();
	exit(code);
}
