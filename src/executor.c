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


/* Finds wrong redirections in the token list.
   Returns 1 if a wrong redirection is found, otherwise 0. */
int	find_invalid_redirection(t_node *start)
{
	t_node	*current;
	t_node	*possible_wrong;

	current = start;
	possible_wrong = current;
	while (possible_wrong != NULL)
	{
		if (!is_redirection_token(possible_wrong))
		{
			possible_wrong = possible_wrong->next;
			continue ;
		}
		if (!(is_redirection_token(possible_wrong)
				&& possible_wrong->next != NULL
				&& !is_redirection_token(possible_wrong->next)))
		{
			break ;
		}
		possible_wrong = possible_wrong->next->next;
	}
	return (possible_wrong != NULL);
}

/* Checks for invalid redirections and handles them.
   Returns 1 if an invalid redirection is found and handled, otherwise 0. */
int	invalid_redirection(t_tools *shell, t_node *token, t_simple_cmds *table)
{
	t_node	*wrong;
	int		wrong_found;

	wrong = token;
	wrong_found = find_invalid_redirection(wrong);
	if (!wrong_found)
		return (0);
	perform_only_hd(shell, table, wrong);
	return (1);
}

/* Performs here documents up to the given end token.
   Processes heredoc redirections in the command table. */
void	perform_only_hd(t_tools *shll, t_simple_cmds *tabl, t_node *end)
{
	t_node	*start;
	char	*tmp;

	start = tabl->redirections;
	while (start != end && start != NULL)
	{
		if (start->class == LESS_LESS)
		{
			tmp = get_break_string(start->next->data, shll);
			free(start->next->data);
			start->next->data = tmp;
			generate_temp_heredoc(tabl, start->next->data, shll);
		}
		start = start->next;
	}
}

/* Checks if the current token has a valid redirection.
   Returns 1 if valid, otherwise 0. */
int	check_valid_redirection(t_node *curr)
{
	return (curr && (curr->class == LESS_LESS || curr->class == GREAT_GREAT
			|| curr->class == LESS || curr->class == GREAT) && curr->next
		&& curr->next->class == STRING);
}

/* Handles redirection errors.
   Sets the last status to 258 and prints an error message. */
void	handle_redirection_error(t_tools *shell)
{
	shell->last_status = 258;
	printf("Redirection error\n");
}

/* Manages redirections for a command.
   Checks and processes redirections, returns 1 if successful, otherwise 0. */
int	manage_redirections(t_tools *shell, t_simple_cmds *table)
{
	t_node	*current_token;
	int		file_descriptor;
	int		is_valid;

	current_token = table->redirections;
	while (current_token != NULL)
	{
		is_valid = check_valid_redirection(current_token);
		if (!is_valid)
		{
			handle_redirection_error(shell);
			return (0);
		}
		if (current_token->class == LESS_LESS)
		{
			file_descriptor = create_heredoc_file(table, shell, current_token);
		}
		else if (current_token->class == GREAT
			|| current_token->class == GREAT_GREAT
			|| current_token->class == LESS)
		{
			file_descriptor = descriptor_open_file(current_token->class,
					current_token->next->data, shell);
		}
		else
		{
			file_descriptor = -1;
		}
		if (file_descriptor != -1 && redirect_io(current_token->class,
				file_descriptor, shell) == FALSE)
		{
			printf("Error changing stdin/stdout\n");
			exit(EXIT_FAILURE);
		}
		current_token = current_token->next ? current_token->next->next : NULL;
	}
	return (1);
}

/* Duplicates a file descriptor and closes the old one.
   Returns the result of dup2. */
int	duplicate_and_close(int oldfd, int newfd)
{
	int	result;

	result = dup2(oldfd, newfd);
	close(oldfd);
	return (result);
}

/* Handles errors by setting the last status and printing an error message. */
void	report_error(t_tools *shell)
{
	shell->last_status = errno;
	printf("Error: %s\n", strerror(errno));
	// Assuming free_at_child is defined elsewhere and frees necessary resources
	// free_at_child(shell);
}

/* Processes a here document.
   Duplicates the file descriptor to STDIN and unlinks the temporary file. */
int	process_heredoc_input(int fd)
{
	int	ret_val;

	ret_val = duplicate_and_close(fd, STDIN_FILENO);
	unlink("/tmp/heredoc.XXXXXX");
	return (ret_val);
}

/* Redirects stdin and stdout based on the redirection type.
   Handles different redirection types and processes heredocs. */
int	redirect_io(t_class type, int fd, t_tools *shell)
{
	int	result;

	result = 0;
	while (1)
	{
		if (type == LESS_LESS)
		{
			result = process_heredoc_input(fd);
			break ;
		}
		if (type == LESS)
		{
			if (fd == -1)
				return (0);
			result = duplicate_and_close(fd, STDIN_FILENO);
			break ;
		}
		if (type == GREAT || type == GREAT_GREAT)
		{
			result = duplicate_and_close(fd, STDOUT_FILENO);
			break ;
		}
		break ;
	}
	if (result == -1)
	{
		report_error(shell);
		return (0);
	}
	return (1);
}


/* Finds the last heredoc token in the redirections list.
   Returns a pointer to the heredoc token if found, otherwise NULL. */
t_node	*locate_heredoc_token(t_node *redirs_list)
{
	t_node	*heredoc_token;

	heredoc_token = NULL;
	while (redirs_list != NULL)
	{
		if (redirs_list->class == LESS_LESS)
			heredoc_token = redirs_list;
		redirs_list = redirs_list->next;
	}
	return (heredoc_token);
}

/* Restores the standard input file descriptor to its original state.
   Returns 0 on success, and -1 on failure with an error message. */
int	restore_standard_input(t_tools *shell)
{
	if (dup2(shell->standard_input_fd, STDIN_FILENO) == -1)
	{
		printf("Failed to restore standard input\n");
		return (-1);
	}
	return (0);
}

/* Creates a heredoc by finding the heredoc token and restoring stdin.
   Opens the heredoc file for reading and returns the file descriptor. */
int	create_heredoc_file(t_simple_cmds *table, t_tools *shell, t_node *token)
{
	int		fd;
	t_node	*heredoc_token;

	heredoc_token = locate_heredoc_token(table->redirections);
	if (heredoc_token != token)
		return (-1);
	if (restore_standard_input(shell) == -1)
		return (-1);
	fd = open(table->hd_file_name, O_RDONLY);
	if (fd == -1)
	{
		printf("Failed to open heredoc file: %s\n", table->hd_file_name);
		return (-1);
	}
	return (fd);
}

/* Opens a file for reading and returns the file descriptor. */
int	open_file_for_reading(char *file_name)
{
	int	fd;

	fd = open(file_name, O_RDONLY);
	return (fd);
}

/* Opens a file for writing with the given flags and returns the file descriptor. */
int	open_file_for_writing(char *file_name, int flags)
{
	int	fd;

	fd = open(file_name, flags, 0644);
	return (fd);
}

/* Handles errors when opening a file.
   Prints an error message and sets the last status to 1. */
int	handle_file_open_error(char *file_name, t_tools *shell)
{
	if (shell->flag_log)
		printf("%s%s: %s\n", "SHELL", file_name, strerror(errno));
	shell->last_status = 1;
	return (-1);
}

/* Opens a file based on the redirection type.
   Returns the file descriptor or -1 on error. */
int	descriptor_open_file(t_class type, char *file_name, t_tools *shell)
{
	int	fd;

	if (type == LESS)
		fd = open_file_for_reading(file_name);
	else if (type == GREAT)
		fd = open_file_for_writing(file_name, O_WRONLY | O_CREAT | O_TRUNC);
	else if (type == GREAT_GREAT)
		fd = open_file_for_writing(file_name, O_WRONLY | O_CREAT | O_APPEND);
	if (fd == -1)
		return (handle_file_open_error(file_name, shell));
	return (fd);
}

/* Removes quotes from a string.
   Returns a new string without quotes. */
char	*trim_quotes(char *str)
{
	size_t	len;
	char	*new_str;
	size_t	j;

	len = strlen(str);
	new_str = smalloc(len + 1);
	j = 0;
	for (size_t i = 0; i < len; i++)
	{
		if (str[i] != '\'' && str[i] != '\"')
		{
			new_str[j++] = str[i];
		}
	}
	new_str[j] = '\0';
	return (new_str);
}

/* Checks for expansions in the stop word for heredoc.
   Sets the flag_hd in the shell structure. */
int	validate_expansion(char *str, t_tools *shell)
{
	size_t	i;

	i = 0;
	shell->flag_hd = 1;
	while (str[i] != '\0')
	{
		if (str[i] == '\'' || str[i] == '\"')
		{
			shell->flag_hd = 0;
			break ;
		}
		i++;
	}
	return (0);
}

/* Processes the stop word for heredoc.
   Returns the stop word without quotes. */
char	*get_break_string(char *str, t_tools *shell)
{
	char	*trimmed_result;

	validate_expansion(str, shell);
	trimmed_result = trim_quotes(str);
	return (trimmed_result);
}

/* Runs here documents in the command table.
   Processes and generates heredoc files. */
int	execute_heredocs(t_simple_cmds *cmd_tbl, t_tools *shell)
{
	t_node	*tk;
	char	*tmp;

	tk = cmd_tbl->redirections;
	while (tk != NULL)
	{
		if (tk->class == LESS_LESS)
		{
			validate_expansion(tk->next->data, shell);
			tmp = trim_quotes(tk->next->data);
			free(tk->next->data);
			tk->next->data = tmp;
			cmd_tbl->hd_file_name = generate_temp_heredoc(cmd_tbl,
					tk->next->data, shell);
		}
		tk = tk->next;
	}
	return (0);
}

/* Cleans up the specified filename by unlinking it.
   Returns 0 on success. */
int	clean_filename(char *filename)
{
	unlink(filename);
	return (0);
}

/* Concatenates two strings and returns the result.
   Allocates memory for the new string. */
char	*concatenate_strings(char *str1, char *str2)
{
	char	*result;

	result = (char *)smalloc(strlen(str1) + strlen(str2) + 1);
	strcpy(result, str1);
	strcat(result, str2);
	return (result);
}

/* Generates a unique filename for heredoc.
   Cleans up the previous filename if it exists. */
char	*generate_temp_filename(t_simple_cmds *table)
{
	char	*base_path;
	char	*extension;

	char *part1, *part2, *index_str;
	base_path = "/tmp/heredoc";
	extension = ".XXXXXX";
	if (table->hd_file_name != NULL)
	{
		clean_filename(table->hd_file_name);
		return (table->hd_file_name);
	}
	index_str = ft_itoa(table->count);
	part1 = concatenate_strings(base_path, index_str);
	free(index_str);
	part2 = concatenate_strings(part1, extension);
	free(part1);
	clean_filename(part2);
	return (part2);
}

/* Writes user input to the heredoc file until the stop word is encountered.
   Expands variables in the input if flag_hd is set. */
int	write_to_file(int fd, char *stop_word, t_tools *shell)
{
	char	*input;
	size_t		len;

	while (1)
	{
		write(0, "> ", 2);
		input = get_next_line(STDIN_FILENO);
		len = strlen(stop_word);
		if (input == NULL || (ft_strncmp(input, stop_word, len) == 0
				&& ((strlen(input) - 1) == len)))
		{
			free(input);
			break ;
		}
		if (shell->flag_hd == 1)
			expand_variables(&input, shell);
		write(fd, input, ft_strlen(input));
		free(input);
	}
    return 0;
}

/* Generates a heredoc file and writes input to it.
   Returns the filename of the generated heredoc. */
char	*generate_temp_heredoc(t_simple_cmds *cmd_tbl, char *stop_word,
		t_tools *shell)
{
	int	fd;

	cmd_tbl->hd_file_name = generate_temp_filename(cmd_tbl);
	fd = open(cmd_tbl->hd_file_name, O_RDWR | O_CREAT | O_EXCL, 0600);
	if (fd == -1)
	{
		printf("%s%s\n", "SHELL", strerror(errno));
		return (NULL);
	}
	write_to_file(fd, stop_word, shell);
	shell->requires_expansion = 0;
	close(fd);
	return (cmd_tbl->hd_file_name);
}


/* Constructs the full path for a given command by combining the base path and the command.
   Allocates memory for the full path, which needs to be freed by the caller. */
char	*build_path(const char *base, const char *cmd)
{
	char	*slash_cmd;
	char	*full_path;

	slash_cmd = smalloc(strlen("/") + strlen(cmd) + 1);
	strcpy(slash_cmd, "/");
	strcat(slash_cmd, cmd);
	full_path = smalloc(strlen(base) + strlen(slash_cmd) + 1);
	strcpy(full_path, base);
	strcat(full_path, slash_cmd);
	free(slash_cmd);
	return (full_path);
}

/* Logs an error message and sets the last status code in the shell structure.
   Returns 0 and sets the last status code to the provided code. */
int	report_error_and_exit(t_tools *sh, const char *cmd, const char *msg,
		int code)
{
	if (sh->flag_log)
		printf("%c%s: %s\n", '$', cmd, msg);
	sh->last_status = code;
	return (0);
}

/* Finds the full path of a command by searching through the directories in the PATH environment variable.
   Returns the full path if the command is found and executable,
	otherwise logs an error and returns NULL. */
char	*find_command_path(t_tools *sh, const char *cmd)
{
	int		idx;
	char	*path;

	idx = 0;
	if (strlen(cmd) == 0)
		return (NULL);
	if (sh->paths == NULL)
		exit(EXIT_FAILURE);
	while (sh->paths[idx] != NULL)
	{
		path = build_path(sh->paths[idx], cmd);
		if (access(path, X_OK) == 0)
			return (path);
		free(path);
		idx++;
	}
	return (report_error_and_exit(sh, cmd, strerror(ENOENT), 127), NULL);
}

/* Checks if the given path is accessible and executable.
   Returns 1 if accessible, otherwise logs an error and returns 0. */
int	check_file_access(char *path, t_tools *sh)
{
	if (access(path, X_OK) == 0)
		return (1);
	else
	{
		sh->last_status = -1;
		if (sh->flag_log)
			printf("%c%s: %s\n", '$', path, strerror(ENOENT));
		return (0);
	}
}

/* Audits the command path to check if it is valid and accessible.
   Returns 1 if the path is valid and accessible,
	otherwise logs an error and returns 0. */
int	audit_comand_path(char *path, t_tools *sh)
{
	int	len;

	len = strlen(path);
	sh->flag_log = 0;
	while (1)
	{
		if (path[0] == '.' && path[1] == '.' && path[2] == '/')
			return (check_file_access(path, sh));
		if (path[0] == '.' && path[1] == '/' && len == 2)
		{
			sh->last_status = 127;
			if (sh->flag_log)
				printf("%c%s: %s\n", '$', sh->shell_string, strerror(ENOENT));
			return (0);
		}
		if (path[0] == '.' || path[0] == '/' || path)
		{
			if (path[0] == '.' && path[1] != '/')
			{
				sh->last_status = 127;
				if (sh->flag_log)
					printf("%c%s: %s\n", '$', path, strerror(ENOENT));
				return (0);
			}
			else if (path[0] == '.' || path[0] == '/' || path)
				return (check_file_access(path, sh));
			else if (sh->flag_envair)
			{
				sh->last_status = 127;
				if (sh->flag_log)
					printf("%c%s: %s\n", '$', path, strerror(ENOENT));
				return (0);
			}
		}
		break ;
	}
	return (0);
}


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
			printf("%s%s\n", SHELL, PIPE_ER);
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
		run_builtin(shell, table->command, table->arguments);
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
		run_builtin(shell, table->command, table->arguments);
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
		run_builtin(sh, cmd_table->command, cmd_table->arguments);
	}
	if (!invalid_redirection(sh, cmd_table->redirections, cmd_table))
	{
		reset_file_descriptors(sh);
	}
	else
	{
		sh->last_status = -1;
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
	//int		exec_status;
	pid_t	child_pid;
	int		wait_status;

	sh->flag_pipe = 0;
	sh->flag_ready_to_execute = 1;
	child_pid = fork();
	if (child_pid == 0)
	{
		//exec_status = 
        handle_execution(cmd_table, sh);
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
	if (run_builtin(shell, table->command, table->arguments))
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
	//int				glob_c;

	glob_c = 0;
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
//	int	redir_status;
//	int	heredoc_status;

//	redir_status = 0;
//	heredoc_status = 0;
	handle_redirect(sh, tbl);
	if (sh->commands == NULL)
		return ;
	process_heredocs_run(sh, tbl);
	run_command_execution(sh, tbl);
	cleanup(sh);
}
