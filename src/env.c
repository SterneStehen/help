/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 21:17:37 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/04 09:23:41 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"


char	*get_env_content(char *full_str, char *equals) {
  int full_idx;
  int content_idx;
  char *content;
  int content_length;
  int equals_len;

  equals_len = ft_strlen(equals);

  if (equals_len == ft_strlen(full_str) - 1) {
    // Если формат строки "VAR_NAME=", создаем пустое содержимое
    content = malloc(sizeof(char) * 2);
    if (content == NULL) {
      // Если выделение памяти не удалось, возвращаем NULL
      return (NULL);
    }
    content[0] = ' ';
    content[1] = '\0';
  } else if (equals_len == ft_strlen(full_str)) {
    // Если формат строки "VAR_NAME", содержимое отсутствует
    content = NULL;
  } else {
    // Вычисляем длину содержимого
    content_length = ft_strlen(full_str) - equals_len - 1;
    content = malloc(sizeof(char) * (content_length + 1));
    if (content == NULL) {
      // Если выделение памяти не удалось, возвращаем NULL
      return (NULL);
    }
    // Копируем содержимое из полной строки
    full_idx = equals_len + 1;
    content_idx = 0;
    while (full_str[full_idx + content_idx] != '\0') {
      content[content_idx] = full_str[full_idx + content_idx];
      content_idx++;
    }
    content[content_idx] = '\0';
  }
  return (content);
}



t_environment	*find_env_var(t_environment *head, char *var_name)
{
	t_environment	*curr;

	curr = head;
	while (curr != NULL)
	{
		if (ft_strncmp(curr->title, var_name, ft_strlen(var_name)) == 0
			&& ft_strlen(curr->title) == ft_strlen(var_name))
			return (curr);
		curr = curr->next;
	}
	return (NULL);
}


/* Retrieves the current directory from the PWD environment variable.
   If the PWD is root, returns "/",
	otherwise returns the last directory in the path. */
char	*get_current_directory(t_tools *shell)
{
	int				i;
	t_environment	*pwd;
	char			**split;
	char			*directory;

	i = 0;
	pwd = find_env_var(shell->envair, "PWD");
	if (ft_strcmp(pwd->data, "/") == TRUE)
		directory = ft_strdup("/");
	else
	{
		split = ft_split(pwd->data, '/');
		while (split[i] != NULL)
			i++;
		directory = ft_strdup(split[--i]);
		free_string_array(split);
	}
	return (directory);
}

/* Initializes a new environment variable node from the given string.
   Splits the string by '=' to separate the variable name and value. */
t_environment	*create_envairm(char *env_char)
{
	char			**equals;
	t_environment	*env;

	equals = ft_split(env_char, '=');
	if (equals == NULL)
		return (NULL);
	env = malloc(sizeof(t_environment));
	if (env == NULL)
	{
		free_string_array(equals);
		return (NULL);
	}
	env->title = ft_strdup(equals[0]);
	if (env->title == NULL)
	{
		free(env);
		free_string_array(equals);
		return (NULL);
	}
	env->data = get_env_content(env_char, equals[0]);
	if (env->data == NULL)
	{
		free(env->title);
		free(env);
		free_string_array(equals);
		return (NULL);
	}
	env->next = NULL;
	free_string_array(equals);
	return (env);
}

/* Adds a new environment variable node to the end of the list. */
void	append_env_node(t_environment *head, t_environment *new)
{
	t_environment	*curr;

	curr = head;
	while (curr->next != NULL)
		curr = curr->next;
	curr->next = new;
}

/* Initializes the environment variables from the given array.
   Splits the PATH variable into individual paths and initializes the environment list. */
int	setup_environment(t_tools *shell, char **env)
{
	int				i;
	t_environment	*head;
	t_environment	*new;
	char			*init_path;

	init_path = NULL;
	i = 0;
	while (env[i] != NULL)
	{
		if (strncmp(env[i], "PATH=", 5) == 0)
		{
			init_path = env[i] + 5;
			break ;
		}
		i++;
	}
	shell->paths = ft_split(init_path, ':');
	i = 0;
	if (env[0] != NULL)
	{
		head = create_envairm(env[i++]);
		while (env[i] != NULL)
		{
			new = create_envairm(env[i++]);
			append_env_node(head, new);
		}
		shell->envair = head;
	}
	else
	{
		shell->envair = NULL;
	}
	return (0);
}

/* Prints the environment variables if the shell's flag_log is set to TRUE.
   Handles cases where the environment variable's value is a single space character. */
void	display_env(t_tools *shell, char **args)
{
	t_environment	*curr;

	if (args[1] != NULL)
		return ;
	curr = shell->envair;
	if (shell->flag_log == TRUE)
	{
		while (curr != NULL)
		{
			shell->last_status = 0;
			if (curr->data != NULL)
			{
				if (curr->data[0] == ' ' && ft_strlen(curr->data) == 1)
				{
					printf("%s=\n", curr->title);
				}
				else
				{
					printf("%s=%s\n", curr->title, curr->data);
				}
			}
			curr = curr->next;
		}
	}
}

/* Converts the linked list of environment variables to an array of strings.
   Each string is formatted as "name=value". */
char	**convert_env_list_to_array(t_environment *env)
{
	char			*tmp;
	char			*full_env_str;
	char			**env_array;
	int				list_size;
	int				index;
	t_environment	*current_node;

	list_size = 0;
	current_node = env;
	while (current_node != NULL)
	{
		current_node = current_node->next;
		list_size++;
	}
	env_array = malloc(sizeof(char *) * (list_size + 1));
	if (env_array == NULL)
		return (NULL);
	env_array[list_size] = NULL;
	current_node = env;
	index = 0;
	while (current_node != NULL && index < list_size)
	{
		tmp = ft_strjoin(current_node->title, "=");
		if (current_node->data != NULL)
		{
			full_env_str = ft_strjoin(tmp, current_node->data);
			free(tmp);
		}
		else
		{
			full_env_str = tmp;
		}
		env_array[index] = full_env_str;
		current_node = current_node->next;
		index++;
	}
	return (env_array);
}

/* Updates the environment variables in the shell structure.
   Frees the existing environment array and converts the linked list to an array of strings. */
void	refresh_environment(t_tools *shell)
{
	if (shell->envp != NULL)
		free_string_array(shell->envp);
	shell->envp = convert_env_list_to_array(shell->envair);
}
