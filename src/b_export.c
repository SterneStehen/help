/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   b_export.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 07:08:56 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/08 04:59:51 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	calc_lengths(char *str1, char *str2)
{
	return (strlen(str1) - strlen(str2));
}

void	copy_content(char *src, char *dest, int start_idx)
{
	int	idx;

	idx = 0;
	while (src[start_idx + idx] != '\0')
	{
		dest[idx] = src[start_idx + idx];
		idx++;
	}
	dest[idx] = '\0';
}

// Main function to get environment content
char	*extract_env_data(char *input_str, char *separator)
{
	char	*result_content;
	int		sep_len;
	int		len_diff;
	int		content_size;

	sep_len = strlen(separator);
	if (sep_len == strlen(input_str) - 1)
	{
		result_content = smalloc(sizeof(char) * 2);
		if (result_content == NULL)
		{
			return (NULL);
		}
		result_content[0] = ' ';
		result_content[1] = '\0';
	}
	else if (sep_len == strlen(input_str))
	{
		result_content = NULL;
	}
	else
	{
		len_diff = calc_lengths(input_str, separator);
		content_size = len_diff - 1;
		result_content = smalloc(sizeof(char) * (content_size + 1));
		if (result_content == NULL)
		{
			return (NULL);
		}
		copy_content(input_str, result_content, sep_len + 1);
	}
	return (result_content);
}

void	insert_new_variable(t_tools *tools, char *source)
{
	t_environment	*new_var;
	t_environment	*current_env;

	new_var = create_envairm(source);
	if (tools->envair == NULL)
	{
		tools->envair = new_var;
	}
	else
	{
		current_env = tools->envair;
		while (current_env->next != NULL)
		{
			current_env = current_env->next;
		}
		current_env->next = new_var;
	}
}

int	compare_var_title(char *title, char *var)
{
	int	j;

	j = 0;
	while (title[j] || var[j])
	{
		if (title[j] != var[j])
			return (0);
		j++;
	}
	return (!title[j] && !var[j]);
}

int	update_var_data(t_tools *toolkit, t_environment *env_var, char *str,
		char *var)
{
	free(env_var->data);
	env_var->data = extract_env_data(str, var);
	toolkit->last_status = 0;
	return (0);
}

// Main function to replace variable content
int	alter_var_content(t_tools *toolkit, char *input_str, char *variable)
{
	t_environment	*current;

	current = toolkit->envair;
	while (current != NULL)
	{
		if (compare_var_title(current->title, variable))
		{
			update_var_data(toolkit, current, input_str, variable);
		}
		current = current->next;
	}
	return (0);
}

int	handle_error(t_tools *tools, const char *arg0, const char *arg)
{
	printf("%s%s: `%s': %s\n", MINI, arg0, arg, VAL);
	return (1);
}

// Function to process and print environment variables
void	print_variable(t_environment *env)
{
	if (env->data == NULL)
	{
		printf("declare -x %s\n", env->title);
	}
	else if (env->data[0] == ' ' && strlen(env->data) == 1)
	{
		printf("declare -x %s=\"\"\n", env->title);
	}
	else
	{
		printf("declare -x %s=\"%s\"\n", env->title, env->data);
	}
}

void	process_variable(t_tools *tools, t_environment *env)
{
	tools->last_status = 0;
	if (strcmp(env->title, "TERM") != 0)
	{
		print_variable(env);
	}
}

void	iterate_variables(t_tools *tools)
{
	t_environment	*env_node;

	env_node = tools->envair;
	while (env_node != NULL)
	{
		process_variable(tools, env_node);
		env_node = env_node->next;
	}
}

int	print_env_vars(t_tools *tools)
{
	if (tools == NULL || tools->envair == NULL)
	{
		fprintf(stderr, "No environment variables to display.\n");
		return (-1);
	}
	return (0);
}

// Function to validate and process arguments
int	process_args_run(t_tools *tools, char **args)
{
	int		i;
	int		plus;
	int		j;
	char	**split_var;

	i = 1;
	while (args[i] != NULL)
	{
		if (args[0][0] == '=' || ft_strlen(args[i]) == 0)
		{
			tools->last_status = 1;
			if (tools->flag_log == TRUE)
			{
				handle_error(tools, args[0], args[i]);
			}
			continue ;
		}
		if (args[i][0] == '-')
		{
			tools->last_status = 2;
		}
		else
		{
			plus = FALSE;
			j = 0;
			while (args[i][j] != '\0')
			{
				if (args[i][j] == '+')
				{
					if (args[i][j + 1] == '=')
					{
						plus = FALSE;
						break ;
					}
					plus = TRUE;
					break ;
				}
				j++;
			}
			if (isdigit(args[i][0]) || strchr(args[i], '\\')
				|| check_invalid_chars(args[i]) == TRUE || plus)
			{
				tools->last_status = 1;
				if (tools->flag_log == TRUE)
				{
					handle_error(tools, args[0], args[i]);
				}
			}
			else if (strchr(args[i], '=') != NULL)
			{
				split_var = ft_split(args[i], '=');
				if (ft_find(tools->envair, split_var[0]) == NULL)
				{
					insert_new_variable(tools, args[i]);
				}
				else
				{
					alter_var_content(tools, args[i], split_var[0]);
				}
				free_string_array(split_var);
			}
		}
		i++;
	}
	return (0);
}

// Main export function
int	export(t_tools *tools, char *command, char **args)
{
	if (args[1] == NULL && tools->flag_log == TRUE)
	{
		return (print_env_vars(tools));
	}
	else if (args[1] != NULL && tools->flag_pipe == FALSE)
	{
		return (process_args_run(tools, args));
	}
	else
	{
		tools->last_status = 1;
		if (tools->flag_log == 1)
			printf("%s: wrong arguments\n", MINI);
		return (1);
	}
	return (0);
}