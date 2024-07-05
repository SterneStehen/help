/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 16:43:01 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/05 07:17:33 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	loop_minishell(t_tools *tools) {
  char *line;

  while (TRUE) {
    signals(&tools->terminal_setting);
    // read_line(shell);
    tools->flag_pipe = FALSE;
    tools->flag_ready_to_execute = FALSE;
    if (isatty(fileno(stdin)))
      tools->message = readline("minishell$ ");
    else {
      line = get_next_line(fileno(stdin));
      tools->message = trim_string(line, "\n");
      free(line);
    }
    tools->shell_string = trim_string(tools->message, " ");
    refresh_environment(tools);
    if (!audit_intput(tools) && run_lexer(tools) == 0 &&
        parse_shell_input(tools))
      run_exec_steps(tools, tools->commands);
    add_history(tools->message);
  }
}