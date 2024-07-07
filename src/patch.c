/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   patch.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 07:14:56 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/07 02:31:11 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/* Constructs the full path for a given command by combining the base path and the command.
   Allocates memory for the full path, which needs to be freed by the caller. */
char	*build_path(const char *base, const char *cmd)
{
	char	*slash_cmd;
	char	*full_path;

	slash_cmd = smalloc(strlen("/") + strlen(cmd) + 1);
	strcpy(slash_cmd, "/");
	strcat(slash_cmd, cmd);
	full_path = smalloc(strlen(base) + strlen(slash_cmd) + 1);
	strcpy(full_path, base);
	strcat(full_path, slash_cmd);
	free(slash_cmd);
	return (full_path);
}

/* Logs an error message and sets the last status code in the shell structure.
   Returns 0 and sets the last status code to the provided code. */
int	report_error_and_exit(t_tools *sh, const char *cmd, const char *msg,
		int code)
{
	if (sh->flag_log == 1)
	{
		printf("%c%s: %s\n", '$', cmd, msg);
		sh->flag_log = 0;
	}
	sh->last_status = code;
	return (0);
}

/* Finds the full path of a command by searching through the directories in the PATH environment variable.
   Returns the full path if the command is found and executable,
	otherwise logs an error and returns NULL. */
char	*find_command_path(t_tools *sh, const char *cmd)
{
	int		idx;
	char	*path;

	idx = 0;
	if (strlen(cmd) == 0)
		return (NULL);
	if (sh->paths == NULL)
		exit(EXIT_FAILURE);
	while (sh->paths[idx] != NULL)
	{
		path = build_path(sh->paths[idx], cmd);
		if (access(path, X_OK) == 0)
			return (path);
		free(path);
		idx++;
	}
	return (report_error_and_exit(sh, cmd, strerror(ENOENT), 127), NULL);
}

/* Checks if the given path is accessible and executable.
   Returns 1 if accessible, otherwise logs an error and returns 0. */
int	check_file_access(char *path, t_tools *sh)
{
	if (access(path, X_OK) == 0)
		return (1);
	else
	{
		sh->last_status = -1;
		if (sh->flag_log == 1)
			printf("%c%s: %s\n", '$', path, strerror(ENOENT));
			sh->flag_log = 0;
		return (0);
	}
}

/* Audits the command path to check if it is valid and accessible.
   Returns 1 if the path is valid and accessible,
	otherwise logs an error and returns 0. */
int	audit_comand_path(char *path, t_tools *sh)
{
	int	len;

	len = strlen(path);
	while (1)
	{
		if (path[0] == '.' && path[1] == '.' && path[2] == '/')
			return (check_file_access(path, sh));
		if (path[0] == '.' && path[1] == '/' && len == 2)
		{
			sh->last_status = 127;
			if (sh->flag_log == 1)
				printf("%c%s: %s\n", '$', sh->shell_string, strerror(ENOENT));
			return (0);
		}
		if (path[0] == '.' || path[0] == '/' || path)
		{
			if (path[0] == '.' && path[1] != '/')
			{
				sh->last_status = 127;
				if (sh->flag_log == 1)
					printf("%c%s: %s\n", '$', path, strerror(ENOENT));
				return (0);
			}
			else if (path[0] == '.' || path[0] == '/' || path)
				return (check_file_access(path, sh));
			else if (sh->flag_envair)
			{
				sh->last_status = 127;
				if (sh->flag_log == 1)
				{
					printf("%c%s: %s\n", '$', path, strerror(ENOENT));
					sh->flag_log = 0;
				}
				return (0);
			}
		}
		break ;
	}
	return (0);
}
