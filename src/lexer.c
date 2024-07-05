/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 18:23:50 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/05 07:17:16 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/* Checks if the character at the given index in the input string is surrounded by double quotes.
   Returns 1 if the character is inside double quotes, otherwise 0. */
int	inside_double_quotes(const char *input, int index)
{
	int	surrounded;
	int	j;

	surrounded = 0;
	j = index;
	while (index-- > 1)
	{
		if (input[index] == '"')
		{
			surrounded = 1;
			break ;
		}
	}
	while (input[j] != '\0')
	{
		if (input[j] == '"' && surrounded == 1)
		{
			surrounded = 2;
			break ;
		}
		j++;
	}
	return ((surrounded == 2) ? 1 : 0);
}

/* Checks if the input string contains any single or double quotes.
   Returns 1 if quotes are found, otherwise 0. */
int	contains_quote(const char *input)
{
	int	i;

	i = 0;
	while (input[i] != '\0')
	{
		if (input[i] == '\'' || input[i] == '"')
		{
			return (1); // Returns 1 if the string contains quotes
		}
		i++;
	}
	return (0); // Returns 0 if the string does not contain quotes
}

/* Prints a syntax error message and returns 1. */
int	find_error_sint(char c)
{
	printf("Syntax error\n");
	return (1);
}

/* Prints a syntax error message for newline and returns 1. */
int	find_error_sing_newline(void)
{
	printf("Syntax error\n");
	return (1);
}

/* Checks if the given character is a shell operator (|, <, >).
   Returns 1 if the character is an operator, otherwise 0. */
int	is_character_symbol(char c)
{
	const char	*operators = "|<>";
	int			i;

	i = 0;
	while (operators[i])
	{
		if (operators[i] == c)
		{
			return (1); // Returns 1 if the character is an operator
		}
		i++;
	}
	return (0); // Returns 0 if the character is not an operator
}

/* Checks for invalid redirection operators (e.g.,
	>>> or <<<) in the given string.
   Returns 1 and prints a syntax error message if invalid operators are found,
	otherwise returns 0. */
int	check_redir(const char *str)
{
	int	length;
	int	i;

	length = strlen(str);
	i = 0;
	if (length <= 2)
	{
		return (0);
	}
	while (i <= length - 3)
	{
		if ((str[i] == '>' && str[i + 1] == '>' && str[i + 2] == '>')
			|| (str[i] == '<' && str[i + 1] == '<' && str[i + 2] == '<'))
		{
			printf("Syntax error\n");
			return (1);
		}
		i++;
	}
	return (0);
}

/* Counts the number of escaped characters before the given position in the string.
   Returns the count of escaped characters. */
int	count_escaped_chars(const char *str, int pos)
{
	int	count;

	count = 0;
	while (pos > 0 && str[--pos] == '\\')
	{
		count++;
	}
	return (count);
}

/* Checks if the character at the given index in the string is preceded by a redirection operator.
   Returns 1 if the character is preceded by a redirection operator,
	otherwise 0. */
int	is_previous_char_redirection(const char *str, int index)
{
	if ((str[index - 1] == '<' || str[index - 1] == '>') && str[index
		- 2] != '\\')
	{
		return (1);
	}
	if ((str[index - 1] == '<' || str[index - 1] == '>') && str[index
		- 2] == '\\')
	{
		if (count_escaped_chars(str, index - 1) % 2 == 0)
		{
			return (1);
		}
	}
	return (0);
}

/* Checks for invalid pipe characters in the given string.
   Returns 1 and prints a syntax error message if invalid pipes are found,
	otherwise returns 0. */
int	has_invalid_pipe(const char *str)
{
	for (int i = 1; str[i] != '\0'; ++i)
	{
		if (str[i] == '|')
		{
			if (is_previous_char_redirection(str, i))
			{
				printf("Syntax error\n");
				return (1);
			}
		}
	}
	return (0);
}

/* Audits the given string for invalid symbols and syntax errors.
   Returns 1 and prints a syntax error message if invalid symbols are found,
	otherwise returns 0. */
int	audit_simbol_bad(const char *str)
{
	int	i;

	i = 0;
	if (has_invalid_pipe(str))
	{
		return (1);
	}
	while (str[++i])
	{
		if (is_character_symbol(str[i - 1]) && check_space(str[i]))
		{
			i = advance_past_whitespace(str, i);
			if (is_character_symbol(str[i]))
			{
				if (str[i] == '<' && str[i + 1] == '<')
				{
					return (0);
				}
				else
				{
					printf("Syntax error\n");
					return (1);
				}
			}
		}
	}
	if (is_character_symbol(str[strlen(str) - 1]))
	{
		printf("Syntax error\n");
		return (1);
	}
	return (0);
}

/* Runs the lexer to tokenize the input string and check for syntax errors.
   Returns 0 if the command is ready to execute,
	otherwise returns 1 and prints a syntax error message. */
int	run_lexer(t_tools *shell)
{
	int	len;
	int	start;
	int	end;
	int	index;

	int single_quote_count, double_quote_count, i;
	end = -1;
	start = 0;
	index = -1;
	// Check for empty prompt
	if (strlen(shell->shell_string) == 0)
		return (0); // No command to execute
	// Check for bad pipes
	len = strlen(shell->shell_string) - 1;
	if (shell->shell_string[0] == '|')
	{
		shell->last_status = -1;
		printf("Syntax error\n");
		return (1); // Syntax error at the beginning
	}
	else if (shell->shell_string[len] == '|')
	{
		shell->last_status = -1;
		printf("Syntax error\n");
		return (1); // Syntax error at the end
	}
	// Check for unclosed quotes
	single_quote_count = 0;
	double_quote_count = 0;
	i = -1;
	while (shell->shell_string[++i] != '\0')
	{
		if (shell->shell_string[i] == '\'' || (shell->shell_string[i] == '\\'
			&& shell->shell_string[i + 1] != '\0' && shell->shell_string[i
			+ 1] == '\'' && count_escaped_chars(shell->shell_string, i)
			% 2 == 0))
		{
			if (inside_double_quotes(shell->shell_string, i) == 0)
				single_quote_count++;
		}
		else if (shell->shell_string[i] == '\"'
			|| (shell->shell_string[i] == '\\' && shell->shell_string[i
				+ 1] != '\0' && shell->shell_string[i + 1] == '\"'
				&& count_escaped_chars(shell->shell_string, i) % 2 == 0))
			double_quote_count++;
	}
	if (single_quote_count % 2 != 0)
	{
		printf("Syntax error\n");
		return (1); // Unclosed single quote error
	}
	else if (double_quote_count % 2 != 0)
	{
		printf("Syntax error\n");
		return (1); // Unclosed double quote error
	}
	// Check for wrong operators
	if (audit_simbol_bad(shell->shell_string))
	{
		shell->last_status = -1;
		shell->flag_execution_completed = 0;
		return (1); // Wrong operator error
	}
	shell->flag_execution_completed = 1;
	shell->pipes = split_pipe_segments(shell->shell_string, start, end, index);
	return (0); // Command ready to execute
}
