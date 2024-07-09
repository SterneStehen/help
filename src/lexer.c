/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 18:23:50 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/09 07:01:10 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/* Checks if the character at the given index in the input string
is surrounded by double quotes.
   Returns 1 if the character is inside double quotes, otherwise 0. */
int	check_before(const char *input, int index)
{
	while (index-- > 1)
	{
		if (input[index] == '"')
		{
			return (1);
		}
	}
	return (0);
}

int	check_after(const char *input, int index)
{
	int	j;

	j = index;
	while (input[j] != '\0')
	{
		if (input[j] == '"')
		{
			return (1);
		}
		j++;
	}
	return (0);
}

int	inside_double_quotes(const char *input, int index)
{
	int	before;
	int	after;

	before = check_before(input, index);
	after = check_after(input, index);
	if (before && after)
	{
		return (1);
	}
	else
	{
		return (0);
	}
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
			return (1);
		}
		i++;
	}
	return (0);
}

/* Prints a syntax error message and returns 1. */
int	find_error_sint(char c)
{
	printf("Syntax error");
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
			return (1);
		}
		i++;
	}
	return (0);
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

/* Counts the number of escaped characters before the
given position in the string.
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

/* Checks if the character at the given index in the string
is preceded by a redirection operator.
   Returns 1 if the character is preceded by a redirection operator,
	otherwise 0. */
int	is_previous_char_redirection(const char *str, int index)
{
	if ((str[index - 1] == '<' || str[index - 1] == '>')
		&& str[index - 2] != '\\')
	{
		return (1);
	}
	if ((str[index - 1] == '<' || str[index - 1] == '>')
		&& str[index - 2] == '\\')
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
	int	index;

	index = 1;
	while (str[index] != '\0')
	{
		if (str[index] == '|')
		{
			if (is_previous_char_redirection(str, index))
			{
				printf("Syntax error\n");
				return (1);
			}
		}
		++index;
	}
	return (0);
}

int	check_less(char c1, char c2)
{
	if (c1 == '<' && c2 == '<')
		return (1);
	else
		return (0);
}

/* Audits the given string for invalid symbols and syntax errors.
   Returns 1 and prints a syntax error message if invalid symbols are found,
	otherwise returns 0. */
int	check_space_and_symbol(const char *str, int i)
{
	int	symbol;

	symbol = is_character_symbol(str[i - 1]);
	if (check_space(str[i]) && symbol == 1)
	{
		i = advance_past_whitespace(str, i);
		if (is_character_symbol(str[i]))
		{
			if (check_less(str[i - 2], str[i - 1]))
				return (0);
			else
			{
				printf("Syntax error\n");
				return (1);
			}
		}
	}
	return (-1);
}

int	audit_simbol_bad(const char *str)
{
	int	i;
	int	res;
	int	len;

	i = 1;
	if (has_invalid_pipe(str))
		return (1);
	while (str[i])
	{
		res = check_space_and_symbol(str, i);
		if (res != -1)
			return (res);
		i++;
	}
	len = strlen(str);
	res = is_character_symbol(str[len - 1]);
	if (res == 1)
	{
		printf("Syntax error\n");
		return (1);
	}
	return (0);
}

int	check_wrong_operators(char *str, int *last_status,
		int *flag_execution_completed)
{
	if (audit_simbol_bad(str))
	{
		*last_status = -1;
		*flag_execution_completed = 0;
		return (1);
	}
	return (0);
}

void	count_quotes2(char *str, int *single_quote_count,
		int *double_quote_count)
{
	int	i;

	*single_quote_count = 0;
	*double_quote_count = 0;
	i = 0;
	while (str[i] != '\0')
	{
		if (str[i] == '\'' || (str[i] == '\\' && str[i + 1] == '\''
				&& count_escaped_chars(str, i) % 2 == 0))
		{
			if (inside_double_quotes(str, i) == 0)
			{
				(*single_quote_count)++;
			}
		}
		else if (str[i] == '\"' || (str[i] == '\\' && str[i + 1] == '\"'
				&& count_escaped_chars(str, i) % 2 == 0))
		{
			(*double_quote_count)++;
		}
		i++;
	}
}

int	check_unclosed_quotes(char *str)
{
	int	single_quote_count;
	int	double_quote_count;

	count_quotes2(str, &single_quote_count, &double_quote_count);
	if (single_quote_count % 2 != 0 || double_quote_count % 2 != 0)
	{
		printf("Syntax error\n");
		return (1);
	}
	return (0);
}

int	check_bad_pipes(char *str)
{
	int	len;

	len = strlen(str) - 1;
	if (str[0] == '|' || str[len] == '|')
	{
		printf("Syntax error\n");
		return (1);
	}
	return (0);
}

int	check_empty_prompt(char *str)
{
	return (strlen(str) == 0);
}

/* Runs the lexer to tokenize the input string and check for syntax errors.
   Returns 0 if the command is ready to execute,
	otherwise returns 1 and prints a syntax error message. */

int	run_lexer(t_tools *tools)
{
	int	start;
	int	end;
	int	index;
	int	range[2];

	range[0] = 0;
	range[1] = -1;
	index = -1;
	if (check_empty_prompt(tools->shell_string))
		return (0);
	if (check_bad_pipes(tools->shell_string))
	{
		tools->last_status = -1;
		return (1);
	}
	if (check_unclosed_quotes(tools->shell_string))
		return (1);
	if (check_wrong_operators(tools->shell_string, &(tools->last_status),
			&(tools->flag_execution_completed)))
		return (1);
	tools->flag_execution_completed = 1;
	tools->pipes = pipe_split(tools->shell_string, index, range);
	return (0);
}
