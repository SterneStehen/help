/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   patch.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 07:14:56 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/09 05:27:00 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/* Constructs the full path for a given command by combining
the base path and the command.
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

/* Finds the full path of a command by searching through 
the directories in the PATH environment variable.
   Returns the full path if the command is found and executable,
	otherwise logs an error and returns NULL. */
char	*find_command_path(t_tools *tools, const char *cmd)
{
	int		idx;
	char	*path;

	idx = 0;
	if (strlen(cmd) == 0)
		return (NULL);
	if (tools->paths == NULL)
		exit(EXIT_FAILURE);
	while (tools->paths[idx] != NULL)
	{
		path = build_path(tools->paths[idx], cmd);
		if (access(path, X_OK) == 0)
			return (path);
		free(path);
		idx++;
	}
	return (NULL);
}

// /* Checks if the given path is accessible and executable.
//    Returns 1 if accessible, otherwise logs an error and returns 0. */
// int	check_file_access(char *path, t_tools *sh)
// {
// 	if (access(path, X_OK) == 0)
// 		return (1);
// 	else if(sh->envp != NULL && sh->envp[0] != NULL)
// 	{
// 		sh->last_status = -1;
// 		if (sh->flag_log == 1)
// 		{
// 			printf("%c%s: %s\n", '$', path, strerror(ENOENT));
// 			sh->flag_log = 0;
// 		}
// 		return (0);
// 	}
// 	return (0);
// }

int	log_command(t_tools *params, char *cmd, int err_flag)
{
	if (params->flag_log == 1)
	{
		if (err_flag)
		{
			printf("%s%s: %s\n", MINI, cmd, strerror(ENOENT));
		}
		else
		{
			printf("%s%s: %s\n", MINI, cmd, "command not found");
		}
	}
	params->last_status = 127;
	return (0);
}

int	check_dot_slash(char *route, t_tools *params)
{
	if (route[0] == '.' && route[1] == '/' && strlen(route) == 2)
	{
		return (log_command(params, params->shell_string, 1));
	}
	return (-1);
}

int	check_double_dot(char *route, t_tools *params)
{
	if (route[0] == '.' && route[1] == '.' && route[2] == '/')
	{
		if (access(route, X_OK) == 0)
		{
			return (1);
		}
		else
		{
			return (log_command(params, route, 1));
		}
	}
	return (-1);
}

int	check_paths(char *route, t_tools *params)
{
	if (route[0] == '.' || route[0] == '/' || route)
	{
		if (route[0] == '.' && route[1] != '/')
		{
			return (log_command(params, route, 0));
		}
		else if (route[0] == '.' && access(route, X_OK) == 0)
		{
			return (1);
		}
		else if (route[0] == '/' && access(route, X_OK) == 0)
		{
			return (1);
		}
		else if (access(route, X_OK) == 0)
		{
			return (1);
		}
		else if (params->flag_envair == 1)
		{
			return (log_command(params, route, 1));
		}
	}
	return (0);
}

int	path_audit(char *route, t_tools *params)
{
	int	result;

	result = check_dot_slash(route, params);
	if (result != -1)
		return (result);
	result = check_double_dot(route, params);
	if (result != -1)
		return (result);
	return (check_paths(route, params));
}

// int path_audit(char *path, t_tools *tools) {
//     void handle_command(t_tools *tools, char *command, int not_found) {
//         if (tools->flag_log == 1) {
//             if (not_found) {
//                 printf("%s%s: %s\n", SHELL, command, strerror(ENOENT));
//             } else {
//                 printf("%s%s: %s\n", SHELL, command, "command not found");
//             }
//         }
//         tools->last_status = 127;
//     }

//     // Checking for ".." at path start
//     if (path[0] == '.' && path[1] == '.' && path[2] == '/') {
//         if (access(path, X_OK) == 0)
//             return 1;
//         else {
//             handle_command(tools, path, 1);
//             return 0;
//         }
//     }

//     // Checking for "./" at path start
//     if (path[0] == '.' && path[1] == '/' && strlen(path) == 2) {
//         handle_command(tools, tools->shell_string, 1);
//         return 0;
//     }

//     // Checking for "." or "/" or any path
//     if (path[0] == '.' || path[0] == '/' || path) {
//         if (path[0] == '.' && path[1] != '/') {
//             handle_command(tools, path, 0);
//             return 0;
//         } else if (path[0] == '.' && access(path, X_OK) == 0) {
//             return 1;
//         } else if (path[0] == '/' && access(path, X_OK) == 0) {
//             return 1;
//         } else if (access(path, X_OK) == 0) {
//             return 1;
//         } else if (tools->flag_envair == 1) {
//             handle_command(tools, path, 1);
//             return 0;
//         }
//     }

//     return 0;
// }

// int	audit_comand_path(char *path, t_tools *sh)
// {
// 	int	len;

// 	len = strlen(path);
// 	while (1)
// 	{
// 		if (path[0] == '.' && path[1] == '.' && path[2] == '/')
// 			return (check_file_access(path, sh));
// 		if (path[0] == '.' && path[1] == '/' && len == 2)
// 		{
// 			sh->last_status = 127;
// 			if (sh->flag_log == 1)
// 				printf("%c%s: %s\n", '$', sh->shell_string, "3");
// 			return (0);
// 		}
// 		if (path[0] == '.' || path[0] == '/' || path)
// 		{
// 			if (path[0] == '.' && path[1] != '/')
// 			{
// 				sh->last_status = 127;
// 				if (sh->flag_log == 1)
// 					printf("%c%s: %s\n", '$', path, "2");
// 				return (0);
// 			}
// 			else if (path[0] == '.' || path[0] == '/' || path)
// 				return (check_file_access(path, sh));
// 			else if (sh->flag_envair)
// 			{
// 				sh->last_status = 127;
// 				if (sh->flag_log == 1)
// 				{
// 					printf("%c%s: %s\n", '$', path, "1");
// 					sh->flag_log = 0;
// 				}
// 				return (0);
// 			}
// 		}
// 		break ;
// 	}
// 	return (0);
// }
