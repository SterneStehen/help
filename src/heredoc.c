/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 07:13:43 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/07 17:24:40 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"


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
	int		len;

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
