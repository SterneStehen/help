/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 08:00:44 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/08 17:54:39 by smoreron         ###   ########.fr       */
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
void	process_redirection_run(t_tools *shell, t_simple_cmds *table,
		t_node *current_token)
{
	int	file_descriptor;

	file_descriptor = -1;
	if (current_token->class == LESS_LESS)
	{
		file_descriptor = create_heredoc_file(table, shell, current_token);
	}
	else if (current_token->class == GREAT
		|| current_token->class == GREAT_GREAT || current_token->class == LESS)
	{
		file_descriptor = descriptor_open_file(current_token->class,
				current_token->next->data, shell);
	}
	if (file_descriptor != -1 && redirect_io(current_token->class,
			file_descriptor, shell) == FALSE)
	{
		printf("Error changing stdin/stdout\n");
		exit(EXIT_FAILURE);
	}
}

int	manage_redirections(t_tools *shell, t_simple_cmds *table)
{
	t_node	*token;
	int		valid;

	token = table->redirections;
	while (token != NULL)
	{
		valid = check_valid_redirection(token);
		if (!valid)
		{
			handle_redirection_error(shell);
			return (0);
		}
		process_redirection_run(shell, table, token);
		if (token->next)
		{
			token = token->next->next;
		}
		else
		{
			token = NULL;
		}
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
//    Handles different redirection types and processes heredocs. */

int	process_input(int fd, t_tools *shell)
{
	if (fd == -1)
	{
		shell->last_status = errno;
		printf("Error: %s\n", strerror(errno));
		return (0);
	}
	return (duplicate_and_close(fd, STDIN_FILENO));
}

/* Redirects stdin and stdout based on the redirection type.
//    Handles different redirection types and processes heredocs. */

int	redirect_io(t_class type, int fd, t_tools *shell)
{
	int	result;

	result = 0;
	if (type == LESS_LESS)
	{
		result = process_heredoc_input(fd);
	}
	else if (type == LESS)
	{
		result = process_input(fd, shell);
	}
	else if (type == GREAT || type == GREAT_GREAT)
	{
		result = duplicate_and_close(fd, STDOUT_FILENO);
	}
	if (result == -1)
	{
		shell->last_status = errno;
		printf("Error: %s\n", strerror(errno));
		return (0);
	}
	return (1);
}
