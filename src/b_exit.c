/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   b_exit.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 07:08:17 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/05 07:08:38 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"


int log_error(char *command, char *msg) {
    printf("%s%s: %s\n", SHELL, command, msg);
    return 1;
}

int parse_exit_code(char *arg, int *sign) {
    int result = 0;
    int i = 0;
    
    if (arg[0] == '+') {
        i = 1;
    } else if (arg[0] == '-') {
        i = 1;
        *sign = -1;
    }

    while (arg[i] != '\0') {
        if (arg[i] < '0' || arg[i] > '9') {
            log_error(arg, "Numeric argument required");
            return 255;
        }
        result = (result * 10) + (arg[i] - '0');
        if (result > 255) {
            result %= 256;
        }
        i++;
    }
    
    if (*sign == -1) {
        result = 256 - result;
    }
    
    return result;
}

int process_exit_code(t_tools *tools, char **args) {
    int sign = 0;
    int result = parse_exit_code(args[1], &sign);
    tools->last_status = result;
    cleanup_before_exit(tools);
    exit(tools->last_status);
}

int handle_arguments(t_tools *tools, char *command, char **args) {
    if (args[1] == NULL) {
        cleanup_before_exit(tools);
        exit(0);
    }

    if (ft_strlen(args[1]) == 0) {
        if (tools->flag_log == 1) {
            log_error(command, "Too many arguments");
        }
        cleanup_before_exit(tools);
        exit(255);
    } else if (args[2] == NULL) {
        process_exit_code(tools, args);
    } else {
        if (tools->flag_log == 1) {
            log_error(command, "Too many arguments");
        }
        return 1;
    }

    return 0;
}

int exit_shell(t_tools *tools, char *command, char **args) {
    if (ft_strcmp(command, "exit") != 1) {
        return 1;
    }

    if (tools->flag_pipe == 0) {
        return handle_arguments(tools, command, args);
    } else if (args[2] == NULL && args[1] != NULL) {
        process_exit_code(tools, args);
    }
    
    return 0;
}
