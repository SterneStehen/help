/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 21:25:23 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/05 07:48:23 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"



char	*ft_get_join(char const *s1, char const *s2)
{
	char	*str;
	size_t	z;
	size_t	s;

	if (!s1 || !s2)
		return (NULL);
	str = malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (!str)
		return (NULL);
	z = 0;
	s = 0;
	while (s1[z])
	{
		str[s++] = s1[z];
		z++;
	}
	z = 0;
	while (s2[z])
	{
		str[s++] = s2[z];
		z++;
	}
	str[s] = '\0';
	return (str);
}

/* Checks if a count is odd.
   Returns 1 if odd, 0 otherwise. */
int	is_odd(int count)
{
	return (count % 2);
}

/* Counts the number of occurrences of the specified quote character
   in the string `str` up to position `pos`.
   Returns the count of quote characters. */

int	count_quotes(char *str, int pos, char quote_char)
{
	int	count;

	count = 0;
	while (pos >= 0)
	{
		if (str[pos] == quote_char)
		{
			count++;
		}
		pos--;
	}
	return (count);
}

/* Checks conditions based on the counts of single and double quotes,
   the initial position, and the input string.
   Returns 1 if conditions are met for processing, 0 otherwise. */
int	check_conditions(int sq_count, int dq_count, int initial_position,
		char *input_str)
{
	if (is_odd(sq_count) && !is_odd(dq_count))
	{
		return (1);
	}
	else if (is_odd(sq_count) && is_odd(dq_count) && input_str[initial_position
		- 1] == '"')
	{
		return (1);
	}
	else if (!is_odd(sq_count) && is_odd(dq_count))
	{
		return (0);
	}
	else if (is_odd(sq_count) && dq_count % 4 == 2 && input_str[initial_position
		- 1] == '\'')
	{
		return (1);
	}
	else if (!is_odd(sq_count) && dq_count % 4 == 2
		&& input_str[initial_position - 1] == '\'' && input_str[initial_position
		- 3] == '\'')
	{
		return (1);
	}
	else if (sq_count % 4 == 0 && dq_count % 4 == 3)
	{
		return (1);
	}
	return (0);
}

/* Determines if expansion should be skipped based on the input string
   and position.
   Returns 1 if expansion should be skipped, 0 otherwise. */
int	bypass_expansion(char *input_str, int position)
{
	int	sq_count;
	int	dq_count;
	int	initial_position;
	int	backtrack_needed;
	int	temp_position;

	sq_count = 0;
	dq_count = 0;
	initial_position = position;
	backtrack_needed = 0;
	if (position == 0)
	{
		return (0);
	}
	if (position > 0 && (input_str[position + 1] == '"' || input_str[position
			+ 1] == '\''))
	{
		return (1);
	}
	sq_count = count_quotes(input_str, position, '\'');
	dq_count = count_quotes(input_str, position, '"');
	if (check_conditions(sq_count, dq_count, initial_position, input_str))
	{
		return (1);
	}
	temp_position = initial_position;
	while (temp_position != 0)
	{
		if (input_str[temp_position] == '\'' || input_str[temp_position] == '"')
		{
			backtrack_needed = 0;
		}
		temp_position--;
	}
	if (!backtrack_needed)
	{
		if (check_conditions(sq_count, dq_count, initial_position, input_str))
		{
			return (1);
		}
	}
	return (0);
}

/* Checks if the string `str` contains a dollar sign '$' that should trigger variable expansion.
   Updates `shell->flag_execution_completed` if a dollar sign is found followed by an invalid character.
   Returns TRUE if a valid dollar sign expansion is found,
	otherwise returns FALSE. */

int	is_space_after_dollar(const char *str, int i)
{
	const char	*spaces = SIMBOL;
	int			k;

	k = 0;
	while (spaces[k] != '\0')
	{
		if (spaces[k] == str[i + 1])
		{
			return (1);
		}
		k++;
	}
	return (0);
}

/* Handles conditions related to dollar signs ('$') in the input string
   based on the position and updates the context as needed.
   Returns 1 to continue the loop, -1 to stop with false, -2 to stop with true,
   or 0 if no special condition is met. */
int	process_dollar_conditions(char *input, t_tools *context, int pos)
{
	if ((input[pos] == '$' && count_escaped_chars(input, pos) % 2 != 0)
		|| (input[pos] == '$' && bypass_expansion(input, pos)))
	{
		return 1; // Continue loop
	}
	else if (input[pos] == '$')
	{
		if (is_space_after_dollar(input, pos))
		{
			context->flag_execution_completed = 0;
			return -1; // Stop loop with false
		}
		else
		{
			return -2; // Stop loop with true
		}
	}
	return 0; // No special condition
}

/* Checks if the input string contains a dollar sign ('$') that
   should trigger variable expansion, updating the context as needed.
   Returns 1 if a valid dollar sign is found, 0 otherwise. */
int	detect_dollar_usa(char *input, t_tools *context)
{
	int	pos;
	int	result;

	pos = 0;
	while (input[pos] != '\0')
	{
		if (input[0] == '$' && input[pos + 1] != ' ' && input[pos + 1] != '\0')
		{
			return 1;
		}
		else if (input[pos] == '$' && input[pos + 1] == '\0')
		{
			break ;
		}
		else
		{
			result = process_dollar_conditions(input, context, pos);
			if (result == 1)
			{
				pos++;
				continue ;
			}
			else if (result == -1)
			{
				return 0;
			}
			else if (result == -2)
			{
				return 1;
			}
		}
		pos++;
	}
	return 0;
}

/* Duplicates the substring of `s` starting from index `index` where a dollar sign ('$') is found.
   Handles different delimiters and expands the dollar sign segment before copying.
   Modifies `dst` to contain the duplicated substring. */
void	copy_doll_usa(char **segment, char **source, int idx)
{
	char	delim;
	int		start_pos;
	int		current_pos;

	if (source[0][idx + 1] == '?')
	{
		segment[0] = duplicate_string_range(source[0], idx, idx + 2);
		return ;
	}
	start_pos = idx;
	current_pos = idx + 1;
	delim = ' ';
	if (source[0][idx + 1] == '(')
		delim = ')';
	if (idx > 0 && source[0][idx - 1] == '\'')
		delim = '\'';
	if (idx > 0 && source[0][idx - 1] == '\"')
		delim = '\"';
	while (source[0][start_pos] != '\0' && source[0][start_pos] != delim
		&& ((source[0][current_pos] >= '0' && source[0][current_pos] <= '9')
			|| (source[0][current_pos] >= 'A'
				&& source[0][current_pos] <= 'z')))
	{
		start_pos++;
		current_pos++;
	}
	segment[0] = duplicate_string_range(source[0], idx, start_pos + 1);
}

/* Isolates the segment of `s` containing the dollar sign ('$') for expansion.
   Splits `s` into `bef_doll` (portion before the dollar sign segment) and `rest` (portion after).
   Expands the dollar sign segment and updates `s` to reflect the expanded result. */
int	extract_dollar_part(char **str, t_tools *tools, char **before_dollar,
		char **remaining)
{
	int		idx;
	char	**dollar_seg;
	char	*expanded_val;

	dollar_seg = smalloc(sizeof(char *));
	if (!dollar_seg)
		return -1;
	idx = 0;
	while (str[0][idx] != '\0')
	{
		if (str[0][idx] == '$' && count_escaped_characters(str[0], idx) % 2 == 0
			&& bypass_expansion(str[0], idx) != TRUE)
		{
			before_dollar[0] = duplicate_string_range(str[0], 0, idx);
			copy_doll_usa(dollar_seg, str, idx);
			expanded_val = expand_dollar_signs(dollar_seg[0], tools);
			remaining[0] = duplicate_string_range(str[0], idx
					+ strlen(*dollar_seg), strlen(str[0]));
			free(dollar_seg[0]);
			*dollar_seg = ft_strjoin(before_dollar[0], expanded_val);
			before_dollar[0] = ft_get_join(dollar_seg[0], remaining[0]);
			free(dollar_seg);
			free(str[0]);
			*str = *before_dollar;
			return 0;
		}
		idx++;
	}
	free(dollar_seg);
	return 0;
}

/* Calculates the length of the string `str`.
   Returns the length of the string. */
int	calculate_length(char *str)
{
	int	length;

	length = 0;
	while (str[length] != '\0')
	{
		length++;
	}
	return length;
}

/* Copies the content of the variable `content` into a newly allocated string.
   Returns the copied content on success, or NULL on error. */
char	*duplicate_data(char *content)
{
	int		len;
	char	*copied_content;
	int		index;

	len = calculate_length(content) + 1;
	copied_content = smalloc(sizeof(char) * len);
	if (!copied_content)
		return NULL;
	index = 0;
	while (content[index] != '\0')
	{
		copied_content[index] = content[index];
		index++;
	}
	copied_content[index] = '\0';
	return copied_content;
}

/* Allocates memory of the specified size and returns a pointer to it.
   Prints an error message and returns NULL if allocation fails. */
char	*allocate_memory(size_t size)
{
	char	*ptr;

	ptr = malloc(size);
	if (!ptr)
	{
		printf("Error: Memory allocation failed.\n");
		return (NULL);
	}
	return (ptr);
}

/* Expands variables in the input string using context's environment.
   Continues expanding until all variable references ('$') are processed. */
int	expand_variables(char **input, t_tools *context)
{
	char	**segment1;
	char	**segment2;

	while (detect_dollar_usa(*input, context))
	{
        segment1 = (char **)allocate_memory(sizeof(char *));
        segment2 = (char **)allocate_memory(sizeof(char *));
		extract_dollar_part(input, context, segment1, segment2);
		free(segment2[0]);
		free(segment1);
		free(segment2);
	}
	return (1);
}

/* Processes a command string by expanding variables and converting to lowercase.
   Returns 1 if the command matches "awk", otherwise returns 0. */
int	run_process_command(char *cmd, t_tools *context)
{
	char	*temp_str;
	int		j;

	if (cmd != NULL)
	{
		if (cmd[0] == '$')
		{
			expand_variables(&cmd, context);
		}
		temp_str = ft_strdup(cmd);
		j = 0;
		while (temp_str[j] != '\0')
		{
			temp_str[j] = tolower(temp_str[j]);
			j++;
		}
		return (are_strings_equal(temp_str, "awk") == 0 ? 1 : 0);
	}
	return (0);
}

/* Processes tokens by expanding variables in each token's data field. */
void	process_tokens(t_node *tokens, t_tools *context)
{
	while (tokens != NULL)
	{
		expand_variables(&tokens->data, context);
		tokens = tokens->next;
	}
}

/* Expands variables in multiple command tables and their respective tokens. */
int	multiple_tables_expand(t_simple_cmds *commands, t_tools *context)
{
	char	*temp_result;

	while (commands != NULL)
	{
		temp_result = NULL;
		if (run_process_command(commands->command, context))
		{
			process_tokens(commands->token_arg, context);
			process_tokens(commands->redirections, context);
		}
		commands = commands->next;
		free(temp_result);
	}
	return (1);
}

/* Copies a variable value to a newly allocated string.
   Returns an empty string if the input is NULL or empty. */
char	*copy_variable(const char *var_val)
{
	char	*empty_str;
	char	*copied_val;

	if (!var_val || *var_val == '\0')
	{
		empty_str = allocate_memory(sizeof(char *));
		if (empty_str)
		{
			empty_str[0] = '\0';
		}
		return (empty_str);
	}
	copied_val = strdup(var_val);
	return (copied_val);
}

/* Processes a string enclosed in parentheses to find and return its value from the environment.
   Returns an empty string if the variable is not found in the environment. */
char	*handle_parentheses(char *trimmed_input, t_tools *context)
{
	int				index;
	char			*trimmed_var;
	t_environment	*env_var;
	char			*empty_str;


	index = 0;
	while (trimmed_input[index] != '\0')
	{
		index++;
	}
	if (trimmed_input[index - 1] == ')')
	{
		trimmed_var = trim_string(trimmed_input, "( )");
		env_var = ft_find(context->envair, trimmed_var);
		free(trimmed_var);
		if (!env_var)
		{
			empty_str = allocate_memory(sizeof(char *));
			if (empty_str)
			{
				empty_str[0] = '\0';
			}
			return (empty_str);
		}
		else
		{
			return (strdup(env_var->data));
		}
	}
	else
	{
		empty_str = allocate_memory(1);
		if (empty_str)
		{
			empty_str[0] = '\0';
		}
		return (empty_str);
	}
}

/* Processes a dollar sign expansion in the input string based on the environment variables.
   Returns an allocated string with the expanded value. */
char	*run_dollar_expansion(char *trimmed_input, t_tools *context)
{
	char			*trimmed_var;
	t_environment	*env_var;
	char			*empty_str;

	trimmed_var = trim_string(trimmed_input, "( )");
	env_var = ft_find(context->envair, trimmed_var);
	free(trimmed_var);
	if (!env_var)
	{
		empty_str = allocate_memory(1);
		if (empty_str)
		{
			empty_str[0] = '\0';
		}
		return (empty_str);
	}
	else
	{
		return (strdup(env_var->data));
	}
}

/* Handles the expansion of the $? expression in the input string.
   Returns a string representation of the last command's exit status. */
char	*handle_special_var(char *input, t_tools *context)
{
	int		status;
	char	*status_str;
	char	*result;

	status = context->last_status;
	status_str = ft_itoa(status);
	result = status_str;
	return (result);
}

/* Prepares the final result after expanding a variable value.
   Returns an allocated string with the processed value,
	ensuring no leading space remains. */
char	*finalize_expansion(char *expanded_var)
{
	char	*var_copy;
	char	*result;

	var_copy = copy_variable(expanded_var);
	result = var_copy;
	if (result[0] == ' ' && result[1] == '\0')
	{
		free(result);
		result = allocate_memory(1);
		if (result)
		{
			result[0] = '\0';
		}
	}
	return (result);
}

/* Expands dollar sign expressions in the input string based on context's environment.
   Returns an allocated string with the fully expanded result. */
char	*expand_dollar_signs(char *input, t_tools *context)
{
	char	*expanded_var;
	char	*result;
	char	*trimmed_input;

	expanded_var = NULL;
	result = NULL;
	if (input[1] == '?')
	{
		return (handle_special_var(input, context));
	}
	else
	{
		trimmed_input = trim_string(input, "$");
		if (trimmed_input[0] == '(')
		{
			expanded_var = handle_parentheses(trimmed_input, context);
		}
		else
		{
			expanded_var = run_dollar_expansion(trimmed_input, context);
		}
		free(trimmed_input);
		if (!expanded_var || *expanded_var == '\0')
		{
			return (expanded_var);
		}
		result = finalize_expansion(expanded_var);
	}
	return (result);
}
