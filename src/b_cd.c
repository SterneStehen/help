/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   b_cd.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 07:06:03 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/05 07:40:49 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	count_words(char *input_str, char delimiter)
{
	int	in_word = 0, word_count = 0, index;

	in_word = 0, word_count = 0, index = 0;
	while (input_str[index] != '\0')
	{
		if (input_str[index] == delimiter)
		{
			in_word = 0;
		}
		else if (input_str[index] != delimiter && in_word == 0)
		{
			word_count++;
			in_word = -1;
		}
		index++;
	}
	return (word_count);
}


int	initialize_oldpwd(t_tools *shell)
{
	if (ft_find(shell->envair, "OLDPWD") == 0)
	{
		insert_new_variable(shell, "OLDPWD");
	}
	return (0);
}

int	update_environment_vars(t_tools *shell, char *old_pwd)
{
	char	*new_pwd;
	char	*pwd_combined;
	char	*old_pwd_combined;

	new_pwd = getcwd(NULL, PATH_MAX);
	pwd_combined = smalloc(strlen("PWD=") + strlen(new_pwd) + 1);
	old_pwd_combined = smalloc(strlen("OLDPWD=") + strlen(old_pwd) + 1);
	sprintf(pwd_combined, "PWD=%s", new_pwd);
	sprintf(old_pwd_combined, "OLDPWD=%s", old_pwd);
	alter_var_content(shell, old_pwd_combined, "OLDPWD");
	alter_var_content(shell, pwd_combined, "PWD");
	shell->last_status = shell->last_status;
	free(old_pwd_combined);
	free(pwd_combined);
	free(new_pwd);
	return (0);
}

int	execute_cd_home(t_tools *shell)
{
	t_environment	*home_dir;

	home_dir = ft_find(shell->envair, "HOME");
	if (!home_dir)
	{
		shell->last_status = 1;
		if (shell->flag_log)
		{
			printf("%scd: homeless\n", SHELL);
		}
	}
	else if (strlen(home_dir->data) < 1)
	{
		printf("\n");
	}
	else if (chdir(home_dir->data) == -1 && shell->flag_log)
	{
		printf("%scd: %s: %s\n", SHELL, home_dir->data, strerror(ENOENT));
	}
	return (0);
}


int	execute_cd_tilde(t_tools *shell, char *argument)
{
	t_environment	*home;

	char *path, *tilde_trimmed;
	if (shell->flag_envair == 0)
	{
		home = ft_find(shell->envair, "HOME");
		tilde_trimmed = trim_string(argument, "~");
		path = smalloc(strlen(home->data) + strlen(tilde_trimmed) + 1);
		sprintf(path, "%s%s", home->data, tilde_trimmed);
	}
	else
	{
		path = smalloc(strlen("/Users/") + strlen(shell->login_id) + 1);
		sprintf(path, "/Users/%s", shell->login_id);
	}
	if (chdir(path) == -1)
	{
		if (shell->flag_log)
		{
			return printf("%scd: %s: %s\n", SHELL, path, strerror(errno));
		}
	}
	free(tilde_trimmed);
	free(path);
	return (0);
}

int	execute_cd_oldpwd(t_tools *shell)
{
	t_environment	*old_pwd;

	old_pwd = ft_find(shell->envair, "OLDPWD");
	if (!old_pwd)
	{
		shell->last_status = 1;
		return printf("%scd: %s: %s\n", SHELL, "OLDPWD", strerror(errno));
	}
	else if (old_pwd->data == NULL)
	{
		printf("\n");
	}
	else if (chdir(old_pwd->data) == -1 && shell->flag_log)
	{
		return printf("%scd: %s: %s\n", SHELL, old_pwd->data, strerror(errno));
	}
	else if (shell->flag_log)
	{
		printf("%s\n", old_pwd->data);
	}
	return (0);
}

int	execute_cd_back(t_tools *shell, char **args)
{
	if (strcmp(args[1], "..") == 0 && args[2] == NULL)
	{
		chdir(args[1]);
	}
	else if (chdir(args[1]) == -1 && shell->flag_log)
	{
		return printf("%scd: %s: %s\n", SHELL, args[2], strerror(errno));
	}
	return (0);
}

int	update_directory_vars(t_tools *shell, char *old_pwd_content)
{
	t_environment	*pwd;
	t_environment	*oldpwd;

	pwd = ft_find(shell->envair, "PWD");
	if (pwd)
	{
		pwd->data = getcwd(NULL, 0);
	}
	oldpwd = ft_find(shell->envair, "OLDPWD");
	if (oldpwd)
	{
		oldpwd->data = old_pwd_content;
	}
	return (0);
}

int	change_directory(t_tools *shell, char *cmd, char **args)
{
	char *old_pwd_content;
	t_environment *old_pwd = ft_find(shell->envair, "PWD");
	old_pwd_content = old_pwd->data;

	if (args[1] == NULL)
	{
		execute_cd_home(shell);
	}
	else if (args[1][0] == '~')
	{
		execute_cd_tilde(shell, args[1]);
	}
	else if (are_strings_equal(args[1], "-"))
	{
		execute_cd_oldpwd(shell);
	}
	else if (args[1] != NULL && strcmp(args[1], "..") != 0 && args[1][0] != '-')
	{
		if (chdir(args[1]) == -1)
		{
			shell->last_status = 1;
			if (shell->flag_log)
			{
				return printf("%scd: %s: %s\n", SHELL, args[1], strerror(errno));
			}
		}
	}
	else if (strcmp(args[1], "..") == 0 || strncmp(args[1], "../", 3) == 0)
	{
		execute_cd_back(shell, args);
	}

	update_directory_vars(shell, old_pwd_content);
	return 0;
}