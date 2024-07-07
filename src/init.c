/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_shell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 22:52:41 by smoreron          #+#    #+#             */
/*   Updated: 2024/05/30 22:52:41 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"


/* Allocates memory of the specified size.
   Prints an error message and returns NULL if the allocation fails. */
void	*smalloc(size_t size) {
  void *ptr;

  ptr = malloc(size);
  if (ptr == NULL) {
    printf("Error: Memory allocation failed.\n");
    return (NULL);
  }
  return (ptr);
}

/* Initializes the shell tools structure with default values.
   Duplicates standard input and output file descriptors and gets terminal
   settings. */
void	initialize_tools(t_tools *shell) {
  shell->flag_envair = 0;
  shell->envp = NULL;
  shell->last_status = 0;
  shell->flag_log = FALSE;
  shell->content_hd = NULL;
  shell->commands = NULL;
  shell->shell_string = NULL;
  shell->flag_execution_completed = TRUE;
  shell->standard_input_fd = dup(0);
  shell->standard_output_fd = dup(1);
  shell->login_id = getenv("USER");
  shell->pipes = NULL;
  shell->message = NULL;
  shell->flag_pipe = 0;
  shell->flag_hd = 0;
  shell->requires_expansion = 0;
  shell->flag_ready_to_execute = 0;
  shell->flag_execution_completed = 1;
  shell->flag_log = 0;
  shell->current_pid = getpid();
  tcgetattr(0, &shell->terminal_setting); // Get current terminal settings
}

/* Allocates and initializes a new t_simple_cmds structure.
   Returns the newly allocated structure or NULL if allocation fails. */
t_simple_cmds	*create_empty_structure(void) {
  t_simple_cmds *command_table;

  command_table = smalloc(sizeof(t_simple_cmds));
  if (command_table == NULL)
    return (NULL);
  command_table->command = NULL;
  command_table->arguments = NULL;
  command_table->token_arg = NULL;
  command_table->redirections = NULL;
  command_table->hd_file_name = NULL;
  command_table->count = -1;
  command_table->next = NULL;
  command_table->prev = NULL;
  return (command_table);
}

/* Creates a copy of the given token.
   Allocates memory for the new token and copies data from the original token.
 */
t_node	*copy_list(t_node *original_token) {
  t_node *new_token;

  new_token = smalloc(sizeof(t_node));
  if (new_token == NULL)
    return (NULL);
  new_token->data = strdup(original_token->data);
  new_token->class = original_token->class;
  new_token->index = original_token->index;
  new_token->next = NULL;
  new_token->prev = NULL;
  return (new_token);
}

/* Appends a new token to the end of the token list.
   Returns the head of the token list. */
t_node	*append_token(t_node *token_list, t_node *new_token) {
  t_node *current_token;

  if (token_list == NULL)
    return (new_token);
  current_token = token_list;
  while (current_token->next != NULL)
    current_token = current_token->next;
  current_token->next = new_token;
  new_token->prev = current_token;
  return (token_list);
}

/* Handles redirection tokens by cloning and appending them to the command
   table's redirections list. Returns the next token in the list. */
t_node	*process_redirection(t_simple_cmds *cmd_tbl, t_node *token,
                            t_node *new_token) {
  new_token = copy_list(token);
  cmd_tbl->redirections = append_token(cmd_tbl->redirections, new_token);
  return (token->next);
}

/* Sets redirection tokens in the command table.
   Clones and appends redirection tokens to the redirections list. */
t_node	*install_redirection(t_simple_cmds *cmd_tbl, t_node *token) {
  t_node *new_token;

  new_token = NULL;
  if (token == NULL)
    return (token);
  if (is_redirection_token(token) == TRUE ||
      is_redirection_token(token->prev) == TRUE) {
    return (process_redirection(cmd_tbl, token, new_token));
  }
  return (token);
}

/* Clones the given token and appends it to the command table's token_arg list.
   Returns the next token in the list. */
t_node	*handle_new_token(t_simple_cmds *cmd_table, t_node *tok) {
  t_node *new_tok;

  new_tok = copy_list(tok);
  cmd_table->token_arg = append_token(cmd_table->token_arg, new_tok);
  return (tok->next);
}

/* Validates the given token for setting as a command argument.
   Returns 1 if valid, otherwise returns 0. */
int	check_token_validity(t_simple_cmds *cmd_table, t_node *tok) {
  if (tok == NULL)
    return (0);
  if (tok->class != STRING)
    return (0);
  if (is_redirection_token(tok->prev) == TRUE)
    return (0);
  if (cmd_table->command == NULL)
    return (0);
  return (1);
}

/* Sets the given token as a command argument in the command table.
   Returns the next token in the list. */
t_node	*set_arg_to_command(t_simple_cmds *cmd_tbl, t_node *token) {
  if (!check_token_validity(cmd_tbl, token)) {
    return (token);
  }
  return (handle_new_token(cmd_tbl, token));
}

/* Appends a new command table to the end of the command list.
   Sets the count of the new command table. */
t_simple_cmds	*extend_command_list(t_simple_cmds *command_list,
                                   t_simple_cmds *new_command) {
  t_simple_cmds *current;

  if (command_list == NULL) {
    new_command->count = 0;
    return (new_command);
  }
  current = command_list;
  while (current->next != NULL)
    current = current->next;
  current->next = new_command;
  new_command->count = current->count + 1;
  return (command_list);
}

/* Validates the given token for setting as a command.
   Returns 1 if valid, otherwise returns 0. */
int	validate_command(t_simple_cmds *cmd_table, t_node *tok) {
  if (tok == NULL)
    return (0);
  if (cmd_table->command != NULL)
    return (0);
  if (tok->class != STRING)
    return (0);
  if (is_redirection_token(tok->prev) == TRUE)
    return (0);
  return (1);
}

/* Sets the given token as the command in the command table.
   Returns the next token in the list. */
t_node	*set_command(t_simple_cmds *cmd_table, t_node *tok) {
  cmd_table->command = ft_strdup(tok->data);
  return (tok->next);
}

/* Sets commands and arguments in the command table.
   Validates and processes tokens to set as commands and arguments. */
t_node	*set_commands(t_simple_cmds *cmd_tbl, t_node *token) {
  if (!validate_command(cmd_tbl, token)) {
    return (token);
  }
  return (set_command(cmd_tbl, token));
}

/* Processes tokens to set commands, arguments,
        and redirections in the command table.
   Iterates through tokens and sets them accordingly. */
t_node	*process_tokens_run(t_simple_cmds *cmd, t_node *tokens) {
  t_node *current_token;

  current_token = tokens;
  while (current_token != NULL) {
    current_token = set_commands(cmd, current_token);
    current_token = set_arg_to_command(cmd, current_token);
    current_token = install_redirection(cmd, current_token);
  }
  return (current_token);
}

/* Creates a new command table from the token list and appends it to the command
   tables list.
   Processes tokens and sets commands, arguments, and redirections. */
t_simple_cmds	*create_command_table(t_simple_cmds *command_tables,
                                    t_node *token_list) {
  t_simple_cmds *new_command;


  if (token_list == NULL)
    return (command_tables);
  new_command = create_empty_structure();
  if (new_command == NULL)
    return (command_tables);
  process_tokens_run(new_command, token_list);
  command_tables = extend_command_list(command_tables, new_command);
  return (command_tables);
}
