/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 16:42:32 by smoreron          #+#    #+#             */
/*   Updated: 2024/05/28 16:42:32 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	main(int argc, char **argv, char **envp) {
  t_tools tools;

  if (argc < 1)
    return (0);
  setup_environment(&tools, envp);
  initialize_tools(&tools);
  loop_minishell(&tools);
  free_resources(&tools);
  exit(tools.last_status);
}
