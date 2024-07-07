/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 22:07:32 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/07 03:42:25 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../include/minishell.h"


void err_exit(t_tools *tools, char *command, char *arg, int flag)
{
    if (flag == 0)
    {
        printf("%s%s: %s\n", SHELL, arg, NAR);
        tools->last_status = 255;
    }
    else
    {
        printf("%s%s: %s\n", SHELL, command, TMA);
        tools->last_status = 1;
    }
}


/* Duplicates a substring from the source string from the given start to finish indices.
   Returns the duplicated string or NULL if memory allocation fails or indices are invalid. */
char	*duplicate_string_range(const char *source, int begin, int finish)
{
	int		length;
	char	*duplicate;
	int		i;

	if (begin > finish || begin < 0 || finish < 0 || source == NULL)
	{
		return (NULL);
	}
	length = finish - begin;
	duplicate = (char *)malloc(sizeof(char) * (length + 1));
	if (!duplicate)
	{
		return (NULL);
	}
	i = 0;
	while (i < length)
	{
		duplicate[i] = source[begin + i];
		i++;
	}
	duplicate[length] = '\0';
	return (duplicate);
}

/* Determines the type of the given character.
   Returns a constant indicating whether the character is a digit,
	an alphabet letter, or a printable character. */
int	find_character_type(int c)
{
	if (c >= '0' && c <= '9')
		return (IS_DIGIT);
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return (IS_ALPHA);
	if ((c >= 33 && c <= 47) || (c >= 58 && c <= 59) || (c >= 63 && c <= 64)
		|| c == 61 || (c >= 91 && c <= 96) || (c >= 123 && c <= 126))
		return (IS_PRINTABLE);
	return (0);
}

/* Advances the index through the string as long as the current character is a digit,
	alphabet letter, or printable character.
   Returns the updated index. */
int	validate_and_advance(char *str, int *index)
{
	while (str[*index] != '\0'
		&& (find_character_type(str[*index]) & (IS_DIGIT | IS_ALPHA | IS_PRINTABLE)))
		(*index)++;
	return (*index);
}

/* Compares two strings for equality.
   Returns 1 if the strings are equal, otherwise returns 0. */
int	are_strings_equal(const char *s1, const char *s2)
{
	int	i;

	i = 0;
	if (s1 == NULL || s2 == NULL)
		return (0);
	if (ft_strlen(s1) != ft_strlen(s2))
		return (0);
	while (s1[i] != '\0' || s2[i] != '\0')
	{
		if (s1[i] != s2[i])
			return (0);
		i++;
	}
	return (1);
}

/* Converts certain commands in the command table to lowercase if they match specific built-in commands.
   This is used to standardize commands such as "echo", "pwd", and "env". */
void	optimize_commands(t_simple_cmds *tbl)
{
	char	*temp;
	char	*cmd;

	temp = ft_strdup(tbl->command);
	cmd = tbl->command;
	convert_to_lowercase(temp, ft_strlen(temp));
	if (are_strings_equal(temp, "echo") || are_strings_equal(temp, "pwd")
		|| are_strings_equal(temp, "env"))
		convert_to_lowercase(cmd, ft_strlen(cmd));
	free(temp);
}

/* Advances the index past any whitespace characters in the string.
   Returns the updated index. */
int	advance_past_whitespace(const char *s, int idx)
{
	while (s[idx] != '\0' && check_space(s[idx]))
		idx++;
	return (idx);
}

/* Checks if the given character is a whitespace character.
   Returns 1 if the character is a whitespace character, otherwise returns 0. */
int	check_space(char ch)
{
	const char	*whitespace_chars = " \t\n\v\f\r";
	int			i;

	i = 0;
	while (whitespace_chars[i])
	{
		if (whitespace_chars[i] == ch)
			return (1);
		i++;
	}
	return (0);
}

/* Checks if the given character is an uppercase letter.
   Returns 1 if the character is uppercase, otherwise returns 0. */
int	is_capital_letter(char ch)
{
	return (ch >= 'A' && ch <= 'Z');
}

/* Converts all uppercase letters in the string to lowercase up to the given limit.
   Returns 1 on success. */
int	convert_to_lowercase(char *s, int limit)
{
	int	len;
	int	i;

	i = 0;
	len = strlen(s);
	while (i < len && i < limit)
	{
		if (is_capital_letter(s[i]))
			s[i] = tolower(s[i]);
		i++;
	}
	return (1);
}

/* Allocates memory for a pointer to a pointer and checks for allocation failure.
   Returns 1 on success, and 0 if memory allocation fails. */
int	safe_mall(char ***ptr, size_t size)
{
	*ptr = (char **)malloc(size);
	if (*ptr == NULL)
	{
		printf("Error: Memory allocation failed\n");
		return (0);
	}
	return (1);
}

/* Counts the number of strings in an array of strings.
   Returns the number of strings. */
int	count_strings(char **array)
{
	int	counter;

	counter = 0;
	while (array[counter] != NULL)
		counter++;
	return (counter);
}

/* Duplicates a given string.
   Returns the duplicated string or NULL if memory allocation fails. */
char	*duplicate_string(const char *str)
{
	char	*new_str;

	new_str = ft_strdup(str);
	if (new_str == NULL)
	{
		printf("Error: String duplication failed\n");
		return (NULL);
	}
	return (new_str);
}

/* Creates a deep copy of an array of strings.
   Returns the new array or NULL if memory allocation fails. */
char	**arr_dabl(char **array)
{
	int		index;
	int		num_strings;
	char	**new_array;

	if (array == NULL)
		return (NULL);
	num_strings = count_strings(array);
	if (!safe_mall(&new_array, sizeof(char *) * (num_strings + 1)))
		return (NULL);
	index = 0;
	while (array[index] != NULL)
	{
		new_array[index] = duplicate_string(array[index]);
		if (new_array[index] == NULL)
		{
			while (--index >= 0)
				free(new_array[index]);
			free(new_array);
			return (NULL);
		}
		index++;
	}
	new_array[index] = NULL;
	return (new_array);
}

/* Trims characters from the set at the beginning and end of the string s1.
   Returns the trimmed string or NULL if memory allocation fails. */
char	*del_string(char const *s1, char const *set)
{
	char	*result;
	size_t	i;
	size_t	begin;
	size_t	finish;
	size_t	j;

	if (!s1 || !set)
		return (NULL);
	begin = 0;
	while (s1[begin])
	{
		j = 0;
		while (set[j] && set[j] != s1[begin])
			j++;
		if (!set[j])
			break ;
		begin++;
	}
	finish = strlen(s1);
	while (finish > begin)
	{
		j = 0;
		while (set[j] && set[j] != s1[finish - 1])
			j++;
		if (!set[j])
			break ;
		finish--;
	}
	result = (char *)malloc(sizeof(*s1) * (finish - begin + 1));
	if (!result)
		return (NULL);
	i = 0;
	while (begin < finish)
		result[i++] = s1[begin++];
	result[i] = '\0';
	return (result);
}

/* Checks if the given string contains only space characters.
   Returns 1 if the string contains only spaces, otherwise returns 0. */
int	is_only_space(char *str)
{
	int	i;

	i = -1;
	while (str[++i] != '\0')
	{
		if (str[i] != ' ')
			return (0);
	}
	return (1);
}

/* Compares a string with another string up to the given length.
   Returns 1 if the strings are equal, otherwise returns 0. */
int	run_strncmp(t_tools *shell, char *str, int s)
{
	if (ft_strncmp(shell->shell_string, str, s))
		return (1);
	return (0);
}
