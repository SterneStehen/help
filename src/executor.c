/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+        
	+:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+      
	+#+        */
/*                                                +#+#+#+#+#+  
	+#+           */
/*   Created: 2024/05/23 21:19:12 by smoreron          #+#    #+#             */
/*   Updated: 2024/05/23 21:19:12 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/* Checks for incorrect redirection in command.
	* This function verifies if there are any invalid redirections 
	in the given command list.
	* It ensures that each redirection token is followed by a 
	valid token and not another redirection. */
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

/* Runs here documents (heredocs) in command list.
 * This function processes heredoc redirections in the command list.
 * It replaces the heredoc delimiters with their corresponding content. */
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

/* Checks for invalid redirections in commands.
 * This function checks each command in the list for invalid redirections.
 * If an invalid redirection is found,
	it processes heredoc commands and returns 1. */
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

/* Executes commands in a pipeline.
 * This function runs each command in the pipeline,
	handling the necessary process forking and piping.
 * It waits for all child processes to complete before returning. */
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

/* Creates a pipe and handles errors.
	* This function initializes a pipe and exits with an 
	error message if the pipe creation fails. */
int	initialize_pipe(int *fd, t_tools *shell)
{
	if (pipe(fd) == -1)
	{
		error_exit(shell, "pipe failed", 0);
		return (0);
	}
	return (1);
}

/* Forks and sets up child process for execution.

	* This function forks the current process and sets up the child process 
	for command execution,
 * including signal handling and file descriptor redirection. */
int	create_child_process(t_tools *shell, int *fd)
{
	pid_t	pid;

	pid = fork();
	shell->flag_ready_to_execute = 1;
	if (pid <= -1)
	{
		error_exit(shell, FORK_ERROR, 0);
		return (0);
	}
	else if (pid == 0)
	{
		configure_signals_child(&shell->terminal_setting);
		shell->flag_log = 1;
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		return (1);
	}
	return (2);
}

/* Executes commands in the parent process.
 * This function handles the execution of commands in the parent process,
 * ensuring proper signal handling and file descriptor redirection. */
void	execute_parent_process(t_simple_cmds *table, t_tools *shell, int *fd)
{
	shell->flag_log = 0;
	if (table->command != NULL)
		execute_builtin_commands(shell, table->command, table->arguments);
	close(fd[1]);
	dup2(fd[0], STDIN_FILENO);
	close(fd[0]);
}

/* Handles child process execution for a pipeline.
 * This function sets up the pipeline, forks the process,
	and manages redirections and command execution
 * for each child process in the pipeline. */
int	run_pipeline_child(t_simple_cmds *table, t_tools *shell)
{
	int		fd[2];
	int		exec_flag;
	int		fork_result;

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

/* Checks if a command has redirections.

	* This function checks the command's redirections 
	list to determine if it contains any output redirection tokens. */
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

/* Forks and executes a command, handling redirections.
 * This function forks the process and executes the command,
 * setting up necessary redirections and managing the child process. */
void	spawn_and_execute(t_simple_cmds *table, t_tools *shell, int *has_redir)
{
	pid_t	pid;
	int		status;

	pid = fork();
	shell->flag_ready_to_execute = 1;
	if (pid == -1)
	{
		error_exit(shell, FORK_ERROR, 0);
		return ;
	}
	else if (pid == 0)
	{
		configure_signals_child(&shell->terminal_setting);
		shell->flag_log = 1;
		*has_redir = validate_redirections(table);
		if (*has_redir == 0)
			dup2(shell->standard_output_fd, STDOUT_FILENO);
		manage_redirections(shell, table);
		execute_command(table, shell);
		exit(0);
	}
	configure_signals_parent();
	shell->flag_log = 0;
	waitpid(pid, &status, 0);
	shell->last_status = WEXITSTATUS(status);
}

/* Executes the final command in a pipeline.
 * This function handles the execution of the final command in the pipeline,
 * managing any necessary redirections and final cleanup. */
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

/* Restores standard input and output file descriptors.
 * This function resets the standard input and output file descriptors
 * to their original values,
	ensuring that subsequent commands can execute correctly. */
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

/* Executes built-in commands and restores file descriptors.
 * This function handles the execution of built-in commands and ensures that
 * file descriptors are properly reset after execution. */
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
		sh->last_status = REDIR_ERR_CODE;
	return (0);
}

/* Handles execution of a command table.
 * This function manages the execution of commands, including signal handling,
	redirections, and command execution. */
int	handle_execution(t_simple_cmds *cmd_table, t_tools *sh)
{
	char	**cmd_args_copy;
	char	**env_copy;
	int		exit_status;

	configure_signals_child(&sh->terminal_setting);
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
		free_string_array(cmd_args_copy);
		free_string_array(env_copy);
		return (exit_status);
	}
	return (exit_status);
}

/* Executes a single command.
 * This function handles the execution of a single command, managing forking,
	redirections, and cleanup. */
int	simple_execute(t_simple_cmds *commands, t_tools *tools)
{
	pid_t	child_pid;
	int		wait_status;

	tools->flag_pipe = 0;
	tools->flag_ready_to_execute = 1;
	child_pid = fork();
	if (child_pid == 0)
	{
		handle_execution(commands, tools);
		child_process_finish(tools);
	}
	else if (child_pid == -1)
	{
		error_exit(tools, FORK_ERR_MSG, 0);
		return (-1);
	}
	else
	{
		configure_signals_parent();
		tools->flag_log = 0;
		waitpid(child_pid, &wait_status, 0);
		tools->last_status = WEXITSTATUS(wait_status);
		run_builtin_and_close(commands, tools);
	}
	return (0);
}

/* Exits the program with an error message.
	* This function prints an error message and e
	xits the program with the given status. */
void	error_exit(t_tools *shell, const char *msg, int exit_st)
{
	if (shell->flag_log == 1)
	{
		printf("%s: %s:  %s\n", "minishell: ", shell->message, msg);
		shell->flag_log = 0;
	}
	if (exit_st != 2)
		exit(shell->last_status);
}

/* Handles directory errors in command execution.
 * This function handles errors related to directory commands,
 * printing an error message and exiting with a specific status code. */
void	handle_directory_command(char *cmd, t_tools *shell)
{
	if (shell->flag_log == 1)
	{
		printf("%s%s: %s\n", MINI, cmd, ISDIR);
		shell->flag_log = 0;
	}
	shell->last_status = 126;
	cleanup_child_proc(shell);
	exit(shell->last_status);
}

/* Executes the final command in a command table.
 * This function handles the execution of the final command in a command table,
 * managing redirections and ensuring proper cleanup. */
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
		if (shell->commands)
			error_exit(shell, strerror(errno), 0);
		free(command_path);
		clear_token_list(cmd_args_copy);
		clear_token_list(env_copy);
		exit(exec_result);
	}
}

/* Finalizes the execution of a command.
 * This function handles the final steps of executing a command,
 * including managing directory commands and finalizing the execution. */
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

/* This function checks for specific exit conditions for a 
command and exits with appropriate status codes. */

int	check_strings(char *str1, char *str2)
{
	int	i;

	i = 0;
	if (strlen(str1) != strlen(str2))
		return (0);
	while (str1[i] != '\0' && str2[i] != '\0')
	{
		if (str1[i] != str2[i])
			return (0);
		i++;
	}
	return (1);
}

int	validate_exit_conditions(t_simple_cmds *cmd)
{
	if (cmd->command == NULL || cmd->arguments[1] == NULL)
		return (0);
	if (!check_strings(cmd->command, "cat"))
		return (0);
	if (!check_strings(cmd->arguments[1], "-e"))
		return (0);
	return (1);
}

int	handle_exit_conditions(t_tools *shell, t_simple_cmds *cmd, char *cmd_path)
{
	if (validate_exit_conditions(cmd))
	{
		free(cmd_path);
		free_resources(shell);
		return (127);
	}
	return (-1);
}

int	handle_null_path(t_tools *shell, t_simple_cmds *cmd, char *cmd_path)
{
	if (cmd_path == NULL && strlen(cmd->command) == 0)
	{
		if (shell->flag_log)
		{
			printf("%s%s: %s\n", MINI, cmd->command, CMD_NOT_FND);
		}
		free(cmd_path);
		free_resources(shell);
		return (126);
	}
	return (-1);
}

int	exit_if_null(t_tools *shell, char *cmd_path, t_simple_cmds *table)
{
	int		exit_code;

	while (table->next != NULL)
		table = table->next;
	exit_code = handle_exit_conditions(shell, table, cmd_path);
	if (exit_code != -1)
		return (exit_code);
	exit_code = handle_null_path(shell, table, cmd_path);
	if (exit_code != -1)
		return (exit_code);
	if (shell->flag_log)
		printf("%s%s: %s\n", MINI, table->command, CMD_NOT_FND);
	free(cmd_path);
	free_resources(shell);
	return (127);
}

/* Executes a command.
 * This function handles the execution of a command,
 * including managing redirections, finding the command path,
	and executing built-in commands. */
int	execute_command(t_simple_cmds *command, t_tools *tools)
{
	char	*command_path;

	if (command->command == NULL)
		exit_if_null(tools, command_path, command);
	if (execute_builtin_commands(tools, command->command, command->arguments))
		exit(tools->last_status);
	else if (path_audit(command->command, tools))
		finalize_command_path(command->command, command, tools);
	else if (command->command[0] != '.' && command->command[0] != '/')
	{
		command_path = find_command_path(tools, command->command);
		if (command_path == NULL)
			error_exit(tools, "Command not found", 0);
		else if (access(command_path, X_OK) == 0)
			finalize_command_path(command_path, command, tools);
	}
	return (0);
}

/* Handles redirections in commands.
 * This function checks and handles redirections in the given command table,
 * ensuring that invalid redirections are properly managed. */
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

/* Processes and runs heredocs in command tables.
 * This function processes and executes heredoc commands in the command table,
 * replacing heredoc delimiters with their corresponding content. */
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

/* Performs the execution of command tables.
 * This function executes the given command tables,
	managing pipelines and simple commands as needed. */
void	run_command_execution(t_tools *tools, t_simple_cmds *commands)
{
	int				table_size;
	t_simple_cmds	*temp_tbl;

	g_global_flag = 0;
	table_size = 0;
	temp_tbl = commands;
	while (temp_tbl != NULL)
	{
		table_size++;
		temp_tbl = temp_tbl->next;
	}
	if (table_size == 1)
	{
		simple_execute(commands, tools);
	}
	else
	{
		tools->flag_pipe = 1;
		execute_pipeline_commands(commands, tools);
	}
}

/* Cleans up the shell structure.
 * This function releases resources associated with the shell structure,
 * ensuring proper cleanup after command execution. */
void	cleanup(t_tools *tools)
{
	cleanup_command_tables(tools->commands);
	tools->commands = NULL;
}

/* Main function to execute commands.
 * This function handles the overall execution flow of the shell,
 * managing redirections, heredocs, and command execution. */
void	run_exec_steps(t_tools *sh, t_simple_cmds *tbl)
{
	handle_redirect(sh, tbl);
	if (sh->commands == NULL)
		return ;
	process_heredocs_run(sh, tbl);
	run_command_execution(sh, tbl);
	cleanup(sh);
}
