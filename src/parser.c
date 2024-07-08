/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+
		+:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+
		+#+        */
/*                                                +#+#+#+#+#+
		+#+           */
/*   Created: 2024/05/30 21:51:20 by smoreron          #+#    #+#             */
/*   Updated: 2024/05/30 21:51:20 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/* Determines the type of redirection based on the substring from start to end
   in str.
   Returns the corresponding t_class value representing the redirection type. */
t_class	identify_redirection_type(char *str, int start, int end)
{
	int		length;
	char	start_char;

	length = end - start;
	start_char = str[start];
	if (length == 2)
	{
		if (start_char == '>')
			return (GREAT_GREAT);
		else if (start_char == '<')
			return (LESS_LESS);
	}
	else if (length == 1)
	{
		if (start_char == '>')
			return (GREAT);
		else if (start_char == '<')
			return (LESS);
	}
	return (UNKNOWN);
}

/* Checks if a given token is a redirection token.
   Returns 1 if it is a redirection token, otherwise returns 0. */
int	is_redirection_token(t_node *tkn)
{
	if (!tkn)
		return (0);
	if (tkn->class == LESS_LESS || tkn->class == GREAT_GREAT
		|| tkn->class == LESS || tkn->class == GREAT)
		return (1);
	return (0);
}

/* Frees the memory allocated for a single token. */
void	free_single_token(t_node *tkn)
{
	if (!tkn)
		return ;
	free(tkn->data);
	free(tkn);
}

/* Compares two strings for equality.
   Returns 1 if the strings are equal, otherwise returns 0. */
int	strings_equal(char *str1, char *str2)
{
	int		index;

	index = 0;
	if (!str1 || !str2)
		return (0);
	if (strlen(str1) != strlen(str2))
		return (0);
	while (str1[index] != '\0' || str2[index] != '\0')
	{
		if (str1[index] != str2[index])
			return (0);
		index++;
	}
	return (1);
}

/* Removes quotes from the input string if present.
   Returns the new string without quotes. */
char	*remove_quotes(char *input)
{
	char		*result;

	if (input[0] == '\'' || input[0] == '\"')
	{
		result = duplicate_string_range(input, 1, strlen(input) - 1);
		return (result);
	}
	return (strdup(input));
}

/* Processes tokens by removing quotes from their data.
   Skips processing for HERE_DOC tokens. */
void	manage_process_tokens(t_node *tokens)
{
	char		*temp;

	while (tokens != NULL)
	{
		if (tokens->class == LESS_LESS)
		{
			tokens = tokens->next;
			if (tokens != NULL)
				tokens = tokens->next;
			if (tokens == NULL)
				return ;
		}
		temp = remove_quotes(tokens->data);
		free(tokens->data);
		tokens->data = temp;
		tokens = tokens->next;
	}
}

/* Processes a command table by removing quotes from commands and arguments.
   Converts certain commands to lowercase. */
void	handle_command_table(t_simple_cmds *table, t_tools *tools)
{
	char	*temp;

	if (table->command != NULL)
	{
		temp = remove_quotes(table->command);
		free(table->command);
		table->command = temp;
		optimize_commands(table);
	}
	if (are_strings_equal(table->command, "echo") == 0
		&& are_strings_equal(table->command, "export") == 0)
		delit_quotes_command(table, tools);
}

/* Removes quotes from the command and arguments in a command table. */
void	delit_quotes_command(t_simple_cmds *table, t_tools *shell)
{
	manage_process_tokens(table->token_arg);
	manage_process_tokens(table->redirections);
}

/* Removes quotes from all command tables. */
void	delite_quotes_tables(t_simple_cmds *tables, t_tools *shell)
{
	while (tables != NULL)
	{
		handle_command_table(tables, shell);
		tables = tables->next;
	}
}

/* Calculates the number of tokens in a linked list of tokens.
   Returns the count of tokens. */
int	calculate_token_size(t_node *token)
{
	int		count;
	t_node	*current;

	count = 0;
	current = token;
	while (current != NULL)
	{
		count++;
		current = current->next;
	}
	return (count);
}

/* Counts the number of tokens in a linked list of tokens.
   Returns the count of tokens. */
int	calculate_token_length(t_node *token)
{
	int		i;

	i = 0;
	if (token == NULL)
	{
		return (0);
	}
	else
	{
		i = calculate_token_size(token);
		return (i);
	}
}

/* Checks if a character is a special ASCII character.
   Returns 1 if the character is a special ASCII character,
		otherwise returns 0.
 */
int	is_special_ascii(char c)
{
	int		ascii;

	ascii = (int)c;
	return ((ascii >= '!' && ascii <= '/') || (ascii >= ':' && ascii <= '@')
		|| ascii == '=' || (ascii >= '[' && ascii <= '`') || (ascii >= '{'
			&& ascii <= '~'));
}

/* Checks if a character is a visible character.
   Returns 1 if the character is visible, otherwise returns 0. */
int	is_visible_char(char c)
{
	int		i;

	i = is_special_ascii(c);
	if (i == 0)
		return (1);
	else
		return (0);
}

/* Creates a new token with the given content and type.
   Returns a pointer to the new token. */
t_node	*create_token(char *content, t_class type)
{
	t_node		*new_token;

	new_token = (t_node *)smalloc(sizeof(t_node));
	if (new_token == NULL)
	{
		return (NULL);
	}
	new_token->data = content;
	new_token->class = type;
	new_token->prev = NULL;
	new_token->next = NULL;
	return (new_token);
}

/* Creates a new token with the given string and type.
   Returns a pointer to the new token. */
t_node	*create_new_token(char *str, t_class type)
{
	return (create_token(str, type));
}

/* Inserts a new token into a linked list of tokens.
   Returns the head of the token list. */
t_node	*insert_token(t_node *head, char *str, t_class type)
{
	t_node		*tail;
	t_node		*new_token;

	if (head == NULL)
	{
		return (create_new_token(str, type));
	}
	tail = head;
	while (tail->next != NULL)
	{
		tail = tail->next;
	}
	new_token = create_token(str, type);
	if (new_token == NULL)
	{
		return (head);
	}
	tail->next = new_token;
	new_token->prev = tail;
	return (head);
}

/* Prints an error message. */
void	raise_error(char *msg)
{
	printf("%s\n", msg);
}

/* Allocates memory for command arguments.
   Returns the allocated array of strings or NULL if allocation fails. */
char	**allocate_cmd_args(char *cmd, t_node *token, int *size)
{
	int		initial_size;
	char	**args_array;

	initial_size = calculate_token_length(token);
	if (cmd == NULL)
	{
		*size = initial_size;
	}
	else
	{
		*size = initial_size + 1;
	}
	args_array = (char **)smalloc(sizeof(char *) * (*size + 1));
	if (args_array == NULL)
	{
		return (NULL);
	}
	args_array[*size] = NULL;
	return (args_array);
}

/* Extracts command arguments from the command and token list.
   Returns an array of command arguments. */
char	**extract_cmd_args(char *cmd, t_node *token)
{
	int		size;
	char	**cmd_args;
	int		index;

	cmd_args = allocate_cmd_args(cmd, token, &size);
	if (cmd_args == NULL)
		return (NULL);
	index = 0;
	if (cmd != NULL)
	{
		cmd_args[index++] = strdup(cmd);
	}
	while (token != NULL && index < size)
	{
		cmd_args[index++] = strdup(token->data);
		token = token->next;
	}
	return (cmd_args);
}

/* Initializes command arguments for all command tables. */
void	init_commands(t_simple_cmds *tables)
{
	while (tables != NULL)
	{
		tables->arguments = extract_cmd_args(tables->command,
				tables->token_arg);
		tables = tables->next;
	}
}

/* Splits a string into tokens.
   Returns the head of the token list. */
t_node	*tokenize_slip_string(char *str, t_node *token)
{
	int		current_index;
	int		previous_index;

	current_index = 0;
	while (str[current_index] != '\0')
	{
		previous_index = current_index;
		token = add_quote(str, &current_index, &previous_index, token);
		token = add_string(str, &current_index, &previous_index, token);
		token = plus_redire(str, &current_index, &previous_index, token);
		token = plus_token_f(str, &current_index, &previous_index, token);
		while (str[current_index] != '\0' && check_space(str[current_index]))
			current_index++;
	}
	return (token);
}

/* Counts the number of escaped characters before the given position in the
   string. Returns the count of escaped characters. */
int	calcul_escaped_charact(const char *str, int last_index)
{
	int		index;
	int		escape_count;

	index = last_index - 1;
	escape_count = 0;
	if (last_index <= 0)
		return (0);
	while (index >= 0)
	{
		if (str[index] == '\\')
		{
			escape_count++;
		}
		else
		{
			break ;
		}
		index--;
	}
	return (escape_count);
}

/* Processes a quoted substring and adds it as a token.
   Returns the updated token list. */
t_node	*add_quote(char *input_str, int *current_index, int *previous_index,
		t_node *token)
{
	char	quote_char;
	int		stop_search;

	if (input_str[*current_index] == '\0')
		return (token);
	if (input_str[*current_index] != '\'' && input_str[*current_index] != '\"')
		return (token);
	quote_char = input_str[(*current_index)++];
	stop_search = 0;
	while (input_str[*current_index] != '\0' && stop_search == 0)
	{
		if (input_str[*current_index] == quote_char
			&& calcul_escaped_charact(input_str, *current_index) % 2 == 0)
			stop_search = 1;
		(*current_index)++;
	}
	if (*current_index - 1 != *previous_index)
		token = insert_token(token, duplicate_string_range(input_str,
					*previous_index, *current_index), STRING);
	*previous_index = *current_index;
	return (token);
}

/* Duplicates a substring from start to end in str.
   Returns the duplicated substring. */
char	*duplicate_range(char *str, int start, int end)
{
	char	*new_str;

	new_str = (char *)malloc(end - start + 1);
	if (!new_str)
	{
		printf("Memory allocation error\n");
		return (NULL);
	}
	strncpy(new_str, str + start, end - start);
	new_str[end - start] = '\0';
	return (new_str);
}

/* Processes a string token and adds it to the token list.
   Returns the updated token list. */
t_node	*add_string(char *str, int *current_index, int *previous_index,
		t_node *token)
{
	int		new_index;
	char	*new_word;

	new_index = validate_and_advance(str, current_index);
	if (new_index != *previous_index)
	{
		new_word = duplicate_range(str, *previous_index, new_index);
		token = insert_token(token, new_word, STRING);
		*previous_index = new_index;
	}
	return (token);
}

/* Finds the position of a character in a string.
   Returns the index of the character or -1 if not found. */
int	find_char_in_str(const char *src, int character)
{
	int		index;

	index = 0;
	while (src[index] != '\0')
	{
		if (src[index] == (char)character)
		{
			return (index);
		}
		index++;
	}
	if (src[index] == (char)character)
	{
		return (index);
	}
	return (-1);
}

/* Determines the type of redirection based on the substring from begin to
   finish in input. Returns the corresponding t_class value representing the
   redirection type. */
t_class	determine_redirection_type(char *input, int begin, int finish)
{
	int		length;

	length = finish - begin;
	if (length == 2)
	{
		if (input[begin] == '>')
		{
			return (GREAT_GREAT);
		}
		return (LESS_LESS);
	}
	else if (length == 1)
	{
		if (input[begin] == '>')
		{
			return (GREAT);
		}
		return (LESS);
	}
	return (UNKNOWN);
}

/* Processes a redirection token and adds it to the token list.
   Returns the updated token list. */
t_node	*plus_redire(char *input_str, int *current_index, int *previous_index,
		t_node *token_list)
{
	char		current_char;
	t_class		redirection_type;
	char		*dubl;

	if (input_str[*current_index] == '\0' || find_char_in_str(REDIR,
			input_str[*current_index]) == -1)
	{
		return (token_list);
	}
	current_char = input_str[*current_index];
	while (input_str[*current_index] == current_char
		&& input_str[*current_index] != '\0')
	{
		(*current_index)++;
	}
	redirection_type = determine_redirection_type(input_str, *previous_index,
			*current_index);
	dubl = duplicate_range(input_str, *previous_index, *current_index);
	token_list = insert_token(token_list, dubl, redirection_type);
	*previous_index = *current_index;
	return (token_list);
}

/* Processes a flag token and adds it to the token list.
   Returns the updated token list. */
t_node	*process_flag_token(char *str, int *idx, int *prev_idx,
		t_node *token_list)
{
	char	*substring;
	int		start_idx;

	if (str[*idx] != FLAG)
	{
		return (token_list);
	}
	(*idx)++;
	start_idx = *idx;
	while (str[*idx] != '\0' && str[*idx] != SPACE)
	{
		if ((find_character_type(str[*idx]) & (IS_ALPHA | IS_DIGIT
					| IS_PRINTABLE)) == 0)
		{
			break ;
		}
		(*idx)++;
	}
	if (*idx > start_idx)
	{
		substring = duplicate_string_range(str, *prev_idx, *idx);
		token_list = insert_token(token_list, substring, 1);
	}
	*prev_idx = *idx;
	return (token_list);
}

/* Processes a flag token and adds it to the token list.
   Returns the updated token list. */
t_node	*plus_token_f(char *input_str, int *current_index, int *previous_index,
		t_node *token_list)
{
	int		idx;
	int		prev_idx;

	idx = *current_index;
	prev_idx = *previous_index;
	token_list = process_flag_token(input_str, &idx, &prev_idx, token_list);
	*current_index = idx;
	*previous_index = prev_idx;
	return (token_list);
}

/* Parses the input string into tokens and command tables.
   Returns 1 if an error occurs, otherwise returns 0. */
int	parse_shell_input(t_tools *shell)
{
	int				i;
	t_node			*token;
	t_simple_cmds	*cmd_tbls;

	i = 0;
	cmd_tbls = NULL;
	while (shell->pipes[i] != NULL)
	{
		token = NULL;
		token = tokenize_slip_string(shell->pipes[i++], token);
		cmd_tbls = create_command_table(cmd_tbls, token);
		clear_token_list(token);
	}
	multiple_tables_expand(cmd_tbls, shell);
	delite_quotes_tables(cmd_tbls, shell);
	init_commands(cmd_tbls);
	free_string_array(shell->pipes);
	shell->commands = cmd_tbls;
	if (audit_command(shell->commands))
		return (1);
	else
		return (0);
}

/* Audits the command tables to ensure they are valid.
   Returns TRUE if an error is found, otherwise returns FALSE. */
int	audit_command(t_simple_cmds *commands)
{
	t_node		*token;
	int			result;

	result = 0;
	while (commands != NULL)
	{
		token = commands->redirections;
		if (token == NULL)
			result = 1;
		while (token != NULL)
		{
			if (token->class == STRING)
				return (1);
			token = token->next;
		}
		commands = commands->next;
	}
	return (result);
}

/* Processes a substring from start to end, removing leading and trailing
   spaces. Returns the processed substring. */
char	*process_token_segment(char *str, int start, int end)
{
	char		*tmp;
	char		*trimmed;

	tmp = duplicate_string_range(str, start, end);
	trimmed = del_string(tmp, SIMBOL);
	free(tmp);
	return (trimmed);
}

void	free_tokens(char **tokens, int index)
{
	while (index >= 0)
	{
		free(tokens[index--]);
	}
	free(tokens);
}

int	process_and_store_token(char **tokens, const char *str, int start, int end, int *index)
{
	char	*token;

	token = process_token_segment(str, start, end);
	if (token == NULL)
	{
		free_tokens(tokens, *index);
		return (0);
	}
	tokens[++(*index)] = token;
	return (1);
}

/* Splits a string by pipe characters, respecting quotes.
//    Returns an array of substrings split by pipes. */
char	**pipe_split(char *str, int start, int end, int index)
	{
	char	**tokens;
	int		pipes_count;

	if (str == NULL)
		return (NULL);
	pipes_count = calc_pipe_segment(str);
	tokens = (char **)malloc(sizeof(char *) * (pipes_count + 2));
	if (tokens == NULL)
		return (NULL);
	while (str[++end] != '\0')
	{
		end = pass_quoted_sect(str, end);
		if (str[end] == '|' && str[end - 1] != '\''
			&& calcul_escaped_charact(str, end) % 2 == 0)
		{
			if (!process_and_store_token(tokens, str, start, end, &index))
				return (NULL);
			start = end + 1;
		}
	}
	if (!process_and_store_token(tokens, str, start, end, &index))
		return (NULL);
	tokens[++index] = NULL;
	return (tokens);
}

// char	**pipe_split(char *str, int start, int end, int index)
// {
// 	char **tokens;
// 	char *token;
// 	int pipes_count;

// 	if (str == NULL)
// 		return (NULL);
// 	pipes_count = calc_pipe_segment(str);
// 	tokens = malloc(sizeof(char *) * (pipes_count + 2));
// 	if (tokens == NULL)
// 		return (NULL);
// 	while (str[++end] != '\0')
// 	{
// 		end = pass_quoted_sect(str, end);
// 		if (str[end] == '|' && str[end - 1] != '\''
// 			&& calcul_escaped_charact(str, end) % 2 == 0)
// 		{
// 			token = process_token_segment(str, start, end);
// 			if (token == NULL)
// 			{
// 				while (index >= 0)
// 				{
// 					free(tokens[index--]);
// 				}
// 				free(tokens);
// 				return (NULL);
// 			}
// 			tokens[++index] = token;
// 			start = end + 1;
// 		}
// 	}
// 	token = process_token_segment(str, start, end);
// 	if (token == NULL)
// 	{
// 		while (index >= 0)
// 		{
// 			free(tokens[index--]);
// 		}
// 		free(tokens);
// 		return (NULL);
// 	}
// 	tokens[++index] = token;
// 	tokens[++index] = NULL;
// 	return (tokens);
// }

/* Ignores characters inside quotes when parsing.
   Returns the updated index after the closing quote. */
int	pass_quoted_sect(char *str, int index)
{
	int	quote;

	if (str[index] != 39 && str[index] != 34)
		return (index);
	quote = str[index++];
	while (str[index] != quote && calcul_escaped_charact(str, index) % 2 == 0)
	{
		if (str[index] == '\0')
			return (index);
		index++;
	}
	return (index);
}

/* Counts the number of pipes in a string, respecting quotes.
   Returns the count of pipes. */
int	calc_pipe_segment(char *str)
{
	int		i;
	int		count;

	count = 0;
	i = -1;
	if (str == NULL)
		return (1);
	while (str[++i] != '\0')
	{
		i = pass_quoted_sect(str, i);
		if ((size_t)i < ft_strlen(str) && str[i] != '\0' && str[i] == '|'
			&& calcul_escaped_charact(str, i) % 2 == 0)
			count++;
	}
	return (count + 1);
}
