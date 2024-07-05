/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/23 21:19:12 by smoreron          #+#    #+#             */
/*   Updated: 2024/05/23 21:19:12 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"


/* Checks for incorrect redirection in command. */
int	check_invalid_redirection(t_simple_cmds *command)
{
	t_simple_cmds	*cr;
	t_node			*tkn;

	cr = command;
	while (cr != NULL)
	{
		tkn = cr->redirections;
		while (tkn != NULL)
		{
			if (!tkn || !(tkn->class == LESS_LESS || tkn->class == GREAT_GREAT
					|| tkn->class == LESS || tkn->class == GREAT)
				|| tkn->next == NULL || (tkn->next->class == LESS_LESS
					|| tkn->next->class == GREAT_GREAT
					|| tkn->next->class == LESS || tkn->next->class == GREAT))
			{
				return (1);
			}
			tkn = tkn->next->next;
		}
		cr = cr->next;
	}
	return (0);
}

/* Runs here documents (heredocs) in command list. */
void	run_heredoc_commands(t_simple_cmds *start, t_simple_cmds *last,
		t_tools *sh)
{
	t_node	*token;
	char	*tmp_str;

	while (start != last->next)
	{
		token = start->redirections;
		while (token != NULL)
		{
			if (is_redirection_token(token)
				&& is_redirection_token(token->next))
				return ;
			if (token->class == 'H')
			{
				tmp_str = get_break_string(token->next->data, sh);
				free(token->next->data);
				token->next->data = tmp_str;
				start->hd_file_name = generate_temp_heredoc(start,
						token->next->data, sh);
			}
			token = token->next;
		}
		start = start->next;
	}
}

/* Checks for invalid redirections in commands. */
int	check_commands_for_invalid_redir(t_simple_cmds *tbl, t_tools *sh)
{
	t_simple_cmds	*st;
	t_simple_cmds	*lst;

	if (check_invalid_redirection(tbl))
	{
		st = tbl;
		lst = tbl;
		while (lst != NULL)
		{
			if (check_invalid_redirection(lst))
			{
				run_heredoc_commands(st, lst, sh);
				return (1);
			}
			lst = lst->next;
		}
	}
	return (0);
}

/* Executes commands in a pipeline. */
void	execute_pipeline_commands(t_simple_cmds *table, t_tools *shell)
{
	int	status;
	int	pid;

	while (table->next != NULL)
	{
		run_pipeline_child(table, shell);
		table = table->next;
	}
	finalize_pipeline_execution(table, shell);
	pid = waitpid(0, &status, 0);
	while (pid != -1)
		pid = waitpid(0, &status, 0);
}

/* Creates a pipe and handles errors. */
int	initialize_pipe(int *fd, t_tools *shell)
{
	if (pipe(fd) == -1)
	{
		if (shell->flag_log == 1)
		{
			printf("pipe failed\n");
		}
		return (0);
	}
	return (1);
}

/* Forks and sets up child process for execution. */
int	create_child_process(t_tools *shell, int *fd)
{
	pid_t	pid;

	pid = fork();
	shell->flag_ready_to_execute = 1;
	if (pid <= -1)
	{
		if (shell->flag_log == 1)
		{
			printf("%s%s\n", SHELL, FORK_ERROR);
		}
		return (0);
	}
	else if (pid == 0)
	{
		signals_child(&shell->terminal_setting);
		shell->flag_log = 1;
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		return (1);
	}
	return (2);
}

/* Executes commands in the parent process. */
void	execute_parent_process(t_simple_cmds *table, t_tools *shell, int *fd)
{
	shell->flag_log = 0;
	if (table->command != NULL)
		execute_builtin_commands(shell, table->command, table->arguments);
	close(fd[1]);
	dup2(fd[0], STDIN_FILENO);
	close(fd[0]);
}

/* Handles child process execution for a pipeline. */
int	run_pipeline_child(t_simple_cmds *table, t_tools *shell)
{
	int	fd[2];
	int	exec_flag;
	int	fork_result;

	exec_flag = initialize_pipe(fd, shell);
	if (exec_flag)
	{
		fork_result = create_child_process(shell, fd);
		if (fork_result == 1)
		{
			manage_redirections(shell, table);
			execute_command(table, shell);
			exit(0);
		}
		else if (fork_result == 0)
		{
			return (1);
		}
	}
	execute_parent_process(table, shell, fd);
	return (0);
}

/* Checks if a command has redirections. */
int	validate_redirections(t_simple_cmds *table)
{
	t_node	*token;

	token = table->redirections;
	while (token != NULL)
	{
		if (token->class == GREAT_GREAT || token->class == GREAT)
		{
			return (1);
		}
		token = token->next;
	}
	return (0);
}

/* Forks and executes a command, handling redirections. */
void	spawn_and_execute(t_simple_cmds *table, t_tools *shell, int *has_redir)
{
	pid_t	pid;
	int		status;
	int		fd[2];

	pid = fork();
	shell->flag_ready_to_execute = 1;
	if (pid == -1)
	{
		if (shell->flag_log == 1)
			printf("%s%s\n", SHELL, FORK_ERROR);
		return ;
	}
	else if (pid == 0)
	{
		signals_child(&shell->terminal_setting);
		shell->flag_log = 1;
		*has_redir = validate_redirections(table);
		if (*has_redir == 0)
			dup2(shell->standard_output_fd, STDOUT_FILENO);
		manage_redirections(shell, table);
		execute_command(table, shell);
		exit(0);
	}
	signals_parent();
	shell->flag_log = FALSE;
	waitpid(pid, &status, 0);
	shell->last_status = WEXITSTATUS(status);
}

/* Executes the final command in a pipeline. */
int	finalize_pipeline_execution(t_simple_cmds *table, t_tools *shell)
{
	int	has_redir;

	has_redir = 0;
	spawn_and_execute(table, shell, &has_redir);
	if (table->command != NULL)
		execute_builtin_commands(shell, table->command, table->arguments);
	if (invalid_redirection(shell, table->redirections, table) == 0)
	{
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		dup2(shell->standard_input_fd, STDIN_FILENO);
		dup2(shell->standard_output_fd, STDOUT_FILENO);
		if (shell->last_status != 0)
			shell->flag_execution_completed = 0;
	}
	return (0);
}

/* Restores standard input and output file descriptors. */
int	reset_file_descriptors(t_tools *sh)
{
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	dup2(sh->standard_input_fd, STDIN_FILENO);
	dup2(sh->standard_output_fd, STDOUT_FILENO);
	if (sh->last_status)
	{
		sh->flag_execution_completed = 0;
	}
	return (0);
}

/* Executes built-in commands and restores file descriptors. */
int	run_builtin_and_close(t_simple_cmds *cmd_table, t_tools *sh)
{
	if (cmd_table->command != NULL)
	{
		execute_builtin_commands(sh, cmd_table->command, cmd_table->arguments);
	}
	if (!invalid_redirection(sh, cmd_table->redirections, cmd_table))
	{
		reset_file_descriptors(sh);
	}
	else
	{
		sh->last_status = REDIR_ERR_CODE;
	}
	return (0);
}

/* Handles execution of a command table. */
int	handle_execution(t_simple_cmds *cmd_table, t_tools *sh)
{
	char	**cmd_args_copy;
	char	**env_copy;
	int		exit_status;

	signals_child(&sh->terminal_setting);
	if (!invalid_redirection(sh, cmd_table->redirections, cmd_table))
	{
		sh->flag_log = 1;
		manage_redirections(sh, cmd_table);
		execute_command(cmd_table, sh);
	}
	exit_status = sh->last_status;
	cmd_args_copy = arr_dabl(cmd_table->arguments);
	env_copy = arr_dabl(sh->envp);
	if (execve(cmd_table->command, cmd_args_copy, env_copy) == -1)
	{
		if (sh->flag_log)
		{
			printf("%s%s\n", SHELL, strerror(errno));
		}
		free_string_array(cmd_args_copy);
		free_string_array(env_copy);
		return (exit_status);
	}
	return (exit_status);
}

/* Executes a single command. */
int	simple_execute(t_simple_cmds *cmd_table, t_tools *sh)
{
	int		exec_status;
	pid_t	child_pid;
	int		wait_status;

	sh->flag_pipe = 0;
	sh->flag_ready_to_execute = 1;
	child_pid = fork();
	if (child_pid == 0)
	{
		exec_status = handle_execution(cmd_table, sh);
		child_process_finish(sh);
	}
	else if (child_pid == -1)
	{
		if (sh->flag_log)
		{
			printf("%s\n", FORK_ERR_MSG);
		}
		return (-1);
	}
	else
	{
		signals_parent();
		sh->flag_log = 0;
		waitpid(child_pid, &wait_status, 0);
		sh->last_status = WEXITSTATUS(wait_status);
		run_builtin_and_close(cmd_table, sh);
	}
	return (0);
}

/* Exits the program with an error message. */
void	error_exit(t_tools *shell, const char *msg)
{
	if (shell->flag_log == TRUE)
	{
		printf("%s%s\n", SHELL, msg);
	}
	exit(shell->last_status);
}

/* Handles directory errors in command execution. */
void	handle_directory_command(char *cmd, t_tools *shell)
{
	if (shell->flag_log == TRUE)
	{
		printf("%s%s: %s\n", SHELL, cmd, ISDIR);
	}
	shell->last_status = 126;
	cleanup_child_proc(shell);
	exit(shell->last_status);
}

/* Executes the final command in a command table. */
void	execute_final_command(char *command_path, t_simple_cmds *table,
		t_tools *shell)
{
	char	**cmd_args_copy;
	char	**env_copy;
	int		exec_result;

	shell->flag_execution_completed = 1;
	cmd_args_copy = arr_dabl(table->arguments);
	env_copy = arr_dabl(shell->envp);
	exec_result = execve(command_path, cmd_args_copy, env_copy);
	if (exec_result == -1)
	{
		error_exit(shell, strerror(errno));
	}
}

/* Finalizes the execution of a command. */
int	finalize_command_path(char *command_path, t_simple_cmds *table,
		t_tools *shell)
{
	int	cmd_length;

	cmd_length = strlen(table->command);
	if (table->command[0] == '/' && table->command[cmd_length - 1] == '/')
	{
		handle_directory_command(table->command, shell);
	}
	execute_final_command(command_path, table, shell);
	return (0);
}

/* Executes a command. */
int	execute_command(t_simple_cmds *table, t_tools *shell)
{
	char	*command_path;

	if (table->command == NULL)
	{
		exit(shell->last_status);
	}
	if (execute_builtin_commands(shell, table->command, table->arguments))
	{
		exit(shell->last_status);
	}
	else if (audit_comand_path(table->command, shell))
	{
		finalize_command_path(table->command, table, shell);
	}
	else if (table->command[0] != '.' && table->command[0] != '/')
	{
		command_path = find_command_path(shell, table->command);
		if (command_path == NULL)
		{
			error_exit(shell, "Command not found");
		}
		else if (access(command_path, X_OK) == 0)
		{
			finalize_command_path(command_path, table, shell);
		}
	}
	return (0);
}

/* Handles redirections in commands. */
void	handle_redirect(t_tools *sh, t_simple_cmds *tbl)
{
	if (check_commands_for_invalid_redir(tbl, sh))
	{
		check_redir(sh->shell_string);
		sh->last_status = 258;
		cleanup_command_tables(sh->commands);
		sh->commands = NULL;
	}
}

/* Processes and runs heredocs in command tables. */
void	process_heredocs_run(t_tools *sh, t_simple_cmds *tbl)
{
	t_simple_cmds	*current_tbl;
	t_node			*token;

	current_tbl = tbl;
	while (current_tbl != NULL)
	{
		token = current_tbl->redirections;
		while (token != NULL)
		{
			if (token->class == LESS_LESS)
			{
				execute_heredocs(current_tbl, sh);
				break ;
			}
			token = token->next;
		}
		current_tbl = current_tbl->next;
	}
}

/* Performs the execution of command tables. */
void	run_command_execution(t_tools *sh, t_simple_cmds *tbl)
{
	int				tbl_size;
	t_simple_cmds	*temp_tbl;
	int				g_ctrl_c;

	g_ctrl_c = 0;
	tbl_size = 0;
	temp_tbl = tbl;
	while (temp_tbl != NULL)
	{
		tbl_size++;
		temp_tbl = temp_tbl->next;
	}
	if (tbl_size == 1)
	{
		simple_execute(tbl, sh);
	}
	else
	{
		sh->flag_pipe = TRUE;
		execute_pipeline_commands(tbl, sh);
	}
}

/* Cleans up the shell structure. */
void	cleanup(t_tools *sh)
{
	cleanup_command_tables(sh->commands);
	sh->commands = NULL;
}

/* Runs the execution of commands. */
void	run_exec_steps(t_tools *sh, t_simple_cmds *tbl)
{
	int	redir_status;
	int	heredoc_status;

	redir_status = 0;
	heredoc_status = 0;
	handle_redirect(sh, tbl);
	if (sh->commands == NULL)
		return ;
	process_heredocs_run(sh, tbl);
	run_command_execution(sh, tbl);
	cleanup(sh);
}
