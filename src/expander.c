/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 21:25:23 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/04 11:11:27 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../include/minishell.h"

/* Checks the number of single and double quotes up to index `i` in string `s`.
   Updates `single_quote` and `double_quote` accordingly.
   Returns the updated index `i`. */
int	validate_quotes(char *s, int i, int *single_quote, int *double_quote)
{
	while (i >= 0)
	{
		if (s[i] == '\'' && count_escaped_chars(s, i) % 2 == 0)
			(*single_quote)++;
		if (s[i] == '\"' && count_escaped_chars(s, i) % 2 == 0)
			(*double_quote)++;
		i--;
	}
	return (i);
}

/* Determines if expansion should fail based on the context around `pos` in `input_str`.
   `sq` is the count of single quotes, `dq` is the count of double quotes.
   Returns TRUE if expansion should fail, otherwise returns FALSE. */
int	fail_res_expand(char *input_str, int pos, int sq, int dq)
{
	//int	idx;
	int	initial_pos;

	//idx = pos;
	initial_pos = pos;
	pos = validate_quotes(input_str, pos, &sq, &dq);
	if (sq == 1 && dq == 0)
		return (TRUE);
	else if (sq == 1 && dq == 1 && input_str[initial_pos - 1] == '\"')
		return (TRUE);
	else if (sq == 2 && dq == 1)
		return (FALSE);
	else if (sq == 1 && dq == 2 && input_str[initial_pos - 1] == '\'')
		return (TRUE);
	else if (sq == 2 && dq == 2 && input_str[initial_pos - 1] == '\''
		&& input_str[initial_pos - 3] == '\'')
		return (TRUE);
	else if (sq == 4 && dq == 3)
		return (TRUE);
	return (FALSE);
}

/* Determines if expansion should be skipped based on the context around `pos` in `input_str`.
   Returns TRUE if expansion should be skipped, otherwise returns FALSE. */
int	skip_expans(char *input_str, int pos)
{
	int	temp_pos;
	int	single_quotes;
	int	double_quotes;
	int	need_backtrack;

	single_quotes = 0;
	double_quotes = 0;
	need_backtrack = FALSE;
	if (pos == 0)
		return (FALSE);
	temp_pos = pos;
	if (pos > 0 && (input_str[pos + 1] == '\"' || input_str[pos + 1] == '\''))
		return (TRUE);
	while (temp_pos != 0)
	{
		if (input_str[temp_pos] == '\'' || input_str[temp_pos] == '\"')
			need_backtrack = FALSE;
		temp_pos--;
	}
	if (need_backtrack == FALSE)
	{
		if (fail_res_expand(input_str, pos, double_quotes,
				single_quotes) == TRUE)
			return (TRUE);
	}
	return (FALSE);
}

/* Checks if the string `str` contains a dollar sign '$' that should trigger variable expansion.
   Updates `shell->flag_execution_completed` if a dollar sign is found followed by an invalid character.
   Returns TRUE if a valid dollar sign expansion is found,
	otherwise returns FALSE. */

bool	has_dollar_sign(char *str, t_tools *shell)
{
	int			i;
	const char	*spaces = " \t\n\v\r\f";
	int			j;
	bool		found_space;

	i = -1;
	while (str[++i] != '\0')
	{
		if (str[0] == '$' && str[i + 1] != ' ' && str[i + 1] != '\0')
			return (true);
		else if (str[i] == '$' && str[i + 1] == '\0')
			break ;
		else if ((str[i] == '$' && count_escaped_characters(str, i) % 2 != 0)
			|| (str[i] == '$' && skip_expans(str, i) == true))
			i++;
		else if (str[i] == '$')
		{
			j = 0;
			found_space = false;
			while (spaces[j] != '\0')
			{
				if (spaces[j] == str[i + 1])
				{
					found_space = true;
					break ;
				}
				j++;
			}
			if (found_space)
			{
				shell->flag_execution_completed = false;
				return (false);
			}
			else
				return (true);
		}
	}
	return (false);
}

/* Duplicates the substring of `s` starting from index `index` where a dollar sign ('$') is found.
   Handles different delimiters and expands the dollar sign segment before copying.
   Modifies `dst` to contain the duplicated substring. */
void	duplicate_dollar_segment(char **dst, char **s, int index)
{
	char	delimeter;
	int		i;
	int		doll;

	if (s[0][index + 1] == '?')
		dst[0] = duplicate_string_range(s[0], index, index + 2);
	else
	{
		i = index;
		doll = index + 1;
		delimeter = ' ';
		if (s[0][index + 1] == '(')
			delimeter = ')';
		if (index > 0 && s[0][index - 1] == '\'')
			delimeter = '\'';
		if (index > 0 && s[0][index - 1] == '\"')
			delimeter = '\"';
		while (s[0][i] != '\0' && s[0][i] != delimeter && ((s[0][doll] >= 48
					&& 57 >= s[0][doll]) || (s[0][doll] >= 65
					&& 122 >= s[0][doll])))
		{
			i++;
			doll++;
		}
		dst[0] = duplicate_string_range(s[0], index, i + 1);
	}
}

/* Isolates the segment of `s` containing the dollar sign ('$') for expansion.
   Splits `s` into `bef_doll` (portion before the dollar sign segment) and `rest` (portion after).
   Expands the dollar sign segment and updates `s` to reflect the expanded result. */
void isolate_dollar_segment(char **s, t_tools *sh, char **bef_doll, char **rest) {
    int i;
    char *doll;
    char *val;

    // Проверка на успешное выделение памяти
    if (!s || !*s) return;
    
    doll = malloc(sizeof(char) * 256); // Предполагаем, что максимальная длина переменной 256 символов
    if (!doll) {
        printf("Error: Memory allocation failed for doll.\n");
        return;
    }

    i = -1;
    while ((*s)[++i] != '\0') {
        if ((*s)[i] == '$' && count_escaped_characters(*s, i) % 2 == 0 && skip_expans(*s, i) != 1) {
            bef_doll[0] = duplicate_string_range(*s, 0, i);
            duplicate_dollar_segment(&doll, s, i);
            val = expand_dollar_signs(doll, sh);
            rest[0] = duplicate_string_range(*s, i + strlen(doll), strlen(*s));
            free(doll);
            doll = ft_strjoin(bef_doll[0], val);
            char *tmp = ft_strjoin(doll, rest[0]);
            free(bef_doll[0]);
            bef_doll[0] = tmp;
            free(doll);
            free(s[0]);
            *s = bef_doll[0];
            return;
        }
    }
    free(doll); // Освобождение памяти, если $ не найден
}

/* Copies the content of `content` into a newly allocated string `var_content`.
   Returns the pointer to the newly allocated string. */
char	*copy_variable_content(char *content)
{
	int		i;
	int		content_len;
	char	*var_content;

	i = -1;
	content_len = ft_strlen(content) + 1;
	var_content = malloc(sizeof(char) * content_len);
	while (content[++i] != '\0')
		var_content[i] = content[i];
	var_content[i] = '\0';
	return (var_content);
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

	while (has_dollar_sign(*input, context))
	{
        segment1 = (char **)allocate_memory(sizeof(char *));
        segment2 = (char **)allocate_memory(sizeof(char *));
		isolate_dollar_segment(input, context, segment1, segment2);
		free(segment2[0]);
		free(segment1);
		free(segment2);
	}
	return (1);
}


bool	strcmp_2(char *str1, char *str2)
{
	int		i;

	i = 0;
	if (str1 == NULL || str2 == NULL)
		return (FALSE);
	if (ft_strlen(str1) != ft_strlen(str2))
		return (FALSE);
	while (str1[i] != '\0' || str2[i] != '\0')
	{
		if (str1[i] != str2[i])
			return (FALSE);
		i++;
	}
	return (TRUE);
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
		return (strcmp_2(temp_str, "awk") == 0 ? 1 : 0);
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
		env_var = find_env_var(context->envair, trimmed_var);
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
	env_var = find_env_var(context->envair, trimmed_var);
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
